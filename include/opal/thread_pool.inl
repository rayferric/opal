namespace opal {

thread_pool &thread_pool::common_pool() {
	static thread_pool pool;
	return pool;
}

thread_pool::thread_pool(uint32_t thread_count) {
	// If the thread count is zero, use the number of system threads.
	if (thread_count == 0) {
		thread_count = std::thread::hardware_concurrency();
	}

	// If the thread count is still zero, use a default value.
	if (thread_count == 0) {
		thread_count = 8;
	}

	// Create worker threads.
	threads.reserve(thread_count);

	// Initialize the threads.
	for (uint32_t i = 0; i < thread_count; i++) {
		// Create a thread that will execute the process function.
		threads.emplace_back(&thread_pool::process, this, i, nullptr);

		// Map thread id to its index.
		thread_id_to_index.emplace(threads.back().get_id(), i);
	}
}

thread_pool::~thread_pool() {
	// Threads will exit as soon as there is no more work to pick up.
	terminating.test_and_set();
	notifier.notify_all();

	// Wait for threads that still might be processing.
	for (std::thread &thread : threads) {
		thread.join();
	}
}

std::shared_ptr<future> thread_pool::submit(std::function<void(uint32_t)> job) {
	// Create a future that will be used to wait for the job to finish.
	std::shared_ptr<opal::future> future{new opal::future(*this)};

	{
		// Lock the queue.
		std::unique_lock lock{queue_mutex};

		// Add the job to the queue.
		queue.push(job_item{std::move(job), future});

		// Notify a thread that there is work to pick up.
		// We do that while the queue is locked so that we don't end up
		// notifying a thread once the job was randomly picked up by another
		// thread.
		notifier.notify_one();
	}

	return future;
}

uint32_t thread_pool::thread_count() const {
	return threads.size();
}

void thread_pool::process(
    uint32_t thread_index, const future *quit_when_ready
) {
	// This is the function that worker threads will execute.

	// job that is going to be executed; Declared outside the loop so that it
	// doesn't get reallocated every time.
	std::function<void(uint32_t thread_index)> job;

	// The future that will be used to notify threads waiting for the job to
	// finish.
	std::shared_ptr<future> future;

	while (true) {
		{
			// Lock the queue.
			std::unique_lock lock(queue_mutex);

			// Wait for work to pick up.
			notifier.wait(lock, [&] {
				return !queue.empty() || terminating.test() ||
				       (quit_when_ready != nullptr && quit_when_ready->ready());
			});

			// If there's no more work to pick up and termination was requested
			if (queue.empty() && terminating.test()) {
				break;
			}

			// Or when the quit_when_ready future is ready
			if (quit_when_ready != nullptr && quit_when_ready->ready()) {
				break;
			}

			// Pop the job and the future from the queue.
			job    = std::move(queue.front().job);
			future = std::move(queue.front().future);
			queue.pop();
		}

		try {
			// Execute the job.
			job(thread_index);
		} catch (...) {
			// If the job threw an exception, store it in the future.
			future->exception = std::current_exception();
		}

		// Mark the future as ready.
		future->_ready.test_and_set();

		// Notify all the threads that are waiting for the future to be ready.
		future->_ready.notify_all();

		// Notify all the threads so that quit_when_ready can be honored.
		notifier.notify_all();
	}
}

bool future::ready() const noexcept {
	return _ready.test();
}

void future::wait() const {
	// Quit if the future is already down.
	if (ready()) {
		return;
	}

	// Find the thread id of the current thread.
	std::thread::id id = std::this_thread::get_id();

	// If the current thread is one of the worker threads, then continue doing
	// some other work.
	if (pool.thread_id_to_index.contains(id)) {
		// Continue some other work if it's a job on a worker thread that
		// requested to wait.
		pool.process(pool.thread_id_to_index.at(id), this);
	} else {
		// If it's an external thread that requested to wait, then pause until
		// future is ready.
		_ready.wait(false);
	}
}

void future::rethrow() const {
	// Wait for the future to be ready.
	wait();

	// If the future threw an exception, rethrow it.
	if (exception != nullptr) {
		std::rethrow_exception(exception);
	}
}

future::future(thread_pool &pool) : pool(pool) {}

} // namespace opal
