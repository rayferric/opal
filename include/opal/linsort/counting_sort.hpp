#pragma once

#include <concepts> // std::integral

#include "../concepts.hpp"

namespace opal {

///
/// @brief Sorts elements in range [begin, end) using the counting sort
/// algorithm. Uses custom index generator.
///
/// @tparam Iterator the type of iterator
/// @tparam Indexer the type of indexer function
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param indexer indexer function; It should return the index of the element
/// in the range [0, max_index].
/// @param max_index the maximum index possible to be returned by indexer;
/// Computed automatically if not specified.
///
template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
void counting_sort(
    Iterator       begin,
    Iterator       end,
    const Indexer &indexer,
    std::size_t    max_index = -1
);

///
/// @brief Sorts integers in range [begin, end) using the counting sort
/// algorithm. Indexer function is generated automatically.
///
/// @tparam Ascending whether to sort in ascending order; Defaults to true
/// @tparam Iterator the type of iterator
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
///
template <bool Ascending = true, _internal::iterator Iterator>
// requires std::integral<typename Iterator::value_type>
void counting_sort(Iterator begin, Iterator end);

} // namespace opal

#include "counting_sort.inl"
