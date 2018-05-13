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

int max(int a, int b) {
	return a > b ? a : b;
}

int abs(int a) {
	return a < 0 ? -a : a;
}


///////////////////////////////////////////////////////////////////////////////
// Macierz Sąsiedztwa == Adjacency Matrix
///////////////////////////////////////////////////////////////////////////////

struct AdjacencyMatrix {
	vector< vector<bool> > data;

	AdjacencyMatrix(string file_path) {
		ifstream input(file_path);
		for (string line; getline(input, line);) {
			istringstream line_input(line);
			vector<bool> line_data;
			for (bool byte; line_input >> byte;)
				line_data.push_back(byte);
			data.push_back(line_data);
		}
	}

	bool is_edge(int u, int v) {
		return data[u][v] == true;
	}

	// procedura mutuje macierz!
	void eulerian_cycle_dfs(int v, list<int> &cycle) {
		for (size_t u = 0; u < data.size(); u++) {
			while (data[v][u]) {
				data[v][u] = data[u][v] = false;
				eulerian_cycle_dfs(u, cycle);
			}
			cycle.push_front(v);
		}
	}

	list<int> eulerian_cycle() {
		list<int> cycle;
		eulerian_cycle_dfs(0, cycle);
		return cycle;
	}

	void hamiltonian_cycle_dfs(int v, vector<int> &visited, list<int> &cycle, list< list<int> > &cycles) {
		cycle.push_front(v);
		if (cycle.size() < data.size()) {
			visited[v] = true;
			for (int u = 0; u < data.size(); u++) {
				if (data[v][u] && !visited[u]) {
					hamiltonian_cycle_dfs(u, visited, cycle, cycles);
				}
			}
			visited[v] = false;
		} else {
			bool is_cycle = false;
			for (int u = 0; u < data.size(); u++) {
				if (data[v][u] && u == 0) {
					is_cycle = true;
					break;
				}
			}

			if (is_cycle) {
				cycles.push_back(cycle);
				return;
			}
		}
		cycle.pop_front();
	}

	list< list<int> > hamiltonian_cycle() {
		vector<int> visited(data.size());
		list< list<int> > cycles;
		list<int> cycle;
		hamiltonian_cycle_dfs(0, visited, cycle, cycles);
		return cycles;
	}
};

//Witold Lipski - Kombinatoryka dla Programistów
//hamilton: stop po 300s (liczone do średniej)

//problem plecakowy pomyłka ~50% dla heurystyki wartość/waga
// greedy nie wyjmujemy z plecaka
// ilość rozwiązań 2^n
// pomiary TYLKO jakości, a nie czasu

//void random_graph(V) {
//	Va = V, Vb = {}
//	connect random in Va to random in Va
//		move them to Vb
//	while Va
//		connect random in Va to random in Vb
//			move it to Vb
//}

//void eulerize(V, E) {
// for vertex (u) of odd degree:
//   add edge to another random vectex of odd degree (v)
//   remove edge from another random vertex (w, w!=v)
//}

///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {
	random_seed();

	int n_min = 100, n_max = 1500, n_step = 100;
	int ks[] = {10,20,30,40,60,80,95};
	string cs[] = {"ce", "ch"};

	cout << "n";
	for (auto c : cs)
		for (auto k : ks)
			cout << "," << c << "-" << k;
	cout << "\n";

	for (int n = n_min; n <= n_max; n += n_step) {
		cout << n;

		for (int k : ks) {
			AdjacencyMatrix g("input/" + to_string(n) + "_" + to_string(k));
			AdjacencyMatrix g_copy = g;

			cout << "," << benchmark_simple([&g_copy]{
				g_copy.eulerian_cycle();
			}) * 1e-9;
			cerr << ".";

			cout << "," << benchmark_simple([&g]{
				g.hamiltonian_cycle();
			}) * 1e-9;
			cerr << ".";
		}

		cout << "\n";
		cerr << "\n";
	}

	cerr << "\n";
	return 0;
}

