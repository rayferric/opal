#include <algorithm> // std::swap
#include <cstdlib>   // std::size_t
#include <iterator>  // std::distance

namespace opal::_internal {

// the heap sort algorithm
template <iterator Iterator, comparator<Iterator> Comparator>
class heap_sort {
public:
	// Constructs the algorithm context and begins the sorting process.
	heap_sort(Iterator begin, Iterator end, const Comparator &comparator);

private:
	// the range to sort
	Iterator begin, end;

	// comparator predicate
	const Comparator &comparator;

	// Fixes up a max-heap with bad root. (O(log n))
	void heapify(Iterator root);
};

template <iterator Iterator, comparator<Iterator> Comparator>
heap_sort<Iterator, Comparator>::heap_sort(
    Iterator begin, Iterator end, const Comparator &comparator
)
    : begin(begin), end(end), comparator(comparator) {
	// Build the max-heap.
	// begin + (size / 2) - 1 is the last non-leaf node.
	std::size_t size = std::distance(begin, end);
	for (auto root = begin + (size / 2) - 1; root >= begin; root--) {
		heapify(root);
	}

	while (this->end != begin) {
		// Reduce the size of the heap by one.
		this->end--;

		// Move the largest element (root) outside of the heap.
		std::swap(*begin, *(this->end));

		// Fix up the heap.
		heapify(begin);
	}

	// The heap is now sorted.
}

template <iterator Iterator, comparator<Iterator> Comparator>
void heap_sort<Iterator, Comparator>::heapify(Iterator root) {
	std::size_t root_index = std::distance(begin, root);

	// Find both children of the root.
	Iterator left  = begin + (root_index * 2 + 1);
	Iterator right = begin + (root_index * 2 + 2);

	// Find the largest element of all three.
	Iterator largest = root;
	if (left < end && comparator(*largest, *left)) {
		largest = left;
	}
	if (right < end && comparator(*largest, *right)) {
		largest = right;
	}

	// If the root is not the largest, swap it with the largest and fix up the
	// new heap.
	if (largest != root) {
		std::swap(*root, *largest);
		// root now points to the larger value and largest points to the smaller
		// value (the child). Heapify the sub-heap starting from the child.
		heapify(largest);
	}
}

} // namespace opal::_internal

namespace opal {

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void heap_sort(Iterator begin, Iterator end, const Comparator &comparator) {
	_internal::heap_sort<Iterator, Comparator>{begin, end, comparator};
}

} // namespace opal
