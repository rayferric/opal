#include <algorithm> // std::move(..., ..., ...)
#include <cmath>
#include <cstdlib>  // size_t
#include <iterator> // std::distance, std::back_inserter
#include <utility>  // std::move(...)
#include <vector>   // std::vector

namespace opal::_internal {

// the merge sort algorithm
template <iterator Iterator, comparator<Iterator> Comparator>
class merge_sort {
public:
	// Constructs the algorithm context and begins the sorting process.
	merge_sort(
	    thread_pool      *pool,
	    Iterator          begin,
	    Iterator          end,
	    const Comparator &comparator
	);

private:
	// the maximum size of a range that is to be sorted using the insertion
	// approach; 64 is the sweet spot
	static constexpr size_t insertion_sort_threshold = 64;

	// the maximum size of a range that is to be always sorted using a single
	// thread; It's needed because for smaller branches it's better to use a
	// single job. This parameter only affects sorting of very short arrays at
	// couple thousand elements.
	static constexpr size_t min_single_threaded_threshold = 1024;

	// The type of elements in the range.
	using value_type = typename Iterator::value_type;

	// optional thread pool
	thread_pool *pool;

	// the maximum size of a range that is to be sorted using a single thread
	// Tuned dynamically to the size of the array and the number of threads.
	size_t single_threaded_threshold;

	// comparator predicate
	const Comparator &comparator;

	// the beginning of the full range
	// Needed to calculate the offset of the working area in the merge buffer.
	Iterator begin;

	// merge buffer
	std::vector<value_type> buffer;

	// Recursively sorts the range [begin, end). (O(n log n))
	void branch(Iterator begin, Iterator end);

	// Merges two sorted ranges [begin, middle) and [middle, end) into one
	// sorted range. (O(n))
	void merge(Iterator begin, Iterator middle, Iterator end);
};

template <iterator Iterator, comparator<Iterator> Comparator>
merge_sort<Iterator, Comparator>::merge_sort(
    thread_pool      *pool,
    Iterator          begin,
    Iterator          end,
    const Comparator &comparator
)
    : pool(pool), comparator(comparator), begin(begin) {
	// Reserve enough space in the merge buffer to hold the entire range.
	buffer.resize(std::distance(begin, end));

	// Tune multithreading to the given array.
	if (pool) {
		// number of same-level branches that is slightly above the number of
		// threads
		uint32_t number_of_jobs =
		    std::exp2(std::ceil(std::log2(pool->thread_count())));

		// Split the range into the given number of jobs.
		// There will also be some additional jobs on lower levels of recursion,
		// but those won't hurt the performance. They will actually help by
		// merging the final results using a progressively smaller number of
		// threads.
		single_threaded_threshold = buffer.size() / number_of_jobs;

		// Make sure that the threshold is not too small, because for smaller
		// branches it's better to use a single job.
		single_threaded_threshold =
		    std::max(single_threaded_threshold, min_single_threaded_threshold);
	}

	// Begin the first level of recursion.
	branch(begin, end);
}

template <iterator Iterator, comparator<Iterator> Comparator>
void merge_sort<Iterator, Comparator>::branch(Iterator begin, Iterator end) {
	size_t size = std::distance(begin, end);

	// Abort if the range is empty or contains only one element.
	if (size <= 1) {
		return;
	}

	// Use insertion sort for shorter ranges.
	if (size <= insertion_sort_threshold) {
		opal::insertion_sort(begin, end, comparator);
		return;
	}

	// Split range into two halves.
	Iterator middle = begin + (size / 2);

	// Use single-threaded merge sort for shorter ranges.
	if (pool == nullptr || size <= single_threaded_threshold) {
		// Sort each of the two half-ranges.
		branch(begin, middle);
		branch(middle, end);
	} else {
		// Offload the sorting of each of the two branches to a separate thread.
		auto lfuture = pool->submit([&](uint32_t) {
			branch(begin, middle);
		});
		auto rfuture = pool->submit([&](uint32_t) {
			branch(middle, end);
		});

		// Wait for the threads to finish.
		lfuture->wait();
		rfuture->wait();
	}

	// Merge the two sorted ranges.
	merge(begin, middle, end);
}

template <iterator Iterator, comparator<Iterator> Comparator>
void merge_sort<Iterator, Comparator>::merge(
    Iterator begin, Iterator middle, Iterator end
) {
	// Check if the two ranges are possibly already sorted.
	if (comparator(*(middle - 1), *middle)) {
		return;
	}

	// Declare two iterators that indicate the current position in each of the
	// two ranges, [begin, middle) and [middle, end).
	Iterator left  = begin;
	Iterator right = middle;

	// Calculate the offset of the working area in the merge buffer.
	// This offsetting is needed for multithreading.
	Iterator buffer_begin = buffer.begin() + std::distance(this->begin, begin);

	// current position in the merge buffer
	Iterator buffer_it = buffer_begin;

	// Pick smallest element from both ranges and move it into the merge buffer.
	while (left != middle && right != end) {
		// It is important to use left range in the else block, because
		// comparator returns false if the elements are equal.
		// This ensures that the merge sort algorithm is stable as elements from
		// the left range are always ordered before elements from the right
		// range.
		if (comparator(*right, *left)) {
			// buffer.push_back(std::move(*right));
			*buffer_it = std::move(*right);
			right++;
		} else {
			*buffer_it = std::move(*left);
			left++;
		}

		buffer_it++;
	}

	// Move remaining elements from one of the ranges into the merge buffer.
	buffer_it = std::move(left, middle, buffer_it);
	buffer_it = std::move(right, end, buffer_it);

	// At this point buffer_it points to the end of the working area in the
	// buffer.

	// Move elements from merge buffer back into the original range.
	std::move(buffer_begin, buffer_it, begin);
	buffer.clear();
}

} // namespace opal::_internal

namespace opal {

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void merge_sort(Iterator begin, Iterator end, Comparator comparator) {
	_internal::merge_sort<Iterator, Comparator>{
	    nullptr, begin, end, comparator};
}

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void merge_sort(
    thread_pool &pool, Iterator begin, Iterator end, Comparator comparator
) {
	_internal::merge_sort<Iterator, Comparator>{&pool, begin, end, comparator};
}

} // namespace opal
