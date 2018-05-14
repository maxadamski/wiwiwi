#include "testkit.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <vector>
#include <queue>

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

	Bag(int b, int y): b(b), y(y) {};

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

int main() {
	random_seed();

	cout << "n,b,ga,bf,dp\n";

	{
		Items items = { {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };
		Bag bag(10, 12);

		cout << items.size() << "," << bag.b << ",";

		cout << benchmark(1, [bag, &items]{
			solve_brute(bag, items);
		}) * 1e-9;

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

	cerr << "\n";
	return 0;
}

