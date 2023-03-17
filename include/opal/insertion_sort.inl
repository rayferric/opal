#include <iterator> // std::next, std::prev
#include <utility>  // std::move

namespace opal {

template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator>
void insertion_sort(Iterator begin, Iterator end, Comparator comparator) {
	// Focus each element starting from the second one up to the last one.
	for (Iterator focus = std::next(begin); focus != end; focus++) {
		// Remember the focused element before shifting.
		typename Iterator::value_type value = std::move(*focus);

		// Shift previous elements one position to the right
		// until the right spot for the focused value is found.
		Iterator prev = std::prev(focus);
		while (prev != std::prev(begin) && comparator(value, *prev)) {
			// Move prev by replacing the element to the right of it.
			// Focused element will be the first one to be overwritten
			// and that is why it was saved before shifting.
			*std::next(prev) = std::move(*prev);
			prev--;
		}

		// Insert the focused value in the old place of the last shifted
		// element. If no elements were shifted, the focused value will be
		// inserted in the same place where it has been.
		*std::next(prev) = std::move(value);
	}
}

} // namespace opal
