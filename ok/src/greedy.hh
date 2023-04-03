#pragma once

#include "jobshop.hh"

void solve_greedy(Jobs &jobs, Ans &ans);

struct State {
    // job's total remaining steps
    std::vector<int> rem_prog;
    // job's task remaining steps
    std::vector<int> rem_proc;
    // machine's remaining busy time
    std::vector<int> rem_busy;
    // job's current phase
    std::vector<int> phase;
    // current simulation step
    int step = 0;
    // number of jobs done
    int done = 0;

    Jobs &jobs;
    int phase_count;
    int jobs_count;
    int mach_count;

    State(Jobs &jobs): jobs(jobs) {
        init();
    }

    inline bool is_assignable(int job) {
        return !(is_running(job) || is_done(job) || is_busy(job));
    }

    inline bool is_running(int job) {
        return rem_proc[job] > 0;
    }

    inline bool is_done(int job) {
        return phase[job] >= phase_count;
    }

    // must check if job is not done before checking if is_busy!
    inline bool is_busy(int job) {
        return is_done(job) || rem_busy[jobs.mach[job][phase[job]]] > 0;
    }

    inline bool is_finished() {
        return done >= jobs_count;
    }

    void init();

    void assign(int job, Ans &ans);

    void advance();
};

