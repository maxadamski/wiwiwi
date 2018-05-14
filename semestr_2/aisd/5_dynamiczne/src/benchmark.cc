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

struct Item {
	int v, w;

	void print(ostream &out) const {
		out << "(v:" << v << ", w:" << w << ")";
	}
};

struct Bag {
	int b, y, v = 0, w = 0;
	vector<Item> items;

	Bag(int b, int y):
		b(b), y(y) {};

	void add(Item item) {
		items.push_back(item);
		w += item.w;
		v += item.v;
	}

	void clear() {
		if (!items.empty()) items.clear();
		w = 0;
		v = 0;
	}

	void print(ostream &out) const {
		out << "{ b: " << b << ", y: " << y << ", v: "
			<< v << ", w: " << w << ", items:\n  [ ";
		for (Item item : items) {
			item.print(out);
			out << " ";
		}
		out << "] }\n";
	}
};

// 1. Greedy algorithm (GA)
// "nie rozwiązuje"
// złożoność: O(n log n)
// priority queue Q by v / w
// greedily take from Q

void solve_greedy(Bag &bag, vector<Item> &items) {
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
		if (bag.w + item.w <= bag.b) {
			bag.add(item);
		}
	}
}

// 2. Brute force (BF)
// "rozwiązuje"
// złożoność: O(2^n)
// sum_{i=1}^n (n choose i) = 2^n

void solve_brute_r(int n, Bag bag, Bag &best, vector<Item> &items) {
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

void solve_brute(Bag &bag, vector<Item> &items) {
	solve_brute_r(items.size(), bag, bag, items);
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

// wygoda ponad wszystko
typedef vector< vector<int> > Matrix;
typedef vector< Item > Items;

void p(Matrix matrix, ostream &out = cout) {
	for (auto row : matrix) {
		for (auto item : row) cout << item << " ";
		out << "\n";
	}
}

int partial(int i, int l, Items &items, Matrix &m) {
	if (i == 0 || l == 0) return 0;

	Item a = items[i - 1];
	if (a.w > l) {
		int y = i - 1, x = l;

		if (m[y][x] == -1)
			m[y][x] = partial(y, x, items, m);

		return m[y][x];
	} else {
		int y1 = i - 1, x1 = l - a.w,
			y2 = i - 1, x2 = l;

		if (m[y1][x1] == -1)
			m[y1][x1] = partial(y1, x1, items, m);

		if (m[y2][x2] == -1)
			m[y2][x2] = partial(y2, x2, items, m);

		return max(m[y1][x1] + a.v, m[y2][x2]);
	}
}

void solve_dynamic(Bag &bag, Items &items) {
	size_t Y = items.size() + 1, X = bag.b + 1;
	Matrix m(Y);
	for (size_t y = 0; y < Y; y++) {
		m[y].resize(X);
		for (size_t x = 0; x < X; x++) m[y][x] = -1;
	}

	for (size_t y = 0; y < Y; y++) {
		for (size_t x = 0; x < X; x++) {
			m[y][x] = partial(y, x, items, m);
		}
	}

	bag.w = bag.b;
	bag.v = m[Y-1][X-1];
}

int main(int argc, char **argv) {
	random_seed();

	//vector<Item> items = { {3,4}, {0,1}, {10,5}, {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };
	vector<Item> items = { {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };

	Bag bag(10, 12);
	cout << "greedy: " << benchmark(1, [&bag, &items]{
		solve_greedy(bag, items);
	}) * 1e-9 << " s\n";
	bag.print(cerr);
	cout << "\n";

	bag.clear();
	cout << "brute: " << benchmark(1, [&bag, &items]{
		solve_brute(bag, items);
	}) * 1e-9 << " s\n";
	bag.print(cerr);
	cout << "\n";

	bag.clear();
	cout << "dynamic: " << benchmark(1, [&bag, &items]{
		solve_dynamic(bag, items);
	}) * 1e-9 << " s\n";
	bag.print(cerr);
}

