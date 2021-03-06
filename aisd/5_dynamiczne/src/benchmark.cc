#include "testkit.hh"

// NOTATKI:
// cykl hamiltona
// O(n*m)
//
// 'b' value should be random?

using namespace std;

// wygoda ponad wszystko :D
typedef struct Item Item;
typedef vector< vector<int> > Matrix;
typedef vector< Item > Items;

struct Item {
	int v, w;
};

bool cmp(Item l, Item r) {
	return (double) l.v / l.w > (double) r.v / r.w;
}

struct Bag {
	int b, y, v = 0, w = 0;
	Items items = {};

	Bag(int b = 0, int y = 0): b(b), y(y) {};

	void add(Item item) {
		items.push_back(item);
		v += item.v;
		w += item.w;
	}
};

// 1. Greedy algorithm (GA)
// "nie rozwiązuje"
// złożoność: O(n log n)
// priority queue Q by v / w
// greedily take from Q

Bag solve_greedy(Bag bag, Items items) {
	// standard C++>03 wymaga, aby sortowanie było zawsze O(n log n)
	sort(items.begin(), items.end(), cmp);
	// greedily add items to the bag
	for (auto item : items) {
		if (bag.w + item.w <= bag.b) {
			bag.add(item);
		}
	}
	return bag;
}

// 2. Brute force (BF)
// "rozwiązuje"
// złożoność: O(2^n)
// sum_{i=1}^n (n choose i) = 2^n

#include <chrono>
bool bf_did_timeout = false;
chrono::steady_clock::time_point bf_t0;
int bf_t_max = 60*60;

void solve_brute_r(int n, Bag bag, Bag &best, Items &items) {
	auto t1 = chrono::steady_clock::now();
	auto dt = chrono::duration_cast<std::chrono::seconds>(t1 - bf_t0).count();
	if (dt >= bf_t_max) {
		bf_did_timeout = true;
		return;
	}

	// my bag is better than your's!
	if (n == 0 && bag.w <= bag.b && bag.v > best.v)
		best = bag;
	// nothing else to add
	if (n == 0)
		return;
	// descend down one level
	solve_brute_r(n - 1, bag, best, items);
	bag.add(items[n - 1]);
	solve_brute_r(n - 1, bag, best, items);
}

Bag solve_brute(Bag bag, Items &items) {
	bf_t0 = chrono::steady_clock::now();
	Bag best = bag;
	solve_brute_r(items.size(), bag, best, items);
	return best;
}

// 3. Dynamic programming (DP)
// "rozwiązuje"
// wykorzystanie procesu Markova
// złożoność: O(n*b)
// i = 0..n
// L = 0..b
// f(0,L) = 0
// f(i,0) = 0
// f(i,L) = f(i-1,L), where s(a_i) > L
// f(i,L) = max(
//	f(i-1, L-s(a_i)) + w(a_i),
//	f(i-1,L)
// ), where L >= S(a_i)
//
// dla b=10, y=12,
// a_i = { {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };
//
// i\L| 0 1 2 3 4 5 6  7  8  9  10
// ---+---------------------------
//  0 | 0 0 0 0 0 0 0  0  0  0  0 
//  1 | 0 0 0 2 2 2 2  2  2  2  2 
//  2 | 0 3 3 3 5 5 5  5  5  5  5 
//  3 | 0 3 3 3 5 5 5  5  6  6  6 
//  4 | 0 3 4 4 5 6 6  6  6  7  7 
//  5 | 0 3 4 4 5 8 11 12 12 13 14
// 
// jakościowo możemy porównywać wszystkie
// możemy porównywać BF i DP czasowo 
// dla problemu optymalizacyjnego plecak jest NP-trudny

void p(Matrix matrix, ostream &out = cerr) {
	out << "    ";
	for (int c = 0; c < matrix[0].size(); c++)
		out << c << " ";
	out << "\n  +-----------------------------\n";

	int i = 0;
	for (auto row : matrix) {
		out << i << " | ";
		for (auto item : row) out << item << " ";
		out << "\n";
		i++;
	}
}

int solve_dynamic_r(int i, int l, Items &items, Matrix &m) {
	if (i == 0 || l == 0) return 0;

	Item a = items[i - 1];
	if (a.w > l) {
		int y = i - 1, x = l;

		if (m[y][x] == -1)
			m[y][x] = solve_dynamic_r(y, x, items, m);

		return m[y][x];
	} else {
		int y1 = i - 1, x1 = l - a.w,
			y2 = i - 1, x2 = l;

		if (m[y1][x1] == -1)
			m[y1][x1] = solve_dynamic_r(y1, x1, items, m);

		if (m[y2][x2] == -1)
			m[y2][x2] = solve_dynamic_r(y2, x2, items, m);

		return max(m[y1][x1] + a.v, m[y2][x2]);
	}
}

Bag solve_dynamic(Bag bag, Items &items) {
	size_t Y = items.size() + 1, X = bag.b + 1;
	Matrix m(Y);

	// initialize matrix
	for (size_t y = 0; y < Y; y++) {
		m[y].resize(X);
		for (size_t x = 0; x < X; x++) m[y][x] = -1;
	}

	// compute entries
	for (size_t y = 0; y < Y; y++) {
		for (size_t x = 0; x < X; x++) {
			m[y][x] = solve_dynamic_r(y, x, items, m);
		}
	}

	p(m);
	bag.v = m[Y-1][X-1];
	bag.w = bag.b;
	return bag;
}


