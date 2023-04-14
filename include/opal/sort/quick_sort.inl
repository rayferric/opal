#include <algorithm> // std::swap
#include <cmath>     // std::log2
#include <cstdlib>   // std::size_t
#include <iterator>  // std::distance
#include <vector>    // std::vector

#include "heap_sort.hpp"

namespace opal::_internal {

// partition pivot with optional width
template <iterator Iterator>
class pivot {
public:
	// Left split does not include begin.
	// Right split includes end.
	Iterator begin, end;
};

// the merge sort algorithm
template <iterator Iterator, comparator<Iterator> Comparator>
class quick_sort {
public:
	// Constructs the algorithm context and begins the sorting process.
	quick_sort(
	    thread_pool      *pool,
	    Iterator          begin,
	    Iterator          end,
	    const Comparator &comparator
	);

private:
	// the maximum size of a range that is to be sorted using the insertion
	// approach; 64 is the sweet spot.
	static constexpr std::size_t insertion_sort_threshold = 64;

	// determines the maximum size of a range that is to be sorted using the
	// heap sort approach; threshold = log2(n) * heap_sort_threshold_factor
	static constexpr std::size_t heap_sort_threshold_factor = 2;

	// the maximum size of a range that is to be always sorted using a single
	// thread; It's needed because for smaller branches it's better to use a
	// single job.
	static constexpr std::size_t min_single_threaded_threshold = 1024;

	// NOTE: Insertion sort overrides heap sort and heap sort overrides
	// single-threaded quick sort.

	// optional thread pool
	thread_pool *pool;

	// the maximum size of a range that is to be sorted using heap sort
	// Tuned dynamically to the size of the array.
	std::size_t heap_sort_threshold;

	// the maximum size of a range that is to be sorted using a single thread
	// Tuned dynamically to the size of the array and the number of threads.
	std::size_t single_threaded_threshold;

	// comparator predicate
	const Comparator &comparator;

	// Recursively sorts the range [begin, end). (O(n log n))
	void branch(Iterator begin, Iterator end);

	// Partitions [begin, end). (O(n))
	pivot<Iterator> partition(Iterator begin, Iterator end);
};

template <iterator Iterator, comparator<Iterator> Comparator>
quick_sort<Iterator, Comparator>::quick_sort(
    thread_pool      *pool,
    Iterator          begin,
    Iterator          end,
    const Comparator &comparator
)
    : pool(pool), comparator(comparator) {
	std::size_t size = std::distance(begin, end);

	// Tune heap sort threshold.
	heap_sort_threshold = std::log2(size) * heap_sort_threshold_factor;

	// Tune multithreading threshold.
	if (pool) {
		// number of same-level branches that is slightly above the number of
		// threads
		std::uint32_t number_of_jobs =
		    std::exp2(std::ceil(std::log2(pool->thread_count())));

		// Split the range into the given number of jobs.
		// There will also be some additional jobs on lower levels of recursion,
		// but those won't hurt the performance. They will actually help by
		// merging the final results using a progressively smaller number of
		// threads.
		single_threaded_threshold = size / number_of_jobs;

		// Make sure that the threshold is not too small, because for smaller
		// branches it's better to use a single job.
		single_threaded_threshold =
		    std::max(single_threaded_threshold, min_single_threaded_threshold);
	}

	// Begin the first level of recursion.
	branch(begin, end);
}

template <iterator Iterator, comparator<Iterator> Comparator>
void quick_sort<Iterator, Comparator>::branch(Iterator begin, Iterator end) {
	std::size_t size = std::distance(begin, end);

	// Abort if the range is empty or contains only one element.
	if (size <= 1) {
		return;
	}

	// Use insertion sort for shorter ranges.
	if (size <= insertion_sort_threshold) {
		opal::insertion_sort(begin, end, comparator);
		return;
	}

	// Use heap sort for medium-length ranges.
	if (size <= heap_sort_threshold) {
		opal::heap_sort(begin, end, comparator);
		return;
	}

	// Partition the range into two parts.
	auto pivot = partition(begin, end);

	// Use single-threaded quick sort for shorter ranges.
	if (!pool || size <= single_threaded_threshold) {
		// Sort each of the two half-ranges.
		branch(begin, pivot.begin);
		branch(pivot.end, end);
	} else {
		// Offload the sorting of each of the two branches to a separate thread.
		auto lfuture = pool->submit([&](std::uint32_t) {
			branch(begin, pivot.begin);
		});
		auto rfuture = pool->submit([&](std::uint32_t) {
			branch(pivot.end, end);
		});

		// Wait for the threads to finish.
		lfuture->wait();
		rfuture->wait();
	}
}

// Lomuto partition scheme
// template <iterator Iterator, comparator<Iterator> Comparator>
// pivot<Iterator> quick_sort<Iterator, Comparator>::partition(
//     Iterator begin, Iterator end
// ) {
// 	// Initialize iterator that points to the tail of
// 	// a "virtual array" at the beginning of the range.
// 	Iterator it = begin;

// 	// Pivot the last element of the range.
// 	Iterator pivot = std::prev(end);

// 	// For each element except the pivot...
// 	for (Iterator jt = begin; jt != pivot; jt++) {
// 		// If the element comes before the pivot,
// 		// add it to the virtual array.
// 		if (comparator(*jt, *pivot)) {
// 			std::swap(*it, *jt);
// 			it++;
// 		}
// 	}

// 	// Finish the virtual array by adding the pivot at its end.
// 	std::swap(*it, *pivot);

// 	// Now the virtual array before the pivot contains all smaller values, and
// 	// the rest of the range after the pivot contains all values larger than or
// 	// equal to the pivot.
// 	return {it, std::next(it)};
// }

// Hoare partition scheme
template <iterator Iterator, comparator<Iterator> Comparator>
pivot<Iterator>
quick_sort<Iterator, Comparator>::partition(Iterator begin, Iterator end) {
	// Pivot the beginning of the range.
	auto pivot = *begin;

	// Initialize two iterators, both one position out of bounds.
	Iterator left  = std::prev(begin);
	Iterator right = end;

	while (true) {
		// Slide left iterator to the right until it finds an
		// element that does not satisfy the pivot condition.
		do {
			left++;
		} while (comparator(*left, pivot));

		// Slide right iterator to the left until it finds an
		// element that does not satisfy the pivot condition.
		do {
			right--;
		} while (comparator(pivot, *right));

		// If iterators have crossed, the partition is complete.
		if (right <= left) {
			return {std::next(right), std::next(right)};
		}

		// Swap the elements at the left and right iterators.
		std::swap(*left, *right);
	}
}

} // namespace opal::_internal

namespace opal {

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void quick_sort(Iterator begin, Iterator end, const Comparator &comparator) {
	_internal::quick_sort<Iterator, Comparator>{
	    nullptr, begin, end, comparator};
}

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void quick_sort(
    thread_pool      &pool,
    Iterator          begin,
    Iterator          end,
    const Comparator &comparator
) {
	_internal::quick_sort<Iterator, Comparator>{&pool, begin, end, comparator};
}

} // namespace opal
