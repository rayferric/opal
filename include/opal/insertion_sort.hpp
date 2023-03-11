#pragma once

#include "concepts.hpp"

namespace opal {

///
/// @brief Sorts the elements in the range [begin, end) using the insertion sort
/// algorithm.
///
/// @tparam Iterator the type of the iterator
/// @tparam Comparator the type of the comparator predicate
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param comparator the comparator predicate; It should return true only if
/// arguments are in the desired order but are not the same.
///
template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator = decltype(std::less{})>
void insertion_sort(
    Iterator begin, Iterator end, Comparator comparator = Comparator{}
);

} // namespace opal

#include "insertion_sort.inl"
