#include "greedy.hh"
#include "util.hh"

#include <iostream>

using namespace std;

void State::init() {
    int J = jobs.J, M = jobs.M;
    phase_count = M;
    jobs_count = J;
    mach_count = M;
    rem_prog.resize(J);
    rem_proc.resize(J);
    rem_busy.resize(M);
    phase.resize(J);

    // compute program time for each job
    for (int j = 0; j < jobs_count; j++)
        for (int p = 0; p < phase_count; p++)
            rem_prog[j] += jobs.time[j][p];
}

void State::assign(int job, Ans &ans) {
    int p = phase[job];
    int m = jobs.mach[job][p];
    int t = jobs.time[job][p];
    rem_busy[m] += t;
    rem_prog[job] -= t;
    rem_proc[job] += t;
    ans.time[job][p] = step;
    phase[job] += 1;
}

#include <limits.h>
void State::advance() {
	int min = INT_MAX;
	for (int i = 0; i < mach_count; i++)
		if (rem_busy[i] < min && rem_busy[i] > 0) min = rem_busy[i];
	int advance = min == 0 ? 1 : min;

    for (int i = 0; i < jobs_count; i++) {
        // decrement job's remaining program steps
        if (rem_proc[i] > 0) rem_proc[i] -= advance;
		if (rem_proc[i] < 0) rem_proc[i] = 0;

        // mark job with no remaining steps as done
        if (rem_proc[i] <= 0 && phase[i] == phase_count) {
            phase[i] += 1;
            done += 1;
        }
    }
    // decrement machine remaining busy times
    for (int i = 0; i < mach_count; i++) {
        if (rem_busy[i] > 0) {
			rem_busy[i] -= advance;
			if (rem_busy[i] < 0) rem_busy[i] = 0;
		}
    }
    step += advance;
}

void solve_greedy(Jobs &jobs, Ans &ans) {
    State s(jobs);
    while (!s.is_finished()) {
        while (true) {
            int best = -1;
            for (int i = 0; i < s.jobs_count; i++) {
                if (s.is_assignable(i))
                    if (best == -1 || s.rem_proc[i] < s.rem_proc[best])
                        best = i;
            }
            if (best == -1) break;
            s.assign(best, ans);
        }
        s.advance();
    }
    ans.span = s.step;
}

