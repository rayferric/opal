#pragma once

#include <cstddef>  // std::ptrdiff_t
#include <iterator> // std::random_access_iterator_tag

#include "bst_node.hpp"

namespace opal {

enum class bst_iterator_type {
	in_order,
	pre_order,
	post_order
	// level_order // breadth-first
};

template <typename Value>
class bst;

template <typename Value>
class bst_iterator {
public:
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	using pointer           = Value *;
	using reference         = Value &;
	using value_type        = Value;

	static constexpr bst_iterator_type default_type =
	    bst_iterator_type::in_order;

	bst_iterator() noexcept;
	bst_iterator(const bst_iterator &other) noexcept;

	bst_iterator &operator=(const bst_iterator &other) noexcept;

	operator bool() const noexcept;

	bst_iterator &operator++() noexcept;
	bst_iterator &operator--() noexcept;

	bst_iterator operator++(int);
	bst_iterator operator--(int);

	bst_iterator &operator+=(difference_type n) noexcept;
	bst_iterator &operator-=(difference_type n) noexcept;

	bool operator==(const bst_iterator &other) const noexcept;
	bool operator!=(const bst_iterator &other) const noexcept;

	reference operator*() const;
	pointer   operator->() const;

	bst_iterator_type type() const noexcept;
	void              type(bst_iterator_type type) noexcept;

	bst_iterator parent() const noexcept;
	bst_iterator left() const noexcept;
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
