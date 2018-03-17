#include "testkit.hh"
#include <functional>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>


//
// generowanie danych
//

int *new_array(int length) {
	int *array = (int*) malloc(sizeof(int) * length);
	if (!array) {
		std::cerr << "[error] make_random: could allocate memory!\n";
		exit(1);
	}
	return array;
}

int *generate_random_array(int min, int max, int length) {
	int *array = new_array(length);
	for (int i = 0; i < length; i += 1) 
		array[i] = random(min, max);
	return array;
}

int *generate_increasing_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i < length; i += 1) 
		array[i] = i;
	return array;
}

int *generate_decreasing_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i < length; i += 1) 
		array[i] = length - i - 1;
	return array;
}

int *generate_constant_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i < length; i += 1) 
		array[i] = 1;
	return array;
}

int *generate_a_shape_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i <= length / 2; i += 1) 
		array[i] = 2*i + 1;
	for (int i = length / 2; i < length; i += 1) 
		array[i] = 2*(length - i);
	return array;
}

int *generate_v_shape_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i <= length / 2; i += 1) 
		array[i] = 2*(length - i) + 1;
	for (int i = length / 2; i < length; i += 1) 
		array[i] = 2*i;
	return array;
}

//
// implementacja algorytmow
//

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void selection_sort(int *a, int len) {
	for (int i = 0; i < len - 1; i += 1) {
		int min = i;
		for (int j = i + 1; j < len; j += 1) {
			if (a[j] < a[min]) {
				min = j;
			}
		}
		swap(&a[min], &a[i]);
	}
}

void insertion_sort(int *a, int len) {
	for (int i = 0; i < len; i += 1) {
		for (int j = i; i > 0 && a[j] < a[j-1]; j -= 1) {
			swap(&a[j], &a[j-1]);
		}
	}
}

void bubble_sort(int *a, int len) {
	for (int i = 0; i < len; i += 1) {
		for (int j = i + 1; j < len; j += 1) {
			if(a[j] > a[j+1]) {
				swap(&a[j], &a[j+1]);
			}
		}
	}
}

void quick_sort(int *a, int len) {
	if (len < 2) return;
//	int pivot = a[random(0, len - 1)];
	int pivot = a[len / 2];
	int i, j;
	for (i = 0, j = len - 1; ; i++, j--) {
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i >= j) break;
		swap(&a[i], &a[j]);
	}
	quick_sort(a, i);
	quick_sort(a + i, len - i);
}

void heap_sort(int *a, int len) {
}

void shell_sort(int *a, int len) {
}

void merge_sort(int *a, int len) {
}

void counting_sort(int *a, int len) {
	int b_len = len + 1;
	int *b = (int*) calloc(b_len, sizeof(int));
	for (int i = 0; i < len; i += 1) {
		b[i] += 1;
	}

	int p = 0;
	for (int i = 0; i < b_len; i += 1) {
		for (int j = b[i]; j > 0; j -= 1) {
			a[p] = i;
			p += 1;
		}
	}
	free(b);
}

void cpp_sort(int *a, int len) {
	std::sort(a, a + len);
}

//
// pomiary
//

int main() {
	random_seed();

	using namespace std;
	typedef function<void(int*, int)> algorithm;
	typedef function<int*(int)> generator;

	int len_min = 1000, len_max = 10000, len_step = 500;

	vector<pair<string, generator>> generators;
	generators.push_back(make_pair("losowe",
		[](int len) { return generate_random_array(1, 1000, len); }));
	generators.push_back(make_pair("stałe", &generate_constant_array));
	generators.push_back(make_pair("rosnące", &generate_increasing_array));
	generators.push_back(make_pair("malejące", &generate_decreasing_array));
	generators.push_back(make_pair("a-kształtne", &generate_a_shape_array));
	generators.push_back(make_pair("v-kształtne", &generate_v_shape_array));

	vector<pair<string, algorithm>> algorithms;
//	algorithms.push_back(make_pair("bubble sort",    &bubble_sort));
//	algorithms.push_back(make_pair("selection sort", &selection_sort));
	algorithms.push_back(make_pair("insertion sort", &insertion_sort));
	algorithms.push_back(make_pair("quick sort",     &quick_sort));
	algorithms.push_back(make_pair("c++ sort",       &cpp_sort));
	algorithms.push_back(make_pair("counting sort",  &counting_sort));

	// wydrukuj naglowek csv
	cout << "length";
	for (auto generator : generators) {
		for (auto algorithm : algorithms) {
			cout << "," << algorithm.first << " - " << generator.first;
		}
	}
	cout << "\n";


	for (int len = len_min; len <= len_max; len += len_step) {
		cerr << 100 * len / len_max << "% ";
		cout << len;
		for (auto generator : generators) {
			int *array_orig = generator.second(len);
			int *array_copy;
			auto before = [&array_orig, &array_copy, len]() {
				//array_copy = copy_array(array_orig, len);
				return;
			};
			auto after = [&array_copy]() {
				//free(array_copy);
				return;
			};
			

			for (auto algorithm : algorithms) {
				auto measure = [algorithm, &array_orig, len]() {
					int *copy = copy_array(array_orig, len);
					algorithm.second(copy, len);
					free(copy);
				};

				auto nanoseconds = benchmark(5, false, before, measure, after);
				// dostajemy sekundy
				cout << "," << fixed << nanoseconds * 10e-9 << flush;
			}

			free(array_orig);
		}
		cout << "\n";
	}
	cerr << "\n";

	return 0;
}

