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
#include <chrono>

using namespace std;

int max(int a, int b) {
	return a > b ? a : b;
}

int abs(int a) {
	return a < 0 ? -a : a;
}

int edges(int V, double phi, bool undirected = true) {
	double divisor = undirected ? 2 : 1;
	return phi * (V*V - V) / divisor;
}

///////////////////////////////////////////////////////////////////////////////
// Macierz Sąsiedztwa == Adjacency Matrix
///////////////////////////////////////////////////////////////////////////////

struct AdjacencyMatrix {
	vector< vector<bool> > data;

	AdjacencyMatrix(int v) {
		for (int i = 0; i < v; i++)
			data.push_back(vector<bool>(v));
	}

	AdjacencyMatrix(int V, int E)
	{
		for (int i = 0; i < V; i++)
			data.push_back(vector<bool>(V));

		vector <int> nodes;
		vector <int> cycle;
		int tmp;
		int nodeA, nodeB, nodeC;
		for (int i = 0; i < V; i++)
			nodes.push_back(i);
		for (int i = 0; i < V; i++)
		{
			tmp = rand() % nodes.size();
			cycle.push_back(nodes[tmp]);
			nodes.erase(nodes.begin() + tmp);
		}

		for (int i = 0; i < V - 1; i++)
		{
			data[cycle[i]][cycle[i + 1]] = data[cycle[i + 1]][cycle[i]] = true;
		}

		data[cycle[0]][cycle[cycle.size() - 1]] = data[cycle[cycle.size() - 1]][cycle[0]] = true;
		for (int i = 0; i < E - V; i += 3)
		{
			do
			{
				nodeA = rand() % V;
				nodeB = rand() % V;
				nodeC = rand() % V;
			} while (nodeA == nodeB || nodeA == nodeC || nodeB == nodeC ||
					data[nodeA][nodeB] == true ||
					data[nodeA][nodeC] == true ||
					data[nodeB][nodeC] == true);

			data[nodeA][nodeB] = data[nodeB][nodeA] = true;
			data[nodeA][nodeC] = data[nodeC][nodeA] = true;
			data[nodeB][nodeC] = data[nodeC][nodeB] = true;

		}
	}

	//
	// Input & Output
	//

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

	void save(string file_path) {
		ofstream output(file_path);
		for (size_t y = 0; y < data.size(); y++) {
			for (size_t x = 0; x < data.size(); x++) {
				string padding = x == data.size() - 1 ? "" : " ";
				output << data[y][x] << padding;
			}
			output << "\n";
		}
	}

	//
	// Properties
	//

	void is_connected_dfs(int v, vector<int> &visited) {
		visited[v] = 1;
		for (size_t u = 0; u < data.size(); u++)
			if (is_edge(v, u) && !visited[u])
				is_connected_dfs(u, visited);
	}

	bool is_connected() {
		vector<int> visited(data.size());
		is_connected_dfs(0, visited);
		for (int v : visited)
			if (v == 0) return false;
		return true;
	}

	bool is_edge(int u, int v) {
		return data[u][v] == true;
	}

	bool is_eulerian() {
		return odd_vertices().empty();
	}

	int degree(int v) {
		int deg = 0;
		for (size_t u = 0; u < data.size(); u++) 
			deg += data[v][u];
		return deg;
	}

	vector<int> odd_vertices() {
		vector<int> odd;
		for (size_t v = 0; v < data.size(); v++) 
			if (degree(v) % 2 != 0)
				odd.push_back(v);
		return odd;
	}

	vector<int> neighbours(int v) {
		vector<int> us;
		for (size_t u = 0; u < data.size(); u++)
			if (data[v][u]) us.push_back(u);
		return us;
	}

	int edge_count() {
		int count = 0;
		for (size_t y = 0; y < data.size(); y++)
			for (size_t x = 0; x < data.size(); x++)
				count += data[y][x];
		return count;
	}

