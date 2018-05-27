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

struct Bag {
	int b, y, v = 0, w = 0;

	Bag(int b = 0, int y = 0): b(b), y(y) {};

	void add(Item item) {
		v += item.v;
		w += item.w;
	}
};

// 1. Greedy algorithm (GA)
// "nie rozwiązuje"
// złożoność: O(n log n)
// priority queue Q by v / w
// greedily take from Q

Bag solve_greedy(Bag bag, Items &items) {
	// TODO: replace with a real sorting algorithm
	// create queue
	auto cmp = [](Item l, Item r) { return (double) l.v / l.w < (double) r.v / r.w;};
	priority_queue< Item, vector<Item>, decltype(cmp) > queue(cmp);
	// add items to the priority queue
	for (Item item : items) queue.push(item);
	// greedily add items to the bag
	while (!queue.empty()) {
		Item item = queue.top();
		queue.pop();
		if (bag.w + item.w <= bag.b)
			bag.add(item);
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
int bf_t_max = 60;

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

void p(Matrix matrix, ostream &out = cout) {
	for (auto row : matrix) {
		for (auto item : row) cout << item << " ";
		out << "\n";
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

	bag.v = m[Y-1][X-1];
	bag.w = bag.b;
	return bag;
}

void generate(int n, double b, double phi, Bag &bag, Items &items) {
	int total_w = 0;
	int total_v = 0;

	for (int i = 0; i < n; i++) {
		int w = random(1, b);
		int v = random(1, w);
		items.push_back({w,v});
		total_w += w;
		total_v += v;
	}

	//cerr << "total_v: " << total_v << "\n";
	bag.y = max(1, (int) (phi * total_v));
	bag.b = b;
}

void generate_greedy_worst(int n, double b, double phi, Bag &bag, Items &items) {
	int w = b / (n - 1);
	int v = w / 2;
	int total_v = 0;

	// czyste zło
	items.push_back({w, v + 1});
	total_v += v + 1;

	for (int i = 1; i < n; i++) {
		items.push_back({w,v});
		total_v += v;
	}

	//cerr << "total_v: " << total_v << "\n";
	bag.y = max(1, (int) (phi * total_v));
	bag.b = b;
}

void bench() {
	cout << "n,b,y,phi,brute,greedy,dynamic\n";

	for (int n = 1; n <= 20; n += 1) {
		for (double b = 1; b <= 10; b += 1) {
		for (double phi = 0.1; phi <= 1; phi += 0.1) {
			Items items; Bag bag;
			generate(n, b, phi, bag, items);

			cout << items.size() << "," << bag.b << "," << bag.y << ","
				 << b << "," << phi << ",";

			if (n <= 20) {
				cout << benchmark(1, [bag, &items]{
					solve_brute(bag, items);
				}) * 1e-9;
			} else {
				cout << "-1";
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
		}
		}
		cerr << "\n";
	}

	cerr << "\n";
}

void test() {
	// "Suita testowa":
	//Items items = { {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };
	//Bag bag(10, 12);
}

void usage(bool abort = true) {
	cerr << "usage: benchmark [bench|test]";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	auto args = parse_args(argc, argv);
	random_seed();

	if (args.size() != 2)
		usage();
	if (args[1] == "--bench")
		bench();
	if (args[1] == "--test")
		test();
	
	return 0;
}

