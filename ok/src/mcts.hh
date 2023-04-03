#pragma once

#include <vector>
#include <map>
#include <cmath>
#include "jobshop.hh"

struct MoonFunction {
    std::vector<double> func;
    double dropout = 0.05;
    double propagation_param = 0.97;

    explicit MoonFunction(int len);

    void update_values(int index, bool better_span);

    double calculate(int x, int index);

    double operator[](const int index) {
        return func[index];
    }
};

struct Task {
    int job, ord, time, mach;
    MoonFunction func;

    Task(int j, int o, int t, int m, int len) : job(j), ord(o), time(t), mach(m), func(len) {}
};

struct Job {
    int next_task = 0;
    bool finished = false;
    std::vector<Task> tasks;

    explicit Job(std::vector<Task> t) : tasks(t) {}
};

struct RunAns {
    int num_of_tasks_for_job, num_of_jobs;
    std::vector<Task *> alignment;
    Ans answer;

    void back_propagate(bool better);

    void calc_makespan(std::vector<Task *> tasks);

    RunAns(int J, int T, std::vector<Task *> &al) : num_of_jobs(J), num_of_tasks_for_job(T), alignment(al),
                                                    answer(J, T) { calc_makespan(al); }
};

struct Comperator {
    int index;

    Comperator(int i) : index(i) {};

    bool operator()(Task *t1, Task *t2) {
        return t1->func[index] > t2->func[index];
    }
};

struct Tuple {
    int a, b;
};

void solve_mcts(Jobs &jobs, Ans &ans);

std::vector<Job> jobs_as_tasks(Jobs &jobs);
