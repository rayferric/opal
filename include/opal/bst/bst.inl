#include <cmath>

namespace opal::_internal {


template <typename Value>
std::vector<std::string> stringify_bst(_internal::bst_node<Value> *node) {
	// If there's no value, return empty.
	if (!node) {
		return {std::string{}};
	}

	// If there's no children, stringify just the value.
	if (!node->left && !node->right) {
		return {std::to_string(node->value)};
	}

	// // If there's one child, create a vertical connection.
	// if (!node->left || !node->right) {
	//     std::string value = std::to_string(node->value);
	//     std::vector<std::string> child = stringify(node->left ?
	//     node->left.get() : node->right.get());

	//     // Pad child to the same length as the value.
	//     if (child.front().size() < value.size()) {
	//         std::int32_t length_delta = value.size() - child.front().size();
	//         std::string left_padding = std::string(length_delta / 2, ' ');
	//         std::string right_padding = std::string((length_delta + 1) / 2, '
	//         ');

	//         for (auto &line : child) {
	//             line = left_padding + line + right_padding;
	//         }
	//     }

	//     // Find the connection index.
	//     std::size_t child_value_start_i = child.front().find_first_not_of("
	//     _"); std::size_t child_value_end_i = child.front().find_last_not_of("
	//     _"); std::size_t child_value_center_i = (child_value_start_i +
	//     child_value_end_i) / 2;

	//     // Create connection padding.
	//     auto left_connection_padding = std::string(child_value_center_i, '
	//     '); auto right_connection_padding = std::string(child.front().size()
	//     - child_value_center_i - 1, ' ');

	//     // Create the connection string.
	//     std::string connection = left_connection_padding + '|' +
	//     right_connection_padding;

	//     // Add the connection.
	//     child.insert(child.begin(), std::move(connection));

	//     // Pad value so that its center is above the connection.
	//     std::size_t left_value_padding_size =
	//     std::max(static_cast<std::int32_t>(child_value_center_i) -
	//     static_cast<std::int32_t>(value.size() - 1) / 2, 0);
	//     left_value_padding_size = std::min(left_value_padding_size,
	//     child.front().size() - value.size()); auto left_value_padding =
	//     std::string(left_value_padding_size, ' '); auto right_value_padding =
	//     std::string(child.front().size() - value.size() -
	//     left_value_padding_size, ' '); value = left_value_padding + value +
	//     right_value_padding;

	//     // Add the value.
	//     child.insert(child.begin(), std::move(value));

	//     // debug check
	//     std::size_t len = child.front().size();
	//     for (auto &line : child) {
	//         if (line.size() != len) {
	//             throw std::runtime_error("Line lengths mismatch.");
	//         }
	//     }

	//     return child;
	// }

	// If there's two children, create two diagonal connections.

	// Stringify the value and the children.
	std::string              value = std::to_string(node->value);
	std::vector<std::string> left  = node->left ? stringify_bst(node->left.get())
	                                            : std::vector{std::string{"~"}};
	std::vector<std::string> right = node->right
	                                   ? stringify_bst(node->right.get())
	                                   : std::vector{std::string{"~"}};

	// Join the children horizontally, separated by a space:
	// > 1000 1000

	// Find the maximum height of the children.
	std::size_t height = std::max(left.size(), right.size());
	// Initialize all lines to empty strings.
	std::vector<std::string> children(height, std::string{});
	// Join the children.
	std::string left_placeholder  = std::string(left.front().size(), ' ');
	std::string right_placeholder = std::string(right.front().size(), ' ');
	for (std::size_t i = 0; i < height; i++) {
		if (i < left.size() && i < right.size()) {
			children.at(i) = left.at(i) + ' ' + right.at(i);
		} else if (i < left.size()) {
			children.at(i) = left.at(i) + ' ' + right_placeholder;
		} else {
			children.at(i) = left_placeholder + ' ' + right.at(i);
		}
	}

	// Create new line for diagonal connections:
	// >  /   \
    //  1000 1000

	// Find the left connection index.
	std::size_t left_value_start_i = left.front().find_first_not_of(" _");
	std::size_t left_value_end_i   = left.front().find_last_not_of(" _");
	std::size_t left_value_center_i =
	    (left_value_start_i + left_value_end_i + 1) / 2;

	// Find the right connection index.
	std::size_t right_value_start_i = right.front().find_first_not_of(" _");
	std::size_t right_value_end_i   = right.front().find_last_not_of(" _");
	std::size_t right_value_center_i =
	    (right_value_start_i + right_value_end_i) / 2;

	// Create connection padding.
	auto left_connection_left_padding = std::string(left_value_center_i, ' ');
	auto right_connection_right_padding =
	    std::string(right.front().size() - right_value_center_i - 1, ' ');

	auto mid_connection_padding = std::string(
	    children.front().size() - left_connection_left_padding.size() -
	        right_connection_right_padding.size() - 2,
	    ' '
	);

	std::string connection = left_connection_left_padding + '/' +
	                         mid_connection_padding + '\\' +
	                         right_connection_right_padding;
	children.insert(children.begin(), std::move(connection));

	// Pad children (and connection) to the same length as value while keeping
	// the other one centered.
	if (children.front().size() < value.size()) {
		std::size_t length_delta  = children.front().size() - value.size();
		std::string left_padding  = std::string(length_delta / 2, ' ');
		std::string right_padding = std::string((length_delta + 1) / 2, ' ');

		for (auto &line : children) {
			line = left_padding + line + right_padding;
		}
	}

	// Finally, append the value line:
	// >  _0_
	//   /   \
    // 1000 1000

	// Find the number of underscores needed at the sides of value.
	std::int32_t underscore_count = std::max(
	    static_cast<int32_t>(mid_connection_padding.size()) -
	        static_cast<int32_t>(value.size()),
	    0
	);
	auto left_underscores  = std::string(underscore_count / 2, '_');
	auto right_underscores = std::string((underscore_count + 1) / 2, '_');

	// Add the underscores to value.
	value = left_underscores + value + right_underscores;

	// Center the value line between the diagonal connections.
	std::size_t left_slash_i  = children.front().find('/');
	std::size_t right_slash_i = children.front().find('\\');
	std::size_t center_i      = (left_slash_i + right_slash_i) / 2;
	auto left_padding  = std::string(center_i - (value.size() - 1) / 2, ' ');
	auto right_padding = std::string(
	    children.front().size() - value.size() - left_padding.size(), ' '
	);
	value = left_padding + value + right_padding;

	// Add the value line.`
	children.insert(children.begin(), std::move(value));

	// // debug check
	// std::size_t len = children.front().size();
	// for (auto &line : children) {
	//     if (line.size() != len) {
	//         throw std::runtime_error("Line lengths mismatch.");
	//     }
	// }

	return children;
}

} // namespace opal::_internal

