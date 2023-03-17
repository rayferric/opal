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

constexpr size_t n = 1000000;

template <typename Sort>
void benchmark_sort(Sort sort, std::vector<int32_t> numbers) {
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

	if (std::is_sorted(numbers.begin(), numbers.end())) {
		std::cout << "Sequence is properly sorted." << std::endl;
	} else {
		std::cout << "Sequence is not properly sorted." << std::endl;
	}
}

int main() {
	std::vector<int32_t> numbers;
	numbers.reserve(n);

	std::mt19937                           rng;
	std::uniform_int_distribution<int32_t> dist(0, n);

	std::cout << "Generating " << n << " random numbers..." << std::endl;

	for (int32_t i = 0; i < n; ++i) {
		numbers.push_back(dist(rng));
	}

	std::cout << std::endl
	          << "Benchmarking single-threaded opal::merge_sort..."
	          << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::merge_sort(begin, end);
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

	std::cout << std::endl
	          << "Benchmarking parallel opal::merge_sort..." << std::endl;

	benchmark_sort(
	    [](auto begin, auto end) {
		    opal::merge_sort(opal::thread_pool::common_pool(), begin, end);
	    },
	    numbers
	);

	return 0;
}
