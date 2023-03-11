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

int main() {
	std::vector<int32_t> numbers{7, 11, -1, -2, 9, 11, 0, -1};

	std::cout << "Array before sorting:" << std::endl;
	print_vector(numbers);
	std::cout << std::endl;

	opal::insertion_sort(
	    numbers.begin(),
	    numbers.end(),
	    [&](const auto &a, const auto &b) {
		    std::cout << "Array during comparison of " << a << " and " << b
		              << ":" << std::endl;
		    print_vector(numbers);
		    return a < b;
	    }
	);

	std::cout << std::endl;
	std::cout << "Array after sorting:" << std::endl;
	print_vector(numbers);

	opal::insertion_sort(numbers.begin(), numbers.end(), std::greater{});

	std::cout << std::endl;
	std::cout << "Array sorted in reverse order:" << std::endl;
	print_vector(numbers);

	std::vector<std::string> strings{
	    "hello", "world", "this", "is", "a", "test"};

	std::cout << std::endl;
	std::cout << "String array before sorting:" << std::endl;
	print_vector(strings);

	opal::insertion_sort(strings.begin(), strings.end());

	std::cout << std::endl;
	std::cout << "String array after sorting:" << std::endl;
	print_vector(strings);

	return 0;
}
