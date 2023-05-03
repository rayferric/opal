#pragma once

#include <cstddef>  // std::ptrdiff_t
#include <iterator> // std::random_access_iterator_tag

#include "bst_node.hpp"

namespace opal {

///
/// @brief type of the bst_iterator; Different traversal patterns are used
/// depending on the type of the iterator. Three types are available: in-order,
/// pre-order and post-order.
///
enum class bst_iterator_type {
	in_order,
	pre_order,
	post_order
	// TODO: level_order
};

template <typename Value>
class bst;

///
/// @brief BST tree iterator
///
/// @tparam Value type of values stored in the container
///
template <typename Value>
class bst_iterator {
public:
	///
	/// @brief type used to represent distances between iterators
	///
	using difference_type = std::ptrdiff_t;

	///
	/// @brief iterator category
	///
	using iterator_category = std::bidirectional_iterator_tag;

	///
	/// @brief pointer type to the values accessed by the iterator
	///
	using pointer = Value *;

	///
	/// @brief reference type to the values accessed by the iterator
	///
	using reference = Value &;

	///
	/// @brief type of the values stored in the container
	///
	using value_type = Value;

	///
	/// @brief default type of bst_iterators
	///
	static constexpr bst_iterator_type default_type =
	    bst_iterator_type::in_order;

	///
	/// @brief Constructs an invalid iterator detached from any container.
	///
	bst_iterator() noexcept;

	///
	/// @brief Constructs a copy of the given iterator.
	///
	/// @param other iterator to copy
	///
	bst_iterator(const bst_iterator &other) noexcept;

	///
	/// @brief Assigns a copy of the given iterator to this iterator.
	///
	/// @param other iterator to copy
	///
	/// @return reference to this iterator
	///
	bst_iterator &operator=(const bst_iterator &other) noexcept;

	///
	/// @brief Convert this iterator to a boolean.
	///
	/// @return true if iterator points to a valid node, false otherwise
	///
	operator bool() const noexcept;

	///
	/// @brief Advances the iterator to the next node in the tree.
	///
	/// @return reference to this iterator
	///
	bst_iterator &operator++() noexcept;

	///
	/// @brief Advances the iterator to the previous node in the tree.
	///
	/// @return reference to this iterator
	///
	bst_iterator &operator--() noexcept;

	///
	/// @brief Advances the iterator to the next node in the tree.
	///
	/// @return copy of this iterator before advancing
	///
	bst_iterator operator++(int);

	///
	/// @brief Advances the iterator to the previous node in the tree.
	///
	/// @return copy of this iterator before advancing
	///
	bst_iterator operator--(int);

	///
	/// @brief Advances the iterator by the given number of nodes.
	///
	/// @param n number of nodes to skip
	///
	bst_iterator &operator+=(difference_type n) noexcept;

	///
	/// @brief Advances the iterator back by the given number of nodes.
	///
	/// @param n number of nodes to skip
	///
	bst_iterator &operator-=(difference_type n) noexcept;

	///
	/// @brief Checks whether two iterators point to the same node.
	///
	/// @param other iterator to compare with
	///
	/// @return true if iterators point to the same node, false otherwise
	///
	bool operator==(const bst_iterator &other) const noexcept;

	///
	/// @brief Checks whether two iterators point to different nodes.
	///
	/// @param other iterator to compare with
	///
	/// @return true if iterators point to different nodes, false otherwise
	///
	bool operator!=(const bst_iterator &other) const noexcept;

	///
	/// @brief Accesses the value pointed to by the iterator.
	///
	/// @return reference to the value pointed to by the iterator
	///
	reference operator*() const;

	///
	/// @brief Accesses the value pointed to by the iterator.
	///
	/// @return pointer to the value pointed to by the iterator
	///
	pointer operator->() const;

	///
	/// @brief Tells the type of the iterator.
	///
	/// @return type of the iterator: in-order, pre-order or post-order
	///
	bst_iterator_type type() const noexcept;

	///
	/// @brief Sets the type of the iterator.
	///
	/// @param type type of the iterator: in-order, pre-order or post-order
	///
	void type(bst_iterator_type type) noexcept;

	///
	/// @brief Returns the iterator to the parent node.
	///
	/// @return iterator to the parent node if it exists, end iterator otherwise
	///
	bst_iterator parent() const noexcept;

	///
	/// @brief Returns the iterator to the left child node.
	///
	/// @return iterator to the left child node if it exists, end iterator
	/// otherwise
	///
	bst_iterator left() const noexcept;

	///
	/// @brief Returns the iterator to the right child node.
	///
	/// @return iterator to the right child node if it exists, end iterator
	/// otherwise
	///
	bst_iterator right() const noexcept;

private:
	friend bst<value_type>;

	_internal::bst_node<value_type> *root;
	_internal::bst_node<value_type> *node;
	bst_iterator_type                _type;

	bst_iterator(
	    _internal::bst_node<value_type> *root,
	    _internal::bst_node<value_type> *node,
	    bst_iterator_type                type = default_type
	) noexcept;
};

} // namespace opal

#include "bst_iterator.inl"
