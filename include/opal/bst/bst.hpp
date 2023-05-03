#pragma once

#include <iterator> // std::reverse_iterator
#include <memory>   // std::unique_ptr
#include <ostream>  // std::ostream

#include "bst_iterator.hpp"
#include "bst_node.hpp"

namespace opal {

///
/// @brief binary search tree container
///
/// @tparam Value type of the values to be stored
///
template <typename Value>
class bst {
public:
	// types

	///
	/// @brief type of the values stored in the container
	///
	using value_type = Value;

	///
	/// @brief type used to represent sizes
	///
	using size_type = std::size_t;

	///
	/// @brief type used to represent distances between iterators
	///
	using difference_type = typename bst_iterator<value_type>::difference_type;

	///
	/// @brief reference type to the values stored in the container
	///
	using reference = value_type &;

	///
	/// @brief const reference type to the values stored in the container
	///
	using const_reference = const value_type &;

	///
	/// @brief pointer type to the values stored in the container
	///
	using pointer = value_type *;

	///
	/// @brief const pointer type to the values stored in the container
	///
	using const_pointer = const value_type *;

	///
	/// @brief type of the iterator for this container
	///
	using iterator = bst_iterator<value_type>;

	///
	/// @brief const type of the iterator for this container
	/// @todo Not yet implemented. Exactly the same as iterator.
	///
	using const_iterator = bst_iterator<value_type>;
	// TODO: const_iterator

	///
	/// @brief type of the reverse iterator for this container
	///
	using reverse_iterator = typename std::reverse_iterator<iterator>;

	///
	/// @brief const type of the reverse iterator for this container
	///
	using const_reverse_iterator =
	    typename std::reverse_iterator<const_iterator>;

	// constructors

	///
	/// @brief Constructs an empty BST.
	///
	bst();

	///
	/// @brief Constructs a copy of the given BST.
	///
	/// @param other BST to copy
	///
	bst(const bst &other);

	///
	/// @brief Constructs a BST by moving the given BST.
	///
	/// @param other BST to move
	///
	bst(bst &&other) noexcept;

	// operators

	///
	/// @brief Assigns a copy of the given BST to this BST.
	///
	/// @param rhs BST to copy
	///
	/// @return reference to this BST
	///
	bst &operator=(const bst &rhs);

	///
	/// @brief Assigns a moved BST to this BST.
	///
	/// @param rhs BST to move
	///
	/// @return reference to this BST
	///
	bst &operator=(bst &&rhs) noexcept;

	///
	/// @brief Stringifies the BST and writes it to the given output stream.
	///
	/// @tparam T type of the values stored in the BST
	///
	/// @param lhs output stream to write to
	/// @param rhs BST to stringify
	///
	/// @return reference to the output stream
	///
	template <typename T>
	friend std::ostream &operator<<(std::ostream &lhs, const bst<T> &rhs);

	// iterators

	///
	/// @brief Returns an iterator to the first element of the BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return iterator to the first element of the BST
	///
	iterator begin(bst_iterator_type type = iterator::default_type) noexcept;

	///
	/// @brief Returns a const iterator to the first element of the BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return const iterator to the first element of the BST
	///
	const_iterator
	begin(bst_iterator_type type = iterator::default_type) const noexcept;

	///
	/// @brief Returns an iterator to a virtual element following the last
	/// element of the BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return iterator to the element following the last element of the BST
	///
	iterator end(bst_iterator_type type = iterator::default_type) noexcept;

	///
	/// @brief Returns a const iterator to a virtual element following the last
	/// element of the BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return const iterator to the element following the last element of the
	/// BST
	///
	const_iterator
	end(bst_iterator_type type = iterator::default_type) const noexcept;

	///
	/// @brief Returns a reverse iterator to the first element of the reversed
	/// BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return reverse iterator to the first element of the reversed BST
	///
	reverse_iterator
	rbegin(bst_iterator_type type = iterator::default_type) noexcept;

