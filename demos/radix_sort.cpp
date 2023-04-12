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

constexpr std::size_t  n     = 1000000;
constexpr std::int32_t range = 100000000;

template <typename Sort, typename TestComparator = decltype(std::less{})>
void benchmark_sort(
    Sort                      sort,
    std::vector<std::int32_t> numbers,
    TestComparator            comparator = TestComparator{}
) {
	std::chrono::steady_clock::time_point start =
	    std::chrono::steady_clock::now();

	sort(numbers.begin(), numbers.end());

	std::chrono::steady_clock::time_point end =
	    std::chrono::steady_clock::now();

	std::cout << "Sorting took "
	          << std::chrono::duration_cast<std::chrono::milliseconds>(
	                 end - start
	             )
	                 .count()
	          << " ms." << std::endl;

	std::cout << "Verifying validity..." << std::endl;

	if (std::is_sorted(numbers.begin(), numbers.end(), comparator)) {
		std::cout << "Sequence is properly sorted." << std::endl;
	} else {
		std::cout << "Sequence is not properly sorted." << std::endl;
	}
}

int main() {
	std::vector<std::int32_t> numbers;
	numbers.reserve(n);

	std::mt19937                                rng;
	std::uniform_int_distribution<std::int32_t> dist{-range, range};

	std::cout << "Generating " << n << " random numbers in [" << -range << ", "
	          << range << ")..." << std::endl;

	for (std::size_t i = 0; i < n; i++) {
		numbers.push_back(dist(rng));
	}

	std::cout << std::endl
	          << "Benchmarking opal::radix_sort (radix 256)..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::radix_sort(begin, end);
	    },
	    numbers
	);

	std::cout << std::endl
	          << "Benchmarking opal::counting_sort..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::counting_sort(begin, end);
	    },
	    numbers
	);

	std::cout << std::endl
	          << "Benchmarking opal::radix_sort (radix 1024)..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::radix_sort(begin, end, 1024);
	    },
	    numbers
	);

	std::cout << std::endl << "Benchmarking std::sort..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    std::sort(begin, end);
	    },
	    numbers
	);

	std::cout << std::endl << "Sorting in reverse (radix 256)..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::radix_sort<false>(begin, end);
	    },
	    numbers,
	    std::greater{}
	);

	std::vector<std::string> strings{
	    "hello", "world", "this", "is", "a", "test"};

	std::cout << std::endl;
	std::cout << "String array before sorting:" << std::endl;
	print_vector(strings);

	// This would also work with opal::counting_sort. Just much slower.
	opal::radix_sort(
	    strings.begin(),
	    strings.end(),
	    [](const std::string &str) {
		    // Convert string to unsigned integer.
		    // Each character is treated as [0, 26) digit.

		    std::size_t result = 0;

		    for (std::size_t i = 0; i < 5; i++) {
			    result *= 26;
			    if (i < str.size()) {
				    result += str[i] - 'a';
			    }
		    }

		    return result;
	    },
	    std::pow(26, 6) - 1,
	    26
	);

	std::cout << std::endl;
	std::cout << "String array after sorting lexically:" << std::endl;
	print_vector(strings);

	return 0;
}
