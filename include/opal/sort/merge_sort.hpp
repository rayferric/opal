#pragma once

#include <functional> // std::less

#include "../concepts.hpp"
#include "../thread_pool.hpp"

namespace opal {

///
/// @brief Sorts the elements in the range [begin, end) using the merge sort
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
void merge_sort(
    Iterator begin, Iterator end, const Comparator &comparator = Comparator{}
);

///
/// @brief Sorts the elements in the range [begin, end) using
/// the multithreaded version of quick sort algorithm.
///
/// @tparam Iterator the type of iterator
/// @tparam Comparator the type of comparator predicate
///
/// @param pool the thread pool to use
/// @param begin the beginning of the range to sort
/// @param end the end of the range
/// @param comparator optional comparator predicate; It should return true only
/// if arguments are in the desired order but are not the same.
///
template <
    _internal::iterator             Iterator,
    _internal::comparator<Iterator> Comparator = decltype(std::less{})>
void merge_sort(
    thread_pool      &pool,
    Iterator          begin,
    Iterator          end,
    const Comparator &comparator = Comparator{}
);

} // namespace opal

#include "merge_sort.inl"
