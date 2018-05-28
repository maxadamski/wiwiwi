#include "testkit.hh"

typedef std::function<void(int*, int)> algorithm;
typedef std::function<int*(int)> generator;

using std::cerr;

// Pozwolisz, że wtrącę się na momencik. Rzecz do której się odnosisz jako Linux, to w gruncie rzeczy GNU/Linux lub jak ostatnio zacząłem to nazywać - GNU plus Linux. Sam Linux nie jest systemem operacyjnym, lecz kolejnym wolnym (od wolności) składnikiem w pełni funkcjonalnego systemu GNU, użytecznego dzięki głównym bibliotekom GNU, narzędziom powłoki oraz niezbędnym składnikom systemu, składającym się na kompletny system operacyjny, zdefiniowany przez POSIX (ang. Portable Operating System Interface for Unix – przenośny interfejs dla systemu operacyjnego Unix). Wielu użytkowników komputera, nie zdając sobie z tego sprawy, codziennie używa zmodyfikowanej wersji systemu GNU. Poprzez dziwny zbieg okoliczności, wersja GNU używana na szeroką skalę, nazywana jest "Linux" i wielu jego użytkowników nie jest świadomych, że w rzeczy samej jest to system GNU rozwinięty przez GNU Project. Linux naprawdę istnieje i ci ludzie go używają, ale to jest tylko jeden ze składników systemu operacyjnego. Linux to jądro - program w systemie operacyjnym, odpowiedzialny za przydzielanie zasobów sprzętowych do innych programów których używasz. Jądro jest niezbędną częścią systemu operacyjnego, ale samo jest bezużyteczne - może funkcjonować jedynie w odniesieniu do całego systemu operacyjnego. Linux jest zazwyczaj używany w połączeniu z systemem operacyjnym GNU - cały system to w zasadzie GNU z dodanym Linuxem, lub GNU/Linux. Wszystkie tak zwane dystrybucje "Linuxa" to w rzeczy samej dystrybucje GNU/Linux.

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
	for (int i = 0; i < length / 2; i += 1) 
		array[i] = 2*i + 1;
	for (int i = length / 2; i < length; i += 1) 
		array[i] = 2*(length - i);
	return array;
}

int *generate_v_shape_array(int length) {
	int *array = new_array(length);
	for (int i = 0; i < length / 2; i++) 
		array[i] = 2*(length / 2 - i) + 1;
	for (int i = length / 2; i < length; i++) 
		array[i] = 2*(i - length / 2);
	return array;
}

//
// implementacja algorytmow
//

//
// shit sort
//

void bubble_sort(int *a, int len) {
	for (int i = 1; i < len; i += 1) {
		for (int j = 0; j < len - 1; j += 1) {
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
	for (int i = 1; i < len; i++) {
		for (int j = i; j > 0 && a[j-1] > a[j]; j--) {
			swap(&a[j], &a[j-1]);
		}
	}
}

//
// quick sort
//

// algorytm z ksiazki algorytmy + struktury danych = programy
// l: left, r: right, p: pivot
void quick_sort_asp(int *a, int L, int R) {
	int p = a[ (L + R) / 2 ];
	int l = L, r = R;
	while (l <= r) {
		while (a[l] < p) l++;
		while (a[r] > p) r--;
		if (l > r) break;
		swap(&a[l], &a[r]);
		l++; r--;
	}
	if (L < r) quick_sort_asp(a, L, r);
	if (l < R) quick_sort_asp(a, l, R);
}

void quick_sort(int *a, int len) {
	quick_sort_asp(a, 0, len-1);
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
	for (int gap = len / 2; gap > 0; gap /= 2) {
		for (int i = 0; i < gap; i += 1) {
			insertion_sort_gap(a + i, len - i, gap);
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
		// to samo co swap
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
	// already sorted
	if (len <= 1) return;

	// find smallest and biggest elements
	int MIN = a[0], MAX = a[0];
	for (int i = 0; i < len; i++) {
		if (a[i] > MAX) MAX = a[i];
		if (a[i] < MIN) MIN = a[i];
	}

	int range = MAX - MIN + 1;
	int *count = (int*) calloc(range, sizeof(int));

	// create element histogram
	for (int i = 0; i < len; i++) {
		count[a[i] - MIN] += 1;
	}

	// e: element, i: count index
	for (int e = MIN, a_i = 0; e <= MAX; e++) {
		for (int i = e - MIN; count[i] > 0; count[i]--) {
			a[a_i++] = e;
		}
	}

	free(count);
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

	//int *V = generate_v_shape_array(10);
	//int *V1 = generate_v_shape_array(11);
	//int *A = generate_a_shape_array(10);
	//int *A1 = generate_a_shape_array(11);
	//print(V, 10);
	//print(V1, 11);
	//print(A, 10);
	//print(A1, 11);
	//return;

	vector<pair<string, generator>> generators({
		make_pair("losowe", [](int len) { return generate_random_array(1, 1000, len); }),
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
	});

	for (auto generator : generators) {
		cout << "+++++++++++++++++++++++\n";
		cout << generator.first << "\n";
		int len = 100;

		for (auto algorithm : algorithms) {
			cout << "-----------------------\n";
			cout << algorithm.first << ":\n";

			int *a = generator.second(len);
			print(a, len);
			algorithm.second(a, len);
			print(a, len);
			free(a);
		}
	}

	cout << "+++++++++++++++++++++++\n";
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
			auto t = benchmark(1, measure, before, after);
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

