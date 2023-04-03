#include "util.hh"

#include <sstream>
#include <fstream>

using namespace std;

bool readFile(const string &path, string &data) {
	ifstream file(path, ios::in);
	if (!file.is_open()) {
		fprintf(stderr, "error: couldn't open file '%s'\n", path.c_str());
		return false;
	}
	stringstream stream;
	stream << file.rdbuf();
	file.close();
	data = stream.str();
	return true;
}

bool fileExists(const string &name) {
	std::ifstream f(name.c_str());
	return f.good();
}