	///
	/// @brief Returns a const reverse iterator to the first element of the
	/// reversed BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return const reverse iterator to the first element of the reversed BST
	///
	const_reverse_iterator
	rbegin(bst_iterator_type type = iterator::default_type) const noexcept;

	///
	/// @brief Returns a reverse iterator to a virtual element following the
	/// last element of the reversed BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return reverse iterator to the element following the last element of
	/// the reversed BST
	///
	reverse_iterator
	rend(bst_iterator_type type = iterator::default_type) noexcept;

	///
	/// @brief Returns a const reverse iterator to a virtual element following
	/// the last element of the reversed BST.
	///
	/// @param type type of the iterator to return
	///
	/// @return const reverse iterator to the element following the last element
	/// of the reversed BST
	///
	const_reverse_iterator
	rend(bst_iterator_type type = iterator::default_type) const noexcept;

	// capacity

	///
	/// @brief Checks whether the BST is empty.
	///
	/// @return true if the BST is empty, false otherwise
	///
	bool empty() const noexcept;

	///
	/// @brief Returns the number of elements in the BST.
	///
	/// @return number of elements in the BST
	///
	size_type size() const noexcept;

	// element access

	///
	/// @brief Returns a reference to the first element of the BST.
	///
	/// @param type type of the iterator whose first element to return
	///
	/// @return reference to the first element of the BST
	///
	reference front(bst_iterator_type type = iterator::default_type);

	///
	/// @brief Returns a const reference to the first element of the BST.
	///
	/// @param type type of the iterator whose first element to return
	///
	/// @return const reference to the first element of the BST
	///
	const_reference
	front(bst_iterator_type type = iterator::default_type) const;

	///
	/// @brief Returns a reference to the last element of the BST.
	///
	/// @param type type of the iterator whose last element to return
	///
	/// @return reference to the last element of the BST
	///
	reference back(bst_iterator_type type = iterator::default_type);

	///
	/// @brief Returns a const reference to the last element of the BST.
	///
	/// @param type type of the iterator whose last element to return
	///
	/// @return const reference to the last element of the BST
	///
	const_reference back(bst_iterator_type type = iterator::default_type) const;

	///
	/// @brief Finds an element with the given value.
	///
	/// @param value value to search for
	///
	/// @return iterator to the element with the given value if found, end()
	/// otherwise
	///
	iterator find(const_reference value);

	///
	/// @brief Finds an element with the given value.
	///
	/// @param value value to search for
	///
	/// @return const iterator to the element with the given value if found,
	/// end() otherwise
	///
	const_iterator find(const_reference value) const;

	// modifiers

	///
	/// @brief Removes all elements from the BST.
	///
	void clear() noexcept;

	///
	/// @brief Inserts an element into the BST.
	///
	/// @param value value to insert
	///
	/// @return iterator to the inserted element
	///
	iterator insert(const_reference value);

	///
	/// @brief Moves an element into the BST.
	///
	/// @param value value to insert
	///
	/// @return iterator to the inserted element
	///
	iterator insert(value_type &&value);

	///
	/// @brief Inserts an element into the BST. The element is constructed
	/// in-place from passed arguments.
	///
	/// @tparam Args types of the arguments
	///
	/// @param args arguments to forward to the constructor of the element
	///
	/// @return iterator to the inserted element
	///
	template <typename... Args>
	iterator emplace(Args &&...args);

	///
	/// @brief Removes an element from the BST.
	///
	/// @param value value to remove
	///
	/// @return iterator to the element following the removed element
	///
	iterator erase(iterator value);

private:
	std::unique_ptr<_internal::bst_node<value_type>> root;

	size_type count_nodes(_internal::bst_node<value_type> *node) const;

	std::unique_ptr<_internal::bst_node<value_type>> &
	find_ref(_internal::bst_node<value_type> *node);
};

} // namespace opal

#include "bst.inl"
