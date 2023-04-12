#pragma once

namespace opal::_internal {

template <typename Value>
class bst_node {
public:
	Value                     value;
	bst_node                 *parent;
	std::unique_ptr<bst_node> left, right;

	bst_node(Value &&value, bst_node *parent);
};

template <typename Value>
bst_node<Value>::bst_node(Value &&value, bst_node *parent)
    : value(std::move(value)), parent(parent) {}

} // namespace opal::_internal
