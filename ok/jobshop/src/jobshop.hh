#pragma once

#include <vector>

struct Jobs {
    // fields:
    //   J: liczba zadań
    //   M: liczba maszyn/faz
    //   zadanie `i` w fazie `j` musi być wykonane na maszynie `mach[i][j]`
    //   zadanie `i` w fazie `j` ma czas wykonania `time[i][j]`
    //
    // assert:
    //   for all i: len(time[i]) == len(mach[i]) == M
    //   len(time) == len(mach) == J

    int J, M;
    std::vector<std::vector<int>> time, mach;
};

struct Task {
    int job, ord, time, mach;
    std::vector<int> next, prev, data;
};

struct Graph {
    std::vector<Task> tasks;
    int last_index;

    explicit Graph(Jobs &jobs);

    void print_graph();
};

struct Ans {
    // fields:
    //   total: czas uszeregowania
    //   zadanie `i` rozpoczęło fazę `j` w kwancie czasu `time[i][j]`
    // assert:
    //   for all i: len(time[i]) == M
    //   len(time) == J

    int J, M, span = 0;
    std::vector<std::vector<int>> time;

    Ans(int J, int M) : J(J), M(M) {
        init();
    }

    inline void init() {
        time.resize(J);
        for (int j = 0; j < J; j++)
            time[j].resize(M);
    }
};

