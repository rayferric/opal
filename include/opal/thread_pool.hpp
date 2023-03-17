#pragma once

#include <atomic>             // std::atomic_flag
#include <condition_variable> // std::condition_variable
#include <cstdint>            // uint32_t
#include <exception>          // std::exception_ptr
#include <functional>         // std::function
#include <memory>             // std::shared_ptr
#include <queue>              // std::queue
#include <thread>             // std::thread

namespace opal {

class future;

///
/// @brief A thread pool that can be used to execute jobs in parallel.
/// Access from jobs submitted to this pool does not require synchronization.
/// Access from any other thread requires synchronization.
///
class thread_pool {
public:
	///
	/// @brief Returns a reusable thread pool that can be used to execute jobs
	/// in parallel. The thread pool is created with the number of threads equal
	/// to the number of hardware threads.
	///
	/// @return thread pool
	///
	static thread_pool &common_pool();

	///
	/// @brief Creates a thread pool with the specified number of threads.
	/// If the thread count is either omitted or zero, the number of hardware
	/// threads is used.
	///
	/// @param thread_count number of threads
	///
	thread_pool(uint32_t thread_count = 0);

	///
	/// @brief Destroys the thread pool.
	/// All threads are terminated and the destructor waits for all threads to
	/// finish.
	///
	~thread_pool();

	///
	/// @brief Submits a job to the thread pool.
	/// The job will be executed by one of the threads in the pool.
	///
	/// @param job job to execute
	///
	/// @return future
	///
	std::shared_ptr<future> submit(std::function<void(uint32_t)> job);

	///
	/// @brief Returns the number of threads in the pool.
	///
	/// @return number of threads
	///
	uint32_t thread_count() const;

private:
	struct job_item {
		std::function<void(uint32_t)> job;
		std::shared_ptr<opal::future> future;
	};

	std::vector<std::thread>                      threads;
	std::unordered_map<std::thread::id, uint32_t> thread_id_to_index;
	std::queue<job_item>                          queue;

	std::mutex              queue_mutex;
	std::condition_variable notifier;
	std::atomic_flag        terminating;

	void process(uint32_t thread_index, const future *quit_when_ready);

	friend future;
};

///
/// @brief A future that can be used to wait for a job to finish or to rethrow
/// any exceptions that were thrown by the job.
///
class future {
public:
	///
	/// @brief Checks whether the job has finished.
	///
	/// @return true if the job has finished, false otherwise
	///
	bool ready() const noexcept;

	///
	/// @brief Waits for the job to finish.
	///
	void wait() const;

	///
	/// @brief Waits for the job to finish and rethrows any exceptions that were
	/// thrown by the job.
	///
	void rethrow() const;

private:
	thread_pool       &pool;
	std::atomic_flag   _ready;
	std::exception_ptr exception = nullptr;

	future(thread_pool &pool);

	friend thread_pool;
};

} // namespace opal

#include "thread_pool.inl"