	double density() {
		double V = data.size();
		return edge_count() / (V*V - V);
	}

	//
	// Euler
	//

	// mutating!
	void eulerian_cycle_dfs(int v, list<int> &cycle) {
		for (size_t u = 0; u < data.size(); u++) {
			if (data[v][u]) {
				data[v][u] = data[u][v] = false;
				eulerian_cycle_dfs(u, cycle);
			}
		}
		cycle.push_front(v);
	}

	list<int> eulerian_cycle() {
		//if (!is_eulerian()) return {};
		list<int> cycle;
		eulerian_cycle_dfs(0, cycle);
		return cycle;
	}

	//
	// Hamilton
	//

	chrono::steady_clock::time_point t0;
	bool did_timeout = false;
	bool did_limit = false;
	int t_max = 600;
	int h_max = 1;

	void hamiltonian_cycle_dfs(int v, vector<int> &visited, list<int> &cycle, vector< list<int> > &cycles) {
		auto t1 = chrono::steady_clock::now();
		auto dt = chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
		if (dt >= t_max) {
			did_timeout = true;
			return;
		}
		if (h_max && cycles.size() >= h_max) {
			return;
		}

		cycle.push_front(v);
		if (cycle.size() < data.size()) {
			visited[v] = true;
			for (size_t u = 0; u < data.size(); u++) {
				if (data[v][u] && !visited[u]) {
					hamiltonian_cycle_dfs(u, visited, cycle, cycles);
				}
			}
			visited[v] = false;
		} else {
			bool is_cycle = false;
			for (size_t u = 0; u < data.size(); u++) {
				if (data[v][u] && u == 0) {
					is_cycle = true;
					break;
				}
			}

			if (is_cycle) {
				cycles.push_back(cycle);
			}
		}
		cycle.pop_front();
	}

	vector< list<int> > hamiltonian_cycle() {
		t0 = chrono::steady_clock::now();
		vector<int> visited(data.size());
		vector< list<int> > cycles;
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

#include <algorithm>

int main() {
	random_seed();

	bool opt_generate = true, opt_benchmark = true;
	int phi_list[] = {10,20,30,40,60,80,95};
	int V = 1000;
//	auto g = AdjacencyMatrix("input/200_30");
//	auto hs = g.hamiltonian_cycle();
//	cout << "n = " << hs.size() << "\n";
//	print(hs[0]);
//	for (auto h : hs) {
//			cout << h.size();
//			cout << "\n";
//	}

	if (opt_generate) {
		cerr << "generating...\n";
		for (auto phi : phi_list) {
			auto m = AdjacencyMatrix(V, edges(V, phi / 100.0));
			m.save("input/" + to_string(V) + "_" + to_string(phi)); 
			cerr << ".";
		}
		cerr << "\n";
	}

	if (opt_benchmark) {
		cerr << "benchmarking...\n";
		cout << "phi,ec";
		int hc = 25;
		for (int i = 0; i < hc; i++)
			cout << ",hc-" << i+1;
		cout << "\n";

		for (int phi : phi_list) {
			cout << phi;

			AdjacencyMatrix g("input/" + to_string(V) + "_" + to_string(phi));
			AdjacencyMatrix g_copy = g;

			cout << "," << benchmark_simple([&g_copy]{
				g_copy.eulerian_cycle();
			}) * 1e-9;
			cerr << ".";

			for (int i = 0; i < hc; i++) {
				if (!g.timeout && !g.did_limit) {
					cout << "," << benchmark_simple([&g]{
						g.hamiltonian_cycle();
					}) * 1e-9;
				} else if (g.did_limit) {
					cout << ",0";
				} else if (g.did_timeout) {
					cout << ",0";
				}
				cerr << ".";
			}
			
			cout << "\n";
			cerr << "\n";
		}

		cerr << "\n";
	}

	cerr << "\n";
	return 0;
}

