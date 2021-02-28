#include "testkit.hh"

using namespace std;

// """NOTATKI"""
//
//void bfs(G *g) {
//	// O(V + E)
//}
//void dfs(G *g) {
//	// O(V + E)
//}
// ilość krawędzi w grafie skierowanym
// V^2
// ilość krawędzi w nieskierowanym
// (V po 2) = V*(V-1)/2
//
// Wierzchołki numerujemy od 0!


///////////////////////////////////////////////////////////////////////////////
// Macierz Sąsiedztwa == Adjacency Matrix
///////////////////////////////////////////////////////////////////////////////

struct AdjacencyMatrix {
	// Op(V^2)
	//Array< Array<int> > data;
	vector< vector<bool> > data;

	AdjacencyMatrix(string file_path) {
		ifstream input(file_path);
		if (!input.good()) {
			cerr << "[error] (bad file) " << file_path << "\n";
			exit(1);
		}
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

	void print() {
		cout << "AdjacencyMatrix[\n";
		for (auto row : data) {
			cout << "  ";
			for (int col : row) cout << col << " ";
			cout << "\n";
		}
		cout << "]\n";
	}
};


///////////////////////////////////////////////////////////////////////////////
// Macierz Incydencji == Incidence Matrix
///////////////////////////////////////////////////////////////////////////////

struct IncidenceMatrix {
	// Op(V+E) ~= Op(V^3)
	// Ot
	//Array< Array<int> > data;
	vector < vector<char> > data;

	IncidenceMatrix(AdjacencyMatrix &adj) {
		//int X = adj.data[0].size();
		int Y = adj.data.size();
		int edge = 0;
		for (int y = 0; y < Y; y++) {
			for (int x = 0; x <= y; x++) {
				if (adj.data[y][x]) {
					data.push_back(vector<char>(Y, 0));
					data[edge][x] = data[edge][y] = 1;
					edge++;
				}
			}
		}
	}

	bool is_edge(int u, int v) {
		for (size_t e = 0; e < data.size(); e++)
			if (data[e][u] == 1 && data[e][v] == 1)
				return true;
		return false;
	}

	void print() {
		cout << "IncidenceMatrix[\n";
		for (auto row : data) {
			cout << "  ";
			for (int col : row) cout << col << " ";
			cout << "\n";
		}
		cout << "]\n";
	}
};


///////////////////////////////////////////////////////////////////////////////
// Lista Krawędzi == Edge List
///////////////////////////////////////////////////////////////////////////////

struct EdgeList {
	// Op(E)
	// Ot(E)
	//	(1, 2)
	//	(2, 3)
	//	(3, 1)
	//	(4, 2)
	//	(5, 2)
	//List< Tuple<int, int> > data;
	vector< array<int, 2> > data;

	EdgeList(string file_path) {
		ifstream input(file_path);
		for (string line; getline(input, line);) {
			istringstream line_input(line);
			array<int, 2> line_data;
			for (int n, i = 0; line_input >> n; i++)
				line_data[i] = n;
			data.push_back(line_data);
		}
	}

	EdgeList(AdjacencyMatrix &adj) {
		for (int y = 0; y < adj.data.size(); y++) {
			for (int x = 0; x <= y; x++) {
				if (adj.data[y][x]) {
					data.push_back({y, x});
				}
			}
		}
	}

	bool is_edge(int u, int v) {
		for (auto edge : data)
			if (edge[0] == u && edge[1] == v)
				return true;
		return false;
	}
};


///////////////////////////////////////////////////////////////////////////////
// Lista Incydencji == Incidence List / Adjacency List
///////////////////////////////////////////////////////////////////////////////

struct AdjacencyList {
	// Op(V+E)
	// czy jest krawędź: Ot(V)
	// czy jest wierzchołek: 
	//	1 -> 2, nil
	//	2 -> 3, nil
	//	3 -> 1, 4, nil
	//	4 -> 2, nil
	//	5 -> nil
	//Array< List<int> > data;
	vector< vector<int> > data;

	AdjacencyList(string file_path) {
		ifstream input(file_path);
		if (!input.good()) {
			cerr << "[error] (bad file) " << file_path << "\n";
			exit(1);
		}
		for (string line; getline(input, line);) {
			istringstream line_input(line);
			vector<int> line_data;
			for (int n; line_input >> n;)
				line_data.push_back(n);
			data.push_back(line_data);
		}
	}

	AdjacencyList(AdjacencyMatrix &adj) {
		for (size_t y = 0; y < adj.data.size(); y++) {
			data.push_back(vector<int>());
			for (size_t x = 0; x <= y; x++) {
				if (adj.data[y][x]) {
					data[y].push_back(x);
				}
			}
		}
	}

