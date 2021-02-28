#include "util.hh"

#include <sstream>
#include <iomanip>

using namespace std;
using namespace chrono;


void RNG::seed() {
    this->generator.seed(random_device()());
}

int RNG::random(int min, int max) {
	uniform_int_distribution<mt19937::result_type> dist(min, max);
	return dist(this->generator);
}

vector<string> parse_args(int argc, char **argv) {
	vector<string> args;
	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);
	return args;
}

string repr(string &x) { return "'" + x + "'"; };

string repr(double &x) { return to_string(x); };

string repr(int &x) { return to_string(x); };

string repr(Jobs &obj) {
    stringstream str;
    for (size_t job = 0; job < obj.J; job++) {
        for (size_t step = 0; step < obj.M; step++) {
            str << setw(2) << obj.mach[job][step] << " ";
            str << setw(2) << obj.time[job][step];
            if (step < obj.M - 1) str << " ";
        }
        if (job < obj.J - 1) str << "\n";
    }
    return str.str();
}

string repr(Ans &obj) {
    stringstream str;
    str << obj.span << "\n";
    for (size_t job = 0; job < obj.J; job++) {
        for (size_t step = 0; step < obj.M; step++) {
            str << obj.time[job][step];
            if (step < obj.M - 1) str << " ";
        }
        if (job < obj.J - 1) str << "\n";
    }
    return str.str();
}

string digit2letter(int i) {
    if (i >= 10 && i <= 10 + 'Z' - 'A')
        return to_string((char) i - 10 + 'A');
    return to_string(i);
}

string visual(Jobs &jobs, Ans &ans) {
    stringstream str;
    vector<int> phase(jobs.J);
    for (int m = jobs.M - 1; m >= 0; m--) {
        str << setw(2) << m << "| ";
        string job = "";
        int mark = 0;
        for (int s = 0; s < ans.span; s++) {
            if (mark == 0) {
                for (int j = 0; j < jobs.J; j++) {
                    for (int p = 0; p < jobs.M; p++) {
                        if (m == jobs.mach[j][p] && s == ans.time[j][p]) {
                            job = digit2letter(j);
                            mark = jobs.time[j][p];
                        }
                    }
                }
            }
            if (mark > 0) {
                str << job;
                mark--;
            } else {
                str << " ";
            }
            str << " ";
        }
        if (m > 0) str << "\n";
    }
    return str.str();
}

