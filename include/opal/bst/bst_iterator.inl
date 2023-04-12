namespace opal {

template <typename Value>
bst_iterator<Value>::bst_iterator() noexcept
    : root(nullptr), node(nullptr), _type(bst_iterator_type::in_order) {}

template <typename Value>
bst_iterator<Value>::bst_iterator(const bst_iterator &other) noexcept
    : root(other.root), node(other.node), _type(other._type) {}

template <typename Value>
bst_iterator<Value> &bst_iterator<Value>::operator=(const bst_iterator &other
) noexcept {
	root  = other.root;
	node  = other.node;
	_type = other._type;
	return *this;
}

template <typename Value>
bst_iterator<Value>::operator bool() const noexcept {
	return node != nullptr;
}

template <typename Value>
bst_iterator<Value> &bst_iterator<Value>::operator++() noexcept {
	switch (_type) {
	// in-order traversal
	case bst_iterator_type::in_order:
		// If iterator is at the end, return to the beginning.
		if (!node) {
			node = root;

			// Go to the left as far as possible.
			while (node->left) {
				node = node->left.get();
			}

			return *this;
		}

		// Otherwise we are somewhere in the middle of th tree.

		// Try to go to the right.
		if (node->right) {
			node = node->right.get();

			// Go to the left as far as possible.
			while (node->left) {
				node = node->left.get();
			}

			return *this;
		}

		// Go up until we can go to the right.
		while (node->parent) {
			if (node->parent->left.get() == node) {
				node = node->parent;
				return *this;
			}

			node = node->parent;
		}

		// We are at the root and we're coming back from the right.
		node = nullptr;
		return *this;
	// pre-order traversal
	case bst_iterator_type::pre_order:
		// If iterator is at the end, return to the beginning.
		if (!node) {
			node = root;
			return *this;
		}

		// Otherwise we are somewhere in the middle of th tree.

		// Try to go to the left.
		if (node->left) {
			node = node->left.get();
			return *this;
		}

		// Try to go to the right.
		if (node->right) {
			node = node->right.get();
			return *this;
		}

		// Go up until we can go to the right.
		// We can only go to the right if we came from the left.
		while (node->parent) {
			if (node->parent->left.get() == node && node->parent->right) {
				node = node->parent->right.get();
				return *this;
			}

			// Otherwise we came from the right or there is no right child to
			// visit. Continue going up.
			node = node->parent;
		}

		// We are at the root and we're coming back from the right.
		node = nullptr;
		return *this;
	// post-order traversal
	case bst_iterator_type::post_order:
		// If iterator is at the end, return to the beginning.
		if (!node) {
			node = root;

			// Go down as far as possible.
			// Prioritize going down to the left.
			while (node->left || node->right) {
				if (node->left) {
					node = node->left.get();
				} else {
					node = node->right.get();
				}
			}

			return *this;
		}

		// Otherwise we are somewhere in the middle of th tree.

		// Go up one level.
		// If we came from the left, go to the right.
		// Then go down as far as possible.
		// Prioritize going down to the left.
		// If we came from the right, we are at the right place.
		if (node->parent) {
			if (node->parent->left.get() == node && node->parent->right) {
				node = node->parent->right.get();

				// Go down as far as possible.
				// Prioritize going down to the left.
				while (node->left || node->right) {
					if (node->left) {
						node = node->left.get();
					} else {
						node = node->right.get();
					}
				}

				return *this;
			}

			// Otherwise we came from the right or there is no right child to
			// visit. Stay here.
			node = node->parent;
			return *this;
		}

		// We are at the root.
		node = nullptr;
		return *this;
	// // breadth-first traversal
	// case bst_iterator_type::level_order: {
	//     // If iterator is at the end, return to the beginning.
	//     if (!node) {
	//         node = root;
	//         return *this;
	//     }

	//     // Otherwise we are somewhere in the middle of th tree.

	//     // Go up until we can go to the right.
	//     // We can only go to the right if we came from the left
	//     size_t level = 0;
	//     while (node->parent) {
	//         if (node->parent->left.get() == node && node->parent->right) {
	//             node = node->parent->right.get();

	//             bool level_ended_while_retracing_steps = false;

	//             // Go down the same number of levels as we went up.
	//             // Prioritize going down to the left.
	//             while (level > 0) {
	//                 if (node->left) {
	//                     node = node->left.get();
	//                 } else if (node->right) {
	//                     node = node->right.get();
	//                 } else {
	//                     // We are in a dead end.
	//                     // Go up until we can go to the right.
	//                     while (node->parent) {
	//                         if (node->parent->left.get() == node &&
	//                         node->parent->right) {
	//                             node = node->parent->right.get();
	//                             level++;
	//                             break;
	//                         }

	//                         // Otherwise we came from the right or there is
	//                         no right child to visit.
	//                         // Continue going up.
	//                         node = node->parent;
	//                         level++;
	//                     }

	//                     // If we are back at the root there's no more nodes
	//                     at this level. if (!node->parent) {
	//                         level_ended_while_retracing_steps = true;
	//                         break;
	//                     }
	//                 }

	//                 level--;
	//             }

	//             if (level_ended_while_retracing_steps) {
	//                 break;
	//             }

	//             return *this;
	//         }

	//         // Otherwise we came from the right or there is no right child to
	//         visit.
	//         // Continue going up.
	//         node = node->parent;
	//         level++;
	//     }

	//     // We are at the root and we're coming back from the right.
	//     // Go down the same number of levels as we went up plus one.
	//     // Prioritize going down to the left.
	//     level++;
	//     while (level > 0) {
	//         if (node->left) {
	//             node = node->left.get();
	//         } else if (node->right) {
	//             node = node->right.get();
	//         } else {
	//             // We are in a dead end.
	//             // Go up until we can go to the right.
	//             while (node->parent) {
	//                 if (node->parent->left.get() == node &&
	//                 node->parent->right) {
	//                     node = node->parent->right.get();
	//                     level++;
	//                     break;
	//                 }

	//                 // Otherwise we came from the right or there is no right
	//                 child to visit.
	//                 // Continue going up.
	//                 node = node->parent;
	//                 level++;
	//             }

	//             // If we are back at the root there's no more nodes at this
	//             level. if (!node->parent) {
	//                 node = nullptr;
	//                 return *this;
	//             }
	//         }

	//         level--;
	//     }

	//     return *this;
	// }
	// ???
	default:
		return *this;
	}
}

template <typename Value>
bst_iterator<Value> &bst_iterator<Value>::operator--() noexcept {
	switch (_type) {
	// in-order traversal
	case bst_iterator_type::in_order:
		// If iterator is at the end (before beginning), return to prev(end).
		if (!node) {
			node = root;

			// Go to the right as far as possible.
			while (node->right) {
				node = node->right.get();
			}

			return *this;
		}

		// Try to go to the left.
		if (node->left) {
			node = node->left.get();

			// Go to the right as far as possible.
			while (node->right) {
				node = node->right.get();
			}

			return *this;
		}

		// Go up until we can go to the left.
		while (node->parent) {
			if (node->parent->right.get() == node) {
				node = node->parent;
				return *this;
			}

			node = node->parent;
		}

		// We are at the root and we're coming back from the left.
		node = nullptr;
		return *this;
	// pre-order traversal
	case bst_iterator_type::pre_order:
		// If iterator is at the end (before beginning), return to prev(end).
		if (!node) {
			node = root;

			// Go down as far as possible.
			// Prioritize going down to the right.
			while (node->left || node->right) {
				if (node->right) {
					node = node->right.get();
				} else {
					node = node->left.get();
				}
			}

			return *this;
		}

		// Go one up.
		// If we can go to the left, go to the left and then down.
		// We can only go to the left if we came from the right.
		if (node->parent) {
			if (node->parent->right.get() == node && node->parent->left) {
				node = node->parent->left.get();

				// Go down as far as possible.
				// Prioritize going down to the right.
				while (node->left || node->right) {
					if (node->right) {
						node = node->right.get();
					} else {
						node = node->left.get();
					}
				}

				return *this;
			}

			node = node->parent;
			return *this;
		}

		// We are at the root and we're coming back from the left.
		node = nullptr;
		return *this;
	case bst_iterator_type::post_order:
		// If iterator is at the end (before beginning), return to prev(end).
		// Post-order traversal finishes at the root.
		if (!node) {
			node = root;
			return *this;
		}

		// Go one down.
		// If there's no path down, go up until we can descend to the left.
		// Stop each time we descend to any side.
		if (node->left || node->right) {
			if (node->right) {
				node = node->right.get();
			} else {
				node = node->left.get();
			}

			return *this;
		}

		// Go up until we can go to the left.
		while (node->parent) {
			if (node->parent->right.get() == node && node->parent->left) {
				node = node->parent->left.get();
				return *this;
			}

			node = node->parent;
		}

		// We are at the root and we're coming back from the left.
		node = nullptr;
		return *this;
	// case bst_iterator_type::level_order: {
	//     // If iterator is at the end (before beginning), return to prev(end).
	//     if (!node) {
	//         node = root;

	//         // Find the lowest node.
	//         bst_node *lowest = node;
	//         std::size_t lowest_level = 0;

	//         // Go down as far as possible.
	//         std::size_t level = 0;
	//         while (node->left || node->right) {
	//             if (level >= lowest_level) {
	//                 lowest = node;
	//                 lowest_level = level;
	//             }

	//             if (node->left) {
	//                 node = node->left.get();
	//                 level++;
	//             } else if (node->right) {
	//                 node = node->right.get();
	//                 level++;
	//             } else {
	//                 // We are in a dead end.
	//                 // Go up until we can go to the right.
	//                 while (node->parent) {
	//                     if (node->parent->left.get() == node &&
	//                     node->parent->right) {
	//                         node = node->parent->right.get();
	//                         level++;
	//                         break;
	//                     }

	//                     // Otherwise we came from the right or there is no
	//                     right child to visit.
	//                     // Continue going up.
	//                     node = node->parent;
	//                     level--;
	//                 }
	//             }
	//         }
	//     }

	//     // Otherwise we are somewhere in the middle of the tree.

	//     // Go up until we can go to the left. Then go down to the right as
	//     far as possible.
	//     // If we stop in a dead end without returning to the same level, we
	//     retrace steps.
	//     // We retrace by going up until we can go to the left. Then go down
	//     to the right as far as possible.
	//     // If we end up at the root, this level is finished.
	//     // In this case we go up one level and find the last node in this
	//     parent level.
	//     // To do that we go down to the right as far as possible.
	//     // If we can't reach the new level, we retrace steps again.
	//     // The scheme continues on until we reach the root level.

	//     // Go up until we can go to the left.
	//     while (node->parent) {
	//         if (node->parent->right.get() == node && node->parent->left) {
	//             node = node->parent->left.get();

	//             // Go down as far as possible.
	//             while (node->left || node->right) {
	//                 if (node->right) {
	//                     node = node->right.get();
	//                 } else {
	//                     node = node->left.get();
	//                 }
	//             }

	//             return *this;
	//         }

	//         node = node->parent;
	//     }
	//     // TODO: UNFINISHED
	// }
	// ???
	default:
		return *this;
	}
}

template <typename Value>
bst_iterator<Value> bst_iterator<Value>::operator++(int) {
	bst_iterator old = *this;
	++(*this);
	return old;
}

template <typename Value>
bst_iterator<Value> bst_iterator<Value>::operator--(int) {
	bst_iterator old = *this;
	--(*this);
	return old;
}

template <typename Value>
bst_iterator<Value> &bst_iterator<Value>::operator+=(difference_type n
) noexcept {
	if (n < 0) {
		while (n++) {
			--(*this);
		}
	}

	// n >= 0
	while (n--) {
		++(*this);
	}

	return *this;
}

template <typename Value>
bst_iterator<Value> &bst_iterator<Value>::operator-=(difference_type n
) noexcept {
	return (*this += -n);
}

template <typename Value>
bool bst_iterator<Value>::operator==(const bst_iterator &other) const noexcept {
	return node == other.node;
}

template <typename Value>
bool bst_iterator<Value>::operator!=(const bst_iterator &other) const noexcept {
	return !(*this == other);
}

template <typename Value>
typename bst_iterator<Value>::reference bst_iterator<Value>::operator*() const {
	return node->value;
}

template <typename Value>
typename bst_iterator<Value>::pointer bst_iterator<Value>::operator->() const {
	return &node->value;
}

template <typename Value>
bst_iterator_type bst_iterator<Value>::type() const noexcept {
	return _type;
}

template <typename Value>
void bst_iterator<Value>::type(bst_iterator_type type) noexcept {
	_type = type;
}

template <typename Value>
bst_iterator<Value> bst_iterator<Value>::parent() const noexcept {
	auto it = bst_iterator{*this};

	if (node) {
		it.node = node->parent;
	}

	return it;
}

template <typename Value>
bst_iterator<Value> bst_iterator<Value>::left() const noexcept {
	auto it = bst_iterator{*this};

	if (node) {
		it.node = node->left.get();
	}

	return it;
}

template <typename Value>
bst_iterator<Value> bst_iterator<Value>::right() const noexcept {
	auto it = bst_iterator{*this};

	if (node) {
		it.node = node->right.get();
	}

	return it;
}

template <typename Value>
bst_iterator<Value>::bst_iterator(
    _internal::bst_node<value_type> *root,
    _internal::bst_node<value_type> *node,
    bst_iterator_type                type
) noexcept
    : root(root), node(node), _type(type) {}

} // namespace opal
