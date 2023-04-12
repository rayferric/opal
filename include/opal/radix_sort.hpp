#pragma once

#include <concepts> // std::integral

#include "concepts.hpp"

namespace opal {

///
/// @brief Sorts elements in range [begin, end) using the radix sort
/// algorithm. Value bounds of the elements are computed automatically.
///
/// @tparam Iterator the type of iterator
/// @tparam Indexer the type of indexer function
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param indexer the indexer to use; Indexer converts elements of the range to
/// unsigned integers.
/// @param max_index maximum possible index returned by indexer; Automatically
/// computed if not specified.
/// @param radix the radix to use; Defaults to 256.
///
template <_internal::iterator Iterator, _internal::indexer<Iterator> Indexer>
void radix_sort(
    Iterator       begin,
    Iterator       end,
    const Indexer &indexer,
    std::size_t    max_index = -1,
    std::uint32_t  radix     = 256
);

///
/// @brief Sorts integers in range [begin, end) using the radix sort
/// algorithm. Indexer function is generated automatically.
///
/// @tparam Ascending whether to sort in ascending order; Defaults to true
/// @tparam Iterator the type of iterator, must be an integer iterator
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param radix the radix to use; Defaults to 256.
///
template <bool Ascending = true, _internal::iterator Iterator>
    requires std::integral<typename Iterator::value_type>
void radix_sort(Iterator begin, Iterator end, std::uint32_t radix = 256);

} // namespace opal

#include "radix_sort.inl"
