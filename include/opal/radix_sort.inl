namespace opal::_internal {

// custom counting sort implementation, optimized for radix sort
template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
static void radix_sort_counting_sort(
    Iterator begin, Iterator end, const Indexer &indexer, std::vector<std::size_t> &counts
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
	using value_type             = typename Iterator::value_type;
	std::size_t             size = std::distance(begin, end);
	std::vector<value_type> sorted(size);
	for (auto it = end - 1; it >= begin; it--) {
		std::size_t index = indexer(*it);

		// Decrement the count to make sure the next instance of this value will
		// be inserted next to this one.
		counts[index]--;
		sorted[counts[index]] = std::move(*it);
	}

	// Move the sorted elements back into the original array.
	std::move(sorted.begin(), sorted.end(), begin);
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
		max_index = indexer(*begin);
		for (auto it = std::next(begin); it != end; it++) {
			max_index =
			    std::max(max_index, static_cast<std::size_t>(indexer(*it)));
		}
	}

	// common buffer for all counting_sort calls
	std::vector<std::size_t> counts(radix, 0);

	// For each digit, sort the elements using counting sort.
	// TODO: Takes too long. Must cache counting_sort buffers and reuse as much
	// data as possible between calls.
	for (std::size_t i = 1; i <= max_index; i *= radix) {
		// Initialize counts to 0.
		if (i != 1) {
			std::fill(counts.begin(), counts.end(), 0);
		}

		_internal::radix_sort_counting_sort(
		    begin,
		    end,
		    [&indexer, i, radix](auto x) {
			    return (indexer(x) / i) % radix;
		    },
		    counts
		);
	}
}

template <bool Ascending, _internal::iterator Iterator>
    requires std::integral<typename Iterator::value_type>
void radix_sort(Iterator begin, Iterator end, std::uint32_t radix) {
	auto min = *std::min_element(begin, end);
	auto max = *std::max_element(begin, end);

	using value_type      = typename Iterator::value_type;
	std::size_t max_index = max - min;

	if constexpr (Ascending) {
		auto indexer = [min](const value_type &value) {
			return value - min;
		};
		radix_sort(begin, end, indexer, max_index, radix);
	} else {
		auto indexer = [max](const value_type &value) {
			return max - value;
		};
		radix_sort(begin, end, indexer, max_index, radix);
	}
}

} // namespace opal
