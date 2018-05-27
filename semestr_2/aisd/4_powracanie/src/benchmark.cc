#include "testkit.hh"

using namespace std;

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

	AdjacencyMatrix(int V, int E, bool eulerian = true) {
		vector <int> A, B;

		for (int i = 0; i < V; i++)
			data.push_back(vector<bool>(V));

		for (int i = 0; i < V; i++)
			A.push_back(i);

		// move random from A to B
		{
			int a = random(0, A.size() - 1);
			B.push_back(A[a]);
			A.erase(A.begin() + a);
		}

		// connect random in A to random in B, and move it to B
		while (!A.empty()) {
			int a = random(0, A.size() - 1);
			int b = random(0, B.size() - 1);
			int u = A[a], v = B[b];
			data[u][v] = data[v][u] = true;

			B.push_back(u);
			A.erase(A.begin() + a);
		}

		if (eulerian) {
			for (int i = 0; i < E - V; i += 3) {
				int a, b, c;
				do {
					a = random(0, V-1);
					b = random(0, V-1);
					c = random(0, V-1);
				} while (a == b || a == c || b == c ||
					data[a][b] || data[a][c] || data[b][c]);

				data[a][b] = data[b][a] = true;
				data[a][c] = data[c][a] = true;
				data[b][c] = data[c][b] = true;
			}
		} else {
			for (int i = 0; i < E - V; i++) {
				int u, v;
				do {
					u = random(0, V-1);
					v = random(0, V-1);
				} while (u == v || data[u][v]);

				data[u][v] = data[v][u] = true;
			}
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
	// 5 minute timeout
	int t_max = 5*60*1000;

	bool contains(vector<int> &collection, int element) {
		for (auto test : collection)
			if (test == element)
				return true;
		return false;
	}

	bool hamiltonian_cycle_dfs(int v, vector<int> &visited) {
		// stop computing after time exceeds t_max
		auto t1 = chrono::steady_clock::now();
		auto dt = chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
		if (dt >= t_max) {
			did_timeout = true;
			return false;
		}

		if (contains(visited, v)) {
			// if all vertices visited, and is at the start, report success
			if (visited.size() == data.size() && v == 0)  {
				visited.push_back(v);
				return true;
			}
			// if already visited this vertex
			return false;
		}

		visited.push_back(v);
		for (size_t u = 0; u < data.size(); u++) {
			// if recursion yields cycle, report success
			if (data[v][u] && hamiltonian_cycle_dfs(u, visited)) {
				return true;
			}
		}

		// backtrack
		visited.pop_back();
		return false;
	}

	// NP-complete
	// O(n!)
	vector<int> hamiltonian_cycle() {
		t0 = chrono::steady_clock::now();

		vector<int> visited;
		if (hamiltonian_cycle_dfs(0, visited)) {
			return visited;
		} else {
			// no cycle :(
			return vector<int>();
		}
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

int Phi[] = {10,20,30,40,60,80,95};
int V_min = 200, V_max = 1000, V_step = 200;

void input() {
	for (int V = V_min; V <= V_max; V += V_step) {
		for (auto phi : Phi) {
			auto m = AdjacencyMatrix(V, edges(V, phi / 100.0));
			m.save("input/" + to_string(V) + "_" + to_string(phi)); 
			cerr << ".";
		}
	}
	cerr << "\n";
}

void bench() {
	cerr << "[info] benchmarking\n";
	cout << "v,phi,ec";
	int hc = 25;
	for (int i = 0; i < hc; i++)
		cout << ",hc-" << i+1;
	cout << "\n";

	for (int V = V_min; V <= V_max; V += V_step) {
		for (int phi : Phi) {

			cerr << "phi: " << phi << ", v: " << V;
			cout << V << "," << phi;

			AdjacencyMatrix g(V, edges(V, phi / 100.0));

			cout << "," << benchmark_simple([&g]{
				g.eulerian_cycle();
			}) * 1e-9;
			cerr << ".";

			for (int i = 0; i < hc; i++) {
				AdjacencyMatrix h(V, edges(V, phi / 100.0), false);

				auto elapsed = benchmark_simple([&h]{
					h.hamiltonian_cycle();
				}) * 1e-9;

				cout << "," << elapsed;
				cerr << ".";
			}
			
			cout << "\n";
			cerr << "\n";
		}

		cerr << "\n";
	}
}

void test() {

}

void usage(bool abort = true) {
	cout << "usage: benchmark [--input|--bench|--test]\n";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	random_seed();
	if (argc != 2)
		usage();
	if (!strcmp(argv[1], "--input"))
		input();
	if (!strcmp(argv[1], "--bench"))
		bench();
	if (!strcmp(argv[1], "--test"))
		test();
	return 0;
}

