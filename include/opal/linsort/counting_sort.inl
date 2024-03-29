#include <algorithm> // std::min_element, std::max_element, std::move
#include <cstdlib>   // std::size_t
#include <iterator>  // std::distance
#include <vector>    // std::vector

#include <functional>

namespace opal {

template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
void counting_sort(
    Iterator begin, Iterator end, const Indexer &indexer, std::size_t max_index
) {
	// Compute max_index if not specified.
	if (max_index == static_cast<std::size_t>(-1)) {
		max_index = indexer(*begin);
		for (auto it = std::next(begin); it != end; it++) {
			max_index =
			    std::max(max_index, static_cast<std::size_t>(indexer(*it)));
		}
	}

	// Create a list of counters for each value.
	std::vector<std::size_t> counts(max_index + 1, 0);

	// Count the number of occurrences of each value.
	for (auto it = begin; it != end; it++) {
		std::size_t index = indexer(*it);
		counts[index]++;
	}

	// Compute the starting index of each value.
	// [0, 3, 2, 0, 1] -> [0, 3, 5, 5, 6]
	for (std::size_t i = 1; i < counts.size(); i++) {
		counts[i] += counts[i - 1];
	}
	// Now counts[i] is the number of elements less than or equal to i.

	// Place each element in its correct position.
	// Iterate backwards to preserve the relative order of equal elements.
	using value_type             = typename Iterator::value_type;
	std::size_t             size = std::distance(begin, end);
	std::vector<value_type> sorted(size);
	for (auto it = end - 1; it >= begin; it--) {
		std::size_t index = indexer(*it);

		// Decrement the count to make sure the next instance of this value will
		// be inserted next to this one.
		counts[index]--;

		// Put the element in its correct position.
		sorted[counts[index]] = std::move(*it);
	}

	// Move the sorted elements back into the original array.
	std::move(sorted.begin(), sorted.end(), begin);
}

template <bool Ascending, _internal::iterator Iterator>
void counting_sort(Iterator begin, Iterator end) {
	// Find the minimum and the maximum in the array.
	auto min = *std::min_element(begin, end);
	auto max = *std::max_element(begin, end);

	// Find the maximum index when mapping [min, max] to [0, max - min].
	using value_type      = typename Iterator::value_type;
	std::size_t max_index = max - min;

	// Sort the array either in ascending or descending order.
	if constexpr (Ascending) {
		// Map [min, max] to [0, max - min].
		auto indexer = [min](const value_type &value) {
			return value - min;
		};

		// Sort the array.
		counting_sort(begin, end, indexer, max_index);
	} else {
		// Map [min, max] to [max - min, 0].
		auto indexer = [max](const value_type &value) {
			return max - value;
		};

		// Sort the array.
		counting_sort(begin, end, indexer, max_index);
	}
}

} // namespace opal
