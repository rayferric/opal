#include <cstdint>
#include <iostream>
#include <vector>

#include <opal/opal.hpp>

template <typename T>
void print_vector(const std::vector<T> &v) {
	for (const auto &e : v) {
		std::cout << e << " ";
	}
	std::cout << std::endl;
}

template <typename T>
void print_heap(const std::vector<T> &v) {
	for (std::size_t i = 0; i < v.size(); i++) {
		std::size_t level       = std::log2(i + 1);
		std::size_t level_begin = (static_cast<std::size_t>(1) << level) - 1;

		if (i == level_begin && i != 0) {
			std::cout << std::endl;
		}

		std::cout << v[i] << " ";
	}

	std::cout << std::endl;
}

int main() {
	std::vector<std::int32_t> numbers{7, 11, -1, -2, 9, 11, 0, -1};

	std::cout << "Array before sorting:" << std::endl;
	print_vector(numbers);
	std::cout << std::endl;

	opal::heap_sort(
	    numbers.begin(),
	    numbers.end(),
	    [&](const auto &a, const auto &b) {
		    std::cout << "Heap during comparison of " << a << " and " << b
		              << ":" << std::endl;
		    print_heap(numbers);
		    return a < b;
	    }
	);

	std::cout << std::endl;
	std::cout << "Array after sorting:" << std::endl;
	print_vector(numbers);

	opal::heap_sort(numbers.begin(), numbers.end(), std::greater{});

	std::cout << std::endl;
	std::cout << "Array sorted in reverse order:" << std::endl;
	print_vector(numbers);

	std::vector<std::string> strings{
	    "hello", "world", "this", "is", "a", "test"};

	std::cout << std::endl;
	std::cout << "String array before sorting:" << std::endl;
	print_vector(strings);

	opal::heap_sort(strings.begin(), strings.end());

	std::cout << std::endl;
	std::cout << "String array after sorting:" << std::endl;
	print_vector(strings);

	return 0;
}
