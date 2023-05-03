namespace opal::_internal {

// custom counting sort implementation, optimized for radix sort
template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
static void radix_sort_counting_sort(
    Iterator                  begin,
    Iterator                  end,
    const Indexer            &indexer,
    std::vector<std::size_t> &counts,
    Iterator                  sorted_begin
) {
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
	for (auto it = end - 1; it >= begin; it--) {
		std::size_t index = indexer(*it);

		// Decrement the count to make sure the next instance of this value will
		// be inserted next to this one.
		counts[index]--;
		*(sorted_begin + counts[index]) = std::move(*it);
	}
}

} // namespace opal::_internal

namespace opal {

template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
void radix_sort(
    Iterator       begin,
    Iterator       end,
    const Indexer &indexer,
    std::size_t    max_index,
    std::uint32_t  radix
) {
	// Compute max_index if not specified.
	if (max_index == static_cast<std::size_t>(-1)) {
		// Find the maximum index from all of the elements.
		max_index = indexer(*begin);
		for (auto it = std::next(begin); it != end; it++) {
			max_index =
			    std::max(max_index, static_cast<std::size_t>(indexer(*it)));
		}
	}

	// common buffers for all counting_sort calls
	using value_type = typename Iterator::value_type;
	std::vector<std::size_t> counts(radix, 0);
	std::vector<value_type>  sorted(std::distance(begin, end));

	// Keep track of the output iterators for counting sort to use
	// double-buffering.
	auto sorted_begin = sorted.begin();
	auto sorted_end   = sorted.end();

	// For each digit, sort the elements using counting sort.
	for (std::size_t i = 1; i <= max_index; i *= radix) {
		// Initialize counts to 0.
		if (i != 1) {
			std::fill(counts.begin(), counts.end(), 0);
		}

		// Sort by the current digit.
		_internal::radix_sort_counting_sort(
		    begin,
		    end,
		    [&indexer, i, radix](auto x) {
			    return (indexer(x) / i) % radix;
		    },
		    counts,
		    sorted_begin
		);

		// Double-buffering: swap the original array iterators with the sorted
		// array iterators. This way, the next iteration will sort the sorted
		// array and store the result in the original array and vice versa.
		std::swap(begin, sorted_begin);
		std::swap(end, sorted_end);
	}

	// If the final array ended up in the sorted buffer, move it back to the
	// original array.
	if (begin == sorted.begin()) {
		std::move(sorted.begin(), sorted.end(), sorted_begin);
	}
}

template <bool Ascending, _internal::iterator Iterator>
    requires std::integral<typename Iterator::value_type>
void radix_sort(Iterator begin, Iterator end, std::uint32_t radix) {
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
		radix_sort(begin, end, indexer, max_index, radix);
	} else {
		// Map [min, max] to [max - min, 0].
		auto indexer = [max](const value_type &value) {
			return max - value;
		};

		// Sort the array.
		radix_sort(begin, end, indexer, max_index, radix);
	}
}

} // namespace opal