	void print() {
		for (size_t u = 0; u < data.size(); u++) {
			cout << u << " -> ";
			for (int v : data[u]) {
				cout << v << " ";
			}
			cout << "\n";
		}
	}

	bool is_edge(int u, int v) {
		for (int x : data[u])
			if (v == x)
				return true;
		return false;
	}

	bool dag = true;

	void visit(int v, vector<int> &marked, list<int> &sorted) {
		if (!dag) return;
		if (marked[v] == 2) return;
		if (marked[v] == 1) {
			cerr << "Not a DAG reeeeeeee!!!\n";
			dag = false;
		}
		marked[v] = 1;
		for (int u : data[v])
			visit(u, marked, sorted);
		marked[v] = 2;
		sorted.push_front(v);
	}

	// algorytm studencki O(V^2)
	// algorytm dfs O(V+E)
	list<int> topsort() {
		// 0 -> unmarked
		// 1 -> temporary mark
		// 2 -> permanent mark
		vector<int> marked(data.size());
		list<int> sorted;
		dag = true;

		bool unmarked = true;
		while (unmarked) {
			unmarked = false;
			for (size_t v = 0; v < data.size(); v++) {
				if (!marked[v]) {
					unmarked = true;
					visit(v, marked, sorted);
				}
			}
		}

		if (!dag) return {};
		return sorted;
	}
};


///////////////////////////////////////////////////////////////////////////////
// BENCHMARK
///////////////////////////////////////////////////////////////////////////////

void tsort() {
	cerr << "sorting topologically...\n";
	cout << "n,topsort\n";

	for (int n = 100; n <= 1500; n += 100) {
		cout << n << ",";

		AdjacencyList adjacency_list("input/" + to_string(n) + "_DAG");
		cerr << ".";

		cout << benchmark_simple([&adjacency_list]{
			adjacency_list.topsort();
		}) * 1e-9 << "\n";
		cerr << ".";
	}

	cerr << "\n";
}

void edges() {
	cerr << "finding connections...\n";
	cout << "n,ms,mi,lk,li\n";

	for (int n = 100; n <= 1500; n += 100) {
		cout << n << ",";

		AdjacencyMatrix adjacency_matrix("input/" + to_string(n) + "_MS");
		IncidenceMatrix incidence_matrix(adjacency_matrix);
		AdjacencyList adjacency_list(adjacency_matrix);
		EdgeList edge_list(adjacency_matrix);
		cerr << ".";

		vector<int> searches;
		for (int i = 0; i < n; i++) {
			int r = i;
			while (r == i)
				r = random(0, n - 1);
			searches.push_back(r);
		}
		cerr << ".";

		// ms
		cout << benchmark_simple([&adjacency_matrix, searches, n]{
			for (int i = 0; i < n; i++)
				adjacency_matrix.is_edge(i, searches[i]);
		}) * 1e-9 << ",";
		cerr << ".";

		// mi
		cout << benchmark_simple([&incidence_matrix, searches, n]{
			for (int i = 0; i < n; i++)
				incidence_matrix.is_edge(i, searches[i]);
		}) * 1e-9 << ",";
		cerr << ".";

		// lk
		cout << benchmark_simple([&edge_list, searches, n]{
			for (int i = 0; i < n; i++)
				edge_list.is_edge(i, searches[i]);
		}) * 1e-9 << ",";
		cerr << ".";

		// li
		cout << benchmark_simple([&adjacency_list, searches, n]{
			for (int i = 0; i < n; i++)
				adjacency_list.is_edge(i, searches[i]);
		}) * 1e-9 << "\n";
		cerr << ".\n";
	}

	cerr << "\n";
}

void test() {
	vector<string> testy({"test1", "test2", "test3"});
	for (string test : testy) {
		cerr << "+++++++++++++++++++\n";
		cerr << test << "\n";
		AdjacencyList m("input/" + test);
		m.print();
		list<int> s = m.topsort();
		for (int i : s)
			cerr << i << " ";
		cerr << "\n";
	}
}

void usage(bool abort = true) {
	cout << "usage: benchmark [--tsort|--edges]\n";
	if (abort) exit(1);
}

int main(int argc, char **argv) {
	auto args = parse_args(argc, argv);
	random_seed();
	if (args.size() != 2)
		usage();
	if (args[1] == "--tsort")
		tsort();
	if (args[1] == "--edges")
		edges();
	if (args[1] == "--test")
		test();
	return 0;
}

