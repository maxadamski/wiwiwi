#include "jobshop.hh"

#include "util.hh"
#include "greedy.hh"
#include "ants.hh"
#include "mcts.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <optional>

using namespace std;

enum Format {
    BEASLEY, TAILARD
};

enum Method {
    GREEDY, ACO, MCTS
};

void parse_beasley(ifstream &input, Jobs &jobs) {
    int J, M;
    input >> J >> M;
    jobs.J = J;
    jobs.M = M;
    jobs.time.resize(J);
    jobs.mach.resize(J);
    for (int job = 0; job < J; job++) {
        jobs.time[job].resize(M);
        jobs.mach[job].resize(M);
        for (int phase = 0; phase < M; phase++) {
            input >> jobs.mach[job][phase];
            input >> jobs.time[job][phase];
        }
    }
}

void parse_tailard(ifstream &input, Jobs &jobs) {
    string line;
    getline(input, line);
    istringstream in;
    in = istringstream(line);

    int J, M, _;
    in >> J >> M >> _ >> _ >> _ >> _;

    jobs.J = J;
    jobs.M = M;
    jobs.time.resize(J);
    jobs.mach.resize(J);

    getline(input, line); // read and discard 'Times'
    for (int job = 0; job < J; job++) {
        getline(input, line);
        in = istringstream(line);
        jobs.time[job].resize(M);
        for (int step = 0; step < M; step++)
            in >> jobs.time[job][step];
    }

    getline(input, line); // read and discard 'Machines'
    for (int job = 0; job < J; job++) {
        getline(input, line);
        in = istringstream(line);
        jobs.mach[job].resize(M);
        for (int step = 0; step < M; step++) {
            in >> jobs.mach[job][step];
            jobs.mach[job][step] -= 1;
        }
    }
}

void print_beasley(Jobs &jobs) {
    cout << jobs.J << " " << jobs.M << "\n";
    cout << repr(jobs);
    cout << "\n";
}

void usage(string message = "") {
    if (message != "") cerr << message;
    cerr << "usage: jobshop [-h] [-d] [-b] [-g] [-c|-m METHOD] -t TYPE -f FILE\n";
    exit(1);
}

int main(int argc, char **argv) {
    vector<string> args = parse_args(argc, argv);
    optional <Format> format;
    optional <Method> method;
    string fpath;
    int job_limit = 0;
    bool to_beasley = false;
    bool benchmark = false;
    bool debug = false;
    bool visualize = false;
    bool graph_display = false;

    for (int i = 1; i < argc; i++) {
        string arg = args[i];
        if (arg == "-h" || arg == "--help") {
            usage();
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "-v" || arg == "--visual") {
            visualize = true;
        } else if (arg == "-c" || arg == "--convert") {
            to_beasley = true;
        } else if (arg == "-b" || arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "-g" || arg == "--graph") {
            graph_display = true;
        } else if (arg == "-j" || arg == "--job-limit") {
            if (++i >= argc) usage();
            string val = args[i];
            job_limit = stoi(val);
        } else if (arg == "-f" || arg == "--file") {
            if (++i >= argc) usage();
            fpath = args[i];
        } else if (arg == "-m" || arg == "--method") {
            if (++i >= argc) usage();
            string val = args[i];
            if (val == "greedy")
                method = GREEDY;
            else if (val == "aco")
                method = ACO;
            else if (val == "mcts")
                method = MCTS;
            else
                usage("bad method\nMETHOD := greedy | aco | mcts\n");
        } else if (arg == "-t" || arg == "--type") {
            if (++i >= argc) usage();
            string val = args[i];
            if (val == "tailard")
                format = TAILARD;
            else if (val == "beasley")
                format = BEASLEY;
            else
                usage("bad type\nTYPE := tailard | beasley\n");
        } else {
            usage();
        }
    }

    if (fpath == "") usage("input FILE must be specified\n");
    ifstream input(fpath.c_str());
    if (!input.good()) {
        cerr << "file '" << fpath << "' doesn't exist\n";
        return 1;
    }

    Jobs jobs;
    if (!format) usage("type TYPE must be specified\n");
    if (debug) cerr << "-- reading problem instance\n";
    switch (*format) {
        case BEASLEY:
            parse_beasley(input, jobs);
            break;
        case TAILARD:
            parse_tailard(input, jobs);
            break;
    }

    if (job_limit > 0) {
        jobs.J = job_limit;
    }


    if (debug) {
        cerr << "-- read problem instance\n";
        if (job_limit > 0)
            cerr << "limited jobs to " << job_limit << "\n";
        cerr << repr(jobs) << "\n\n";
    }

    if (to_beasley) {
        print_beasley(jobs);
        return 1;
    }

    if (!method) usage("method METHOD must be specified\n");
    if (debug) cerr << "-- start solving\n";
    Ans ans(jobs.J, jobs.M);
    auto t0 = time_now();
    switch (*method) {
        case GREEDY:
            solve_greedy(jobs, ans);
            break;
        case MCTS:
            solve_mcts(jobs, ans);
            break;
        case ACO:
            solve_ants(jobs, ans);
            break;
    }
    auto t1 = time_now();

    if (debug) {
        cout << "-- stop solving\n";
        cerr << "took " << time_diff(t0, t1) << "ns\n\n";
        cerr << "-- problem solution\n";
        cerr << repr(ans) << "\n\n";
        if (visualize) {
            cerr << "-- debug schedule\n";
            cerr << visual(jobs, ans) << "\n\n";
        }
//		//For graph testing
//		if (graph_display) {
//			t0 = time_now();
//			Graph graph = Graph(jobs);
//			t1 = time_now();
//			cerr << "constructing graph took: " << time_diff(t0, t1) << endl;
//			cerr << "-- printing graph" << endl << endl;
//			graph.print_graph();
//		}
    }

    if (benchmark) {
        cout << time_diff(t0, t1) << "\n";
    }

    // print the solution to stdout
    cout << repr(ans) << "\n";


    return 0;
}

