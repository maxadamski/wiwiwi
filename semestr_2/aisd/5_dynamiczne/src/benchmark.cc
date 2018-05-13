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

using namespace std;

typedef array< int, 2 > Item;
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

int main(int argc, char **argv) {
	random_seed();

	cout << "testing...\n";
	int b = 10, y = 12;
	vector<int> v = {5, 3, 2, 4, 3};
	vector<int> w = {3, 4, 2, 6, 1};
	vector<int> x = solve(v, w, b, y);
	print(x);

	//cout << "n";
	//for (auto c : cs)
	//	for (auto k : ks)
	//		cout << "," << c << "-" << k;
	//cout << "\n";
	//for (int n = n_min; n <= n_max; n += n_step) {
	//	cout << n;
	//	for (int k : ks) {
	//		AdjacencyMatrix g("input/" + to_string(n) + "_" + to_string(k));
	//		AdjacencyMatrix g_copy = g;
	//		cout << "," << benchmark_simple([&g_copy]{
	//			g_copy.eulerian_cycle();
	//		}) * 1e-9;
	//		cerr << ".";
	//		cout << "," << benchmark_simple([&g]{
	//			g.hamiltonian_cycle();
	//		}) * 1e-9;
	//		cerr << ".";
	//	}
	//	cout << "\n";
	//	cerr << "\n";
	//}
	//cerr << "\n";
	//return 0;
}

