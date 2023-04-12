#pragma once

#include <functional> // std::less

#include "concepts.hpp"

namespace opal {

///
/// @brief Sorts the elements in the range [begin, end) using the insertion sort
/// algorithm.
///
/// @tparam Iterator the type of iterator
/// @tparam Comparator the type of comparator predicate
///
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param comparator optional comparator predicate; It should return true only
/// if arguments are in the desired order but are not the same.
///
template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator = decltype(std::less{})>
void insertion_sort(
    Iterator begin, Iterator end, const Comparator &comparator = Comparator{}
);

} // namespace opal

#include "insertion_sort.inl"
