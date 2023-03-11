#pragma once

#include <concepts>

namespace opal::_internal {

///
/// @brief This concept requires an iterator type, that can be stepped through
/// and dereferenced.
///
// Clang-Format does not seem to like the requires syntax and ends up aligning
// the whole block several spaces to the right.
// clang-format off
template <typename T>
concept iterator = requires(T t) {
	*t;
	++t;
	t++;
	t == t;
	t != t;
	typename T::value_type;
};
// clang-format on

///
/// @brief This concept requires a comparator predicate function, that takes in
/// two values from Iterator and returns a boolean.
///
// clang-format off
template <typename T, typename Iterator>
concept comparator = requires(T t, Iterator a, Iterator b) {
	{ t(*a, *b) } -> std::convertible_to<bool>;
};
// clang-format on

} // namespace opal::_internal
