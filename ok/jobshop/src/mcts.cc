#include "mcts.hh"

#include <iostream>
#include <map>

using namespace std;


void print_vec(vector<int> &vec) {
    for (auto &x: vec)
        cout << x << " ";
    cout << endl;
}

void recurr_find_all_spans(Graph &graph, vector<vector<int>> &spans, int task, vector<int> curr_span) {

    if (graph.tasks[task].job == -2) {
        spans.push_back(curr_span);
        return;
    }

    for (auto &x: graph.tasks[task].next) {
        curr_span.push_back(x);

        if (task == 6)
            print_vec(curr_span);

        recurr_find_all_spans(graph, spans, x, curr_span);
    }
}


void solve_mcts(Jobs &jobs, Ans &ans) {
//    cerr << "not implemented!\n";

    Graph graph = Graph(jobs);

    //Find longest span

    Task starting_task = graph.tasks[0];
    map<int, vector<int>> longest_make_spans;

    for (auto &task: starting_task.next) {

        vector<Task> longest_make_span;
        vector<vector<int>> spans;
        vector<int> empty_span;

        recurr_find_all_spans(graph, spans, task, empty_span);

        int max = 0;
        for (auto &span: spans)
            if (span.size() > max) {
                max = (int) span.size();
                longest_make_spans[task] = span;
            }
    }

    print_longest_spans(longest_make_spans);

    exit(1);
}

void print_longest_spans(std::map<int, std::vector<int>> &longest_spans) {

    for (auto const&[index, vec] : longest_spans) {
        cout << index << ": ";
        for (auto task: vec)
            cout << task << " ";
        cout << endl;
    }

}
