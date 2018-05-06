#include "testkit.hh"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <random>
#include <algorithm>

std::mt19937 generator;

int random(int min, int max) {
	std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
	return dist(generator);
}

void random_seed() {
	generator.seed(std::random_device()());
}

void shuffle(std::vector<int> vector) {
	std::shuffle(std::begin(vector), std::end(vector), generator);
}


long int benchmark(int passes, bool print_passes,
	std::function<void()> before,
	std::function<void()> measure,
	std::function<void()> after) {

	long int average = 0;
	for (int i = 0; i < passes; i += 1) {
		before();
		auto t0 = std::chrono::steady_clock::now();
		measure();
		auto t1 = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
		after();
		average += dt / passes;
		if (print_passes) {
			std::cerr << "[info] pass " << i+1 << " = " << dt << " ns\n";
		}
	}
	return average;
}

long int benchmark_simple(std::function<void()> measure) {
	return benchmark(1, false, []{}, measure, []{});
}

int *copy_array(int* array, int length) {
	size_t memory = sizeof(int) * length;
	int *copy;
	copy = (int*) malloc(memory);
	if (!copy) {
		std::cerr << "[error] make_copy: could allocate memory!\n";
		exit(1);
	}
	copy = (int*) memcpy(copy, array, memory);
	if (!copy) {
		std::cerr << "[error] make_copy: could copy array!\n";
		exit(1);
	}
	return copy;
}

void print_array(int *array, int length) {
	for (int i = 0; i < length; i += 1) 
		std::cerr << array[i] << " ";
	std::cerr << "\n";
}

