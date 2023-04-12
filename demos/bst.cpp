#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include <opal/opal.hpp>

template <typename T>
void print_vector(const std::vector<T> &v) {
	for (const auto &e : v) {
		std::cout << e << " ";
	}
	std::cout << std::endl;
}

constexpr std::size_t n     = 20;
constexpr int32_t     range = 1000;

int main() {
	opal::bst<std::int32_t> tree;

	std::mt19937                                rng;
	std::uniform_int_distribution<std::int32_t> dist{-range, range};

	rng.seed(std::chrono::system_clock::now().time_since_epoch().count());

	std::cout << "Inserting " << n << " random numbers in [" << -range << ", "
	          << range << ")..." << std::endl;

	for (std::size_t i = 0; i < n; i++) {
		tree.insert(dist(rng));
	}

	std::cout << std::endl << tree;

	std::cout << std::endl << "In-order traversal: ";
	std::vector<std::int32_t> in_order_values;
	for (std::int32_t value : tree) {
		std::cout << value << " ";
		in_order_values.push_back(value);
	}
	std::cout << std::endl;

	// Check if in-order traversal is sorted.
	if (std::is_sorted(in_order_values.begin(), in_order_values.end())) {
		std::cout << "In-order traversal is sorted." << std::endl;
	}

	std::cout << std::endl << "Pre-order traversal: ";
	std::vector<std::int32_t> pre_order_values;
	auto it = tree.begin(opal::bst_iterator_type::pre_order);
	while (it) {
		std::cout << *it << " ";
		pre_order_values.push_back(*it);
		++it;
	}
	std::cout << std::endl;

	// Check if pre-order traversal contains the same values as in-order
	// traversal.
	std::sort(pre_order_values.begin(), pre_order_values.end());
	if (pre_order_values == in_order_values) {
		std::cout << "Pre-order traversal visited the same values as in-order "
		             "traversal."
		          << std::endl;
	}

	std::cout << std::endl << "Post-order traversal: ";
	std::vector<std::int32_t> post_order_values;
	it = tree.begin(opal::bst_iterator_type::post_order);
	while (it) {
		std::cout << *it << " ";
		post_order_values.push_back(*it);
		++it;
	}
	std::cout << std::endl;

	// Check if post-order traversal contains the same values as in-order
	// traversal.
	std::sort(post_order_values.begin(), post_order_values.end());
	if (post_order_values == in_order_values) {
		std::cout << "Post-order traversal visited the same values as in-order "
		             "traversal."
		          << std::endl;
	}

	std::cout << std::endl << "Reverse in-order traversal: ";
	auto rit = tree.rbegin();
	while (rit != tree.rend()) {
		std::cout << *rit << " ";
		++rit;
	}
	std::cout << std::endl;

	std::cout << std::endl << "Reverse pre-order traversal: ";
	rit = tree.rbegin(opal::bst_iterator_type::pre_order);
	while (rit != tree.rend(opal::bst_iterator_type::pre_order)) {
		std::cout << *rit << " ";
		++rit;
	}
	std::cout << std::endl;

	std::cout << std::endl << "Reverse post-order traversal: ";
	rit = tree.rbegin(opal::bst_iterator_type::post_order);
	while (rit != tree.rend(opal::bst_iterator_type::post_order)) {
		std::cout << *rit << " ";
		++rit;
	}
	std::cout << std::endl;

	std::size_t size = tree.size();
	std::cout << std::endl
	          << "The size of the tree is " << size << "." << std::endl;

	// Pick random element.
	std::uniform_int_distribution<std::size_t> dist2{0, size - 1};
	std::size_t                                index = dist2(rng);
	std::cout << std::endl
	          << "Picking random value from the tree." << std::endl;
	std::int32_t value = *std::next(tree.begin(), index);
	std::cout << "Picked value: " << value << std::endl;

	// Find the picked value.
	std::cout << "Binary-searching for the picked value..." << std::endl;
	auto bin_it = tree.find(value);

	// Go to the next value in-order.
	std::cout << "Jumping to the next value in-order..." << std::endl;
	++bin_it;

	std::cout << "The next value in-order is "
	          << (bin_it ? std::to_string(*bin_it) : "~") << "." << std::endl;

	// Erase the picked value.
	std::cout << std::endl << "Erasing the picked value..." << std::endl;
	tree.erase(tree.find(value));

	std::cout << std::endl << tree;

	// Insert the picked value again.
	std::cout << std::endl
	          << "Inserting the picked value again..." << std::endl;
	tree.insert(value);

	std::cout << std::endl << tree;

	// Clear the tree.
	std::cout << std::endl << "Clearing the tree..." << std::endl;
	tree.clear();

	std::cout << "Inserting 2, 1, 3..." << std::endl;
	tree.insert(2);
	tree.insert(1);
	tree.insert(3);

	std::cout << std::endl << tree;

	return 0;
}