namespace opal {

// constructors

template <typename Value>
bst<Value>::bst() : root(nullptr) {}

template <typename Value>
bst<Value>::bst(const bst &other)
    : root(std::make_unique<_internal::bst_node<value_type>>(*other.root)) {}

template <typename Value>
bst<Value>::bst(bst &&other) noexcept : root(std::move(other.root)) {}

// operators

template <typename Value>
bst<Value> &bst<Value>::operator=(const bst &rhs) {
	root = std::make_unique<_internal::bst_node<value_type>>(*rhs.root);
	return *this;
}

template <typename Value>
bst<Value> &bst<Value>::operator=(bst &&rhs) noexcept {
	root = std::move(rhs.root);
	return *this;
}

template <typename Value>
std::ostream &operator<<(std::ostream &lhs, const bst<Value> &rhs) {
	std::vector<std::string> lines = _internal::stringify_bst<Value>(rhs.root.get());
	for (std::string &line : lines) {
		lhs << line << std::endl;
	}
	return lhs;
}

// iterators

template <typename Value>
typename bst<Value>::iterator bst<Value>::begin(bst_iterator_type type
) noexcept {
	return ++end(type);
}

template <typename Value>
typename bst<Value>::const_iterator bst<Value>::begin(bst_iterator_type type
) const noexcept {
	return begin(type);
}

template <typename Value>
typename bst<Value>::iterator bst<Value>::end(bst_iterator_type type) noexcept {
	return iterator{root.get(), nullptr, type};
}

template <typename Value>
typename bst<Value>::const_iterator bst<Value>::end(bst_iterator_type type
) const noexcept {
	return end(type);
}

template <typename Value>
typename bst<Value>::reverse_iterator bst<Value>::rbegin(bst_iterator_type type
) noexcept {
	return std::make_reverse_iterator(end(type));
}

template <typename Value>
typename bst<Value>::const_reverse_iterator
bst<Value>::rbegin(bst_iterator_type type) const noexcept {
	return rbegin(type);
}

template <typename Value>
typename bst<Value>::reverse_iterator bst<Value>::rend(bst_iterator_type type
) noexcept {
	return std::make_reverse_iterator(begin(type));
}

template <typename Value>
typename bst<Value>::const_reverse_iterator
bst<Value>::rend(bst_iterator_type type) const noexcept {
	return rend(type);
}

// capacity

template <typename Value>
bool bst<Value>::empty() const noexcept {
	return root == nullptr;
}

template <typename Value>
typename bst<Value>::size_type bst<Value>::size() const noexcept {
	// return std::distance(begin(), end());
	return count_nodes(root.get());
}

// element access

template <typename Value>
typename bst<Value>::reference bst<Value>::front(bst_iterator_type type) {
	return *begin(type);
}

template <typename Value>
typename bst<Value>::const_reference bst<Value>::front(bst_iterator_type type
) const {
	return front(type);
}

template <typename Value>
typename bst<Value>::reference bst<Value>::back(bst_iterator_type type) {
	return *(--end(type));
}

template <typename Value>
typename bst<Value>::const_reference bst<Value>::back(bst_iterator_type type
) const {
	return back(type);
}

template <typename Value>
typename bst<Value>::iterator bst<Value>::find(const_reference value) {
	// Start at the root.
	_internal::bst_node<value_type> *node = root.get();

	// While we have nodes to search.
	while (node != nullptr) {
		// If the value is less than the current node's value...
		if (value < node->value) {
			// Descend to the left.
			node = node->left.get();
		} else if (node->value < value) {
			// Otherwise, descend to the right.
			node = node->right.get();
		} else {
			// If the value has been found, return an iterator to it.
			return iterator{root.get(), node};
		}
	}

	// If the value has not been found, return end iterator.
	return end();
}

template <typename Value>
typename bst<Value>::const_iterator bst<Value>::find(const_reference value
) const {
	return find(value);
}

// modifiers

template <typename Value>
void bst<Value>::clear() noexcept {
	root.reset();
}

template <typename Value>
typename bst<Value>::iterator bst<Value>::insert(const_reference value) {
	// In-place constructed value is automatically cast to xvalue.
	return insert(value_type{value});
}

template <typename Value>
typename bst<Value>::iterator bst<Value>::insert(value_type &&value) {
	// If the tree is empty, create a new root node.
	if (root == nullptr) {
		root = std::make_unique<_internal::bst_node<Value>>(
		    std::move(value), nullptr
		);
		return iterator{root.get(), root.get()};
	}

	// Start at the root.
	_internal::bst_node<Value> *node = root.get();

	// While we have nodes to search.
	while (true) {
		// If the value is less than the current node's value...
		if (value < node->value) {
			// If the left child is null, create a new node.
			if (!node->left) {
				node->left = std::make_unique<_internal::bst_node<Value>>(
				    std::move(value), node
				);
				return iterator{root.get(), node->left.get()};
			}

			// Otherwise, descend to the left.
			// (left is not nullptr)
			node = node->left.get();
		} else if (node->value < value) {
			// If the right child is null, create a new node.
			if (!node->right) {
				node->right = std::make_unique<_internal::bst_node<Value>>(
				    std::move(value), node
				);
				return iterator{root.get(), node->right.get()};
			}

			// Otherwise, descend to the right.
			// (right is not nullptr)
			node = node->right.get();
		} else {
			// If the value has been found, return an iterator to it.
			// No new node is created.
			return iterator{root.get(), node};
		}
	}
}

template <typename Value>
template <typename... Args>
typename bst<Value>::iterator bst<Value>::emplace(Args &&...args) {
	// In-place constructed value is automatically cast to xvalue.
	return insert(value_type{std::forward<Args>(args)...});
}

template <typename Value>
typename bst<Value>::iterator bst<Value>::erase(iterator pos) {
	// If the iterator is invalid, return end iterator.
	if (!pos) {
		return end();
	}

	// Get next iterator.
	auto next = pos;
	++next;

	// Convert iterator to node pointer.
	_internal::bst_node<Value> *node = pos.node;

	// If the node has no children, just delete it.
	if (!node->left && !node->right) {
		find_ref(pos.node) = nullptr;
	}

	// Else if the node has only one child, replace it with that child.
	else if (!node->left) {
		// If there's no left child, replace the node with the right child.
		node->right->parent = node->parent;
		find_ref(node)      = std::move(node->right);
	} else if (!node->right) {
		// If there's no right child, replace the node with the left child.
		node->left->parent = node->parent;
		find_ref(node)     = std::move(node->left);
	}

	// Finally, if there's two children, replace this node with the next
	// in-order node.
	else {
		// Find the next in-order node.
		iterator next_in_order{root.get(), node};
		next_in_order.type(bst_iterator_type::in_order);
		++next_in_order;

		// Move the next in-order node's value to this node.
		node->value = std::move(next_in_order.node->value);

		// Delete the next in-order node.
		erase(next_in_order);
	}

	// Return the iterator to the next element.
	return next;
}

template <typename Value>
typename bst<Value>::size_type
bst<Value>::count_nodes(_internal::bst_node<Value> *node) const {
	if (node == nullptr) {
		return 0;
	}

	return 1 + count_nodes(node->left.get()) + count_nodes(node->right.get());
}

template <typename Value>
std::unique_ptr<_internal::bst_node<Value>> &
bst<Value>::find_ref(_internal::bst_node<Value> *node) {
	// If the node is the root, return the root.
	if (node->parent == nullptr) {
		return root;
	}

	// If the node is a left child, return the left child.
	if (node->parent->left.get() == node) {
		return node->parent->left;
	}

	// If the node is a right child, return the right child.
	if (node->parent->right.get() == node) {
		return node->parent->right;
	}

	// Otherwise, it makes no sense.
	throw std::logic_error{"Node is not a child of its parent."};
}

} // namespace opal