#pragma once

#include <iterator> // std::reverse_iterator
#include <memory>   // std::unique_ptr
#include <ostream>  // std::ostream

#include "bst_iterator.hpp"
#include "bst_node.hpp"

namespace opal {

template <typename Value>
class bst {
public:
	// types

	using value_type = Value;

	using size_type       = std::size_t;
	using difference_type = typename bst_iterator<value_type>::difference_type;

	using reference       = value_type &;
	using const_reference = const value_type &;

	using pointer       = value_type *;
	using const_pointer = const value_type *;

	using iterator       = bst_iterator<value_type>;
	using const_iterator = bst_iterator<value_type>; // TODO: const_iterator

	using reverse_iterator = typename std::reverse_iterator<iterator>;
	using const_reverse_iterator =
	    typename std::reverse_iterator<const_iterator>;

	// constructors

	bst();

	bst(const bst &other);
	bst(bst &&other) noexcept;

	// operators

	bst &operator=(const bst &rhs);
	bst &operator=(bst &&rhs) noexcept;

	template <typename T>
	friend std::ostream &operator<<(std::ostream &lhs, const bst<T> &rhs);

	// iterators

	iterator begin(bst_iterator_type type = iterator::default_type) noexcept;
	const_iterator
	begin(bst_iterator_type type = iterator::default_type) const noexcept;

	iterator end(bst_iterator_type type = iterator::default_type) noexcept;
	const_iterator
	end(bst_iterator_type type = iterator::default_type) const noexcept;

	reverse_iterator
	rbegin(bst_iterator_type type = iterator::default_type) noexcept;
	const_reverse_iterator
	rbegin(bst_iterator_type type = iterator::default_type) const noexcept;

	reverse_iterator
	rend(bst_iterator_type type = iterator::default_type) noexcept;
	const_reverse_iterator
	rend(bst_iterator_type type = iterator::default_type) const noexcept;

	// capacity

	bool empty() const noexcept;

	size_type size() const noexcept;

	// element access

	reference front(bst_iterator_type type = iterator::default_type);
	const_reference
	front(bst_iterator_type type = iterator::default_type) const;

	reference       back(bst_iterator_type type = iterator::default_type);
	const_reference back(bst_iterator_type type = iterator::default_type) const;

	iterator       find(const_reference value);
	const_iterator find(const_reference value) const;

	// modifiers

	void clear() noexcept;

	iterator insert(const_reference value);
	iterator insert(value_type &&value);

	template <typename... Args>
	iterator emplace(Args &&...args);

	iterator erase(iterator value);

private:
	std::unique_ptr<_internal::bst_node<value_type>> root;

	size_type count_nodes(_internal::bst_node<value_type> *node) const;

	std::unique_ptr<_internal::bst_node<value_type>> &
	find_ref(_internal::bst_node<value_type> *node);
};

} // namespace opal

#include "bst.inl"