// 4. Generatory instancji

void generate(int n, double nu, Bag &bag, Items &items, double b = -1) {
	int total_v = 0;
	int total_w = 0;

	for (int i = 0; i < n; i++) {
		int v = random(100, 10000);
		int w = random(10, 1000);
		items.push_back({v,w});
		total_v += v;
		total_w += w;
	}

	bag.y = nu * total_v;
	bag.b = b == -1 ? random(10000, 1000000) : b * total_w;
}


void generate_greedy_worst(int n, Bag &bag, Items &items) {
	int b = (n - 1) * 10;
	int w = 10;
	int v = 5;
	int total_v = 0;

	// czyste zło
	int W = b - 9;
	int V = W/2 + 1;
	items.push_back({V, W});
	total_v += V;

	for (int i = 1; i < n; i++) {
		items.push_back({v,w});
		total_v += v;
	}

	bag.y = (n - 1) * v;
	bag.b = b;
}

// 5. Funkcje pomocnicze

void p(Item item, ostream &out = cerr) {
	out << "{v: " << item.v << ", w: " << item.w << "}";
}

void p(Items items, ostream &out = cerr) {
	out << "[ ";
	for (Item item : items) out << item.v << "/" << item.w << " ";
	out << "]";
}

void p(Bag bag, ostream &out = cerr) {
	out << "{ v: " << bag.v
		<< " w: " << bag.w
		<< " items: ";
	p(bag.items, out);
	out << " }";
}

// Zestawy testujące

void quality() {
	cout << "n,b,y,nu,beta,greedy,brute,dynamic\n";
	for (int n = 3; n <= 28; n += 1) {
		Items items; Bag bag;
		generate_greedy_worst(n, bag, items);

		cout << items.size() << "," << bag.b << "," << bag.y << ",0,0,";

		Bag gr = solve_greedy(bag, items);
		cout << (double) gr.v / gr.y << ",";
		cerr << ".";

		Bag bf = solve_brute(bag, items);
		cout << (double) bf.v / bf.y << ",";
		cerr << ".";

		Bag dp = solve_dynamic(bag, items);
		cout << (double) dp.v / dp.y << "\n";
		cerr << ".";
	}
	cerr << "\n";

	for (int n = 100; n <= 1000; n += 100) {
		Items items; Bag bag;
		double nu = 0.5;
		double beta = 0.2;
		generate(n, nu, bag, items, beta);

		cout << items.size() << "," << bag.b << "," << bag.y << "," 
			 << nu << "," << beta << ",";

		Bag gr = solve_greedy(bag, items);
		cout << (double) gr.v / gr.y << ",";
		cerr << ".";
		
		cout << "0,";
		cerr << ".";

		Bag dp = solve_dynamic(bag, items);
		cout << (double) dp.v / dp.y << "\n";
		cerr << ".";
	}

	cerr << "\n";
}

void speed() {
	cout << "n,b,y,nu,brute,greedy,dynamic\n";

	double nu = 1;
	int stop = 28, step = 1;
	for (int n = 12; n <= stop; n += step) {
		Items items; Bag bag;
		generate(n, nu, bag, items);

		cout << items.size() << "," << bag.b << "," << bag.y << ","
			 << nu << ",";

		if (n <= 28) {
			cout << benchmark(1, [bag, &items]{
				solve_brute(bag, items);
			}) * 1e-9;
		} else {
			cout << 0;
		}
		cout << ",";
		cerr << ".";

		cout << benchmark(1, [bag, &items]{
			solve_greedy(bag, items);
		}) * 1e-9;
		cout << ",";
		cerr << ".";

		cout << benchmark(1, [bag, &items]{
			solve_dynamic(bag, items);
		}) * 1e-9;
		cout << "\n";
		cerr << ".";
		cerr << "\n";

		if (n == 28) {
			n = 100, stop = 2000, step = 100;
		}
	}

	cerr << "\n";
}

void surf() {
	cout << "b\\n";
	vector<int> ns = {10, 50, 100, 500, 1000, 2000, 5000, 10000, 15000, 20000, 25000, 30000};
	for (int n : ns) cout << "," << n ;
	cout << "\n";


	for (int b : ns) {
		cout << b;
		for (int n : ns) {
			Items items; Bag bag;
			generate(n, 1, bag, items, b);
			cout << "," << benchmark(1, [bag, &items]{
				solve_dynamic(bag, items);
			}) * 1e-9;
			cerr << ".";
		}
		cout << "\n";
		cerr << "\n";
	}
}

void test() {
	// "Suita testowa":
	Items items = { {1, 3}, {2,5}, {4,3}, {2,2}, {5,7}, {13,47} };
	Bag bag(12, 10);

	Bag dp = solve_dynamic(bag, items);
	cerr << "dp: " << dp.v << "\n";
}

void usage(bool abort = true) {
	cerr << "usage: benchmark --[quality|speed|test]";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	auto args = parse_args(argc, argv);
	random_seed();

	if (args.size() != 2)
		usage();
	if (args[1] == "--quality")
		quality();
	if (args[1] == "--spread")
		surf();
	if (args[1] == "--speed")
		speed();
	if (args[1] == "--test")
		test();
	
	return 0;
}

