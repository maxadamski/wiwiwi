#include "testkit.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <functional>
#include <vector>
#include <array>
#include <list>
#include <string>
#include <cstring>
#include <queue>

using namespace std;

struct Item {
	int v, w;
};

typedef vector< Item > Bag;

int max(int a, int b) {
	return a > b ? a : b;
}

int abs(int a) {
	return a < 0 ? -a : a;
}

void print(vector<int> a) {
	cout << "{ ";
	for (auto e : a) cout << e << " ";
	cout << "}\n";
}

int addmul(vector<int> e, vector<int> x) {
	int sum = 0;
	for (size_t i = 0; i < x.size(); i++)
		sum += e[i] * x[i];
	return sum;
}

// maximize addmul(v, x),
//     when addmul(w, x) <= y && 0 <= x_i <= b
vector<int> solve(vector<int> v, vector<int> w, int b, int y) {
	vector<int> x(v.size());
	return x;
}


///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////

// 1. Greedy algorithm (GA)
// "nie rozwiązuje"
// złożoność: O(n log n)
// priority queue Q by v / w
// greedily take from Q
//
// 2. Brute force (BF)
// "rozwiązuje"
// złożoność: O(2^n)
// sum_{i=1}^n (n choose i) = 2^n
//
// 3. Dynamic programming (DP)
// "rozwiązuje"
// wykorzystanie procesu Markova
// złożoność: O(n*b)
// i = 0..n
// L = 0..b
// f(0,L) = 0
// f(i,0) = 0
// f(i,L) = f(i-1,L), where s(a_i) < L
// f(i,L) = max(
//	f(i-1, L-s(a_i)) + w(a_i),
//	f(i-1,L)
// ), where L >= S(a_i)
// 
// jakościowo możemy porównywać wszystkie
// możemy porównywać BF i DP czasowo 
// dla problemu optymalizacyjnego plecak jest NP-trudny

// cykl hamiltona
// O(n*m)
//
// random b 

int main(int argc, char **argv) {
	random_seed();

	cout << "testing...\n";
	int b = 10, y = 12;
	vector<int> v = {5, 3, 2, 4, 3};
	vector<int> w = {3, 4, 2, 6, 1};
	vector<int> x = solve(v, w, b, y);
	print(x);

	vector<Item> items = { {2,3}, {3,1}, {1,4}, {1,1}, {8,5} };
	// i\l| 0 1 2 3 4 5 6  7  8  9  10
	// ---+---------------------------
	//  0 | 0 0 0 0 0 0 0  0  0  0  0 
	//  1 | 0 0 0 2 2 2 2  2  2  2  2 
	//  2 | 0 3 3 3 5 5 5  5  5  5  5 
	//  3 | 0 3 3 3 5 5 5  5  6  6  6 
	//  4 | 0 3 4 4 5 6 6  6  6  7  7 
	//  5 | 0 3 4 4 5 8 11 12 12 13 14

	auto cmp = [](Item l, Item r) { return (double) l.v / l.w < (double) r.v / r.w;};
	priority_queue< Item, vector<Item>, decltype(cmp) > q(cmp);
}

