#include "testkit.hh"

typedef std::function<void(int*, int)> algorithm;
typedef std::function<int*(int)> generator;

using std::cerr;

// sort inplace  best  avg   worst
// QS x nlogn nlogn n2
// HS x nlogn nlogn nlogn
// MS   nlogn nlogn nlogn
// SS x n1.25 n1.25 n1.25
// CS   n+k
// 
// quicksort z środkowym elementem
// ciąg v-kształtny 1 3 5-7-6 4 2

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

//
// generowanie danych
//

int *new_array(int length) {
	int *array = (int*) calloc(length, sizeof(int));
	if (!array) {
		cerr << "[error] new_array: could allocate memory!\n";
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

//
// shit sort
//

void bubble_sort(int *a, int len) {
	for (int i = 0; i < len; i += 1) {
		for (int j = i + 1; j < len; j += 1) {
			if(a[j] > a[j+1]) {
				swap(&a[j], &a[j+1]);
			}
		}
	}
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

//
// quick sort
//

// algorytm z ksiazki algorytmy + struktury danych = programy
// l: left, r: right, p: pivot
void quick_sort_asp(int *a, int l, int r) {
	int p = a[ (l + r) / 2 ];
	int i = l, j = r;
	while (i <= j) {
		while (a[i] < p) i++;
		while (a[j] > p) j--;
		if (i > j) break;
		swap(&a[i], &a[j]);
		i++; j--;
	}
	if (l < j) quick_sort_asp(a, l, j);
	if (i < r) quick_sort_asp(a, i, r);
}

void quick_sort(int *a, int len) {
	quick_sort_asp(a, 0, len);
}


//
// heap sort
//

void heapify(int *a, int len, int i) {
	int max = i;
	int l = 2*i + 1;
	int r = 2*i + 2;
	if (l < len && a[l] > a[max]) max = l;
	if (r < len && a[r] > a[max]) max = r;
	if (max != i) {
		swap(&a[i], &a[max]);
		heapify(a, len, max);
	}
}

void heap_sort(int *a, int len) {
	for (int i = len / 2 - 1; i >= 0; i -= 1) {
		heapify(a, len, i);
	}
	for (int i = len - 1; i >= 0; i -= 1) {
		swap(&a[0], &a[i]);
		heapify(a, i, 0);
	}
}

//
// shell sort
//

void insertion_sort_gap(int *a, int len, int gap) {
	for (int i = gap; i < len; i += gap) {
		for (int j = i; j >= gap && a[j - gap] > a[j]; j -= gap) {
			swap(&a[j - gap], &a[j]);
		}
	}
}

void shell_sort(int *a, int len) {
	for (int k = len / 2; k > 0; k /= 2) {
		for (int i = 0; i < k; i += 1) {
			insertion_sort_gap(a + i, len - i, k);
		}
	}
}

//
// merge sort
//

// n = lenth of a, p = first index of the second sublist
void merge(int *a, int n, int p) {
	int *A = copy_array(a, n);
	for (int i = 0, l = 0, r = p; i < n;) {
		if ((A[l] <= A[r] || r >= n) && l < p)
			a[i++] = A[l++];

		if ((A[r] <= A[l] || l >= p) && r < n)
			a[i++] = A[r++];
	}
	free(A);
}

void merge_sort(int *a, int n) {
	if (n <= 1) { 
		return;
	} else if (n == 2) {
		merge(a, n, 1);
	} else {
		int q = n % 2 == 0 ? 0 : 1;
		int p = n / 2;
		merge_sort(a, p);
		merge_sort(a + p, p + q);
		merge(a, n, p);
	}
}

//
// counting sort
//

void counting_sort(int *a, int len) {
	int *b = (int*) calloc(len, sizeof(int));
	for (int i = 0; i < len; i += 1) {
		b[i] += 1;
	}

	for (int i = 0, p = 0; i < len; i += 1) {
		for (int j = b[i]; j > 0; j -= 1) {
			a[p++] = i;
		}
	}
	free(b);
}

//
// c++ sort
//

void cpp_sort(int *a, int len) {
	std::sort(a, a + len);
}

//
// pomiary
//

void bench() {
	using namespace std;

	int len_min = 1000, len_max = 2000000, len_step = 1000;

	vector<pair<string, generator>> generators({
		make_pair("losowe", [](int len) { return generate_random_array(1, 1000, len); }),
		make_pair("stałe",       &generate_constant_array),
		make_pair("rosnące",     &generate_increasing_array),
		make_pair("malejące",    &generate_decreasing_array),
		make_pair("a-kształtne", &generate_a_shape_array),
		make_pair("v-kształtne", &generate_v_shape_array)
	});

	vector<pair<string, algorithm>> algorithms({
		make_pair("bubble sort",    &bubble_sort),
		make_pair("quick sort",     &quick_sort),
		make_pair("merge sort",     &merge_sort),
		make_pair("heap sort",      &heap_sort),
		make_pair("shell sort",     &shell_sort),
		make_pair("counting sort",  &counting_sort),
		make_pair("c++ sort",       &cpp_sort)
	});

	// wydrukuj naglowek
	cout << "length";
	for (auto generator : generators)
		for (auto algorithm : algorithms)
			cout << "," << algorithm.first << " - " << generator.first;
	cout << "\n";

	for (int len = len_min; len <= len_max; len += len_step) {
		cout << len;
		for (auto generator : generators) {
			int *array_orig = generator.second(len);
			int *array_copy;
			auto before = [&array_orig, &array_copy, len]() {
				array_copy = copy_array(array_orig, len);
			};
			auto after = [&array_copy]() {
				free(array_copy);
			};
			for (auto algorithm : algorithms) {
				bool a = generator.first == "a-kształtne";
				bool v = generator.first == "v-kształtne";
				bool q = algorithm.first == "quick sort";
				bool b = algorithm.first == "bubble sort";
				if ((q && (a || v) && len > 15000) || (b && len > 15000))  {
					cout << "," << "NaN" << flush;
					continue;
				}
				auto measure = [algorithm, &array_copy, len]() {
					algorithm.second(array_copy, len);
				};
				auto t = benchmark(1, measure, before, after);
				cout << "," << fixed << t * 1e-9 << flush;
				cerr << ".";
			}
			free(array_orig);
		}
		cout << "\n";
		cerr << "\n";

		if (len == 15000) {
			len_step = 50000;
			len = 50000;
		}
	}
	cerr << "\n";
}

void test() {
	using namespace std;

	for (string shape : {"v","a"}) {
		cout << shape;
		for (int i = 99999; i <= 100001; i++) {
			int *a;
			auto before = [&a, i, shape]() {
				if (shape == "v")
					a = generate_v_shape_array(i);	
				if (shape == "a")
					a = generate_a_shape_array(i);
			};
			auto after = [&a]() {
				free(a);
			};
			auto measure = [&a, i]() {
				quick_sort(a, i);
			};
			auto t = benchmark(25, measure, before, after);
			cout << "," << fixed << t * 1e-9 << flush;
		}
		cout << "\n";
	}
}

void usage(bool abort = true) {
	cerr << "usage: benchmark [--bench|--test]\n";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	random_seed();

	if (argc != 2)
		usage();

	if (!strcmp(argv[1], "--bench"))
		bench();

	if (!strcmp(argv[1], "--test"))
		test();

	return 0;
}

