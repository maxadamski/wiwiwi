#include "mcts.hh"
#include "util.hh"

#include <iostream>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

MoonFunction::MoonFunction(int len) {
    for (int i = 0; i < len; i++)
        func.push_back((float) 0);

    propagation_param = log(propagation_param);


    long seed = system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(-1.0, 1.0);

    for (auto &val: func)
//        val = distribution(generator);
        val = 0.0;
}


double MoonFunction::calculate(int x, int index) {
    return (double) propagation_param * (x - index - sqrt(2.0 / (-1 * propagation_param))) *
           (x - index + sqrt(2.0 / (-1 * propagation_param)));
}


void MoonFunction::update_values(int index, bool better_span) {

    double decr_param = 0.95;
    double bad_param = 0.3;
    for (int j = 0; j < func.size(); ++j) {
        func[j] *= decr_param;
    }

    int formula = (int) floor(sqrt(2.0 / (-1 * propagation_param)));
    int start_i = (index - formula > 0) ? index - formula : 0;
    int end_i = (index + formula < func.size()) ? index + formula : (int) func.size() - 1;

    for (int i = start_i; i <= end_i; i++) {
        double s = (calculate(i, index) + func[i]);
        double val = (calculate(i, index) + func[i]) / (double) 2;
        if (!better_span)
            val = val * bad_param;
        func[i] = val;
    }
}

bool compare_machines(Tuple &x, Tuple &y) {
    return x.a < y.a;
}

int find_machine_spaces(vector<vector<Tuple>> &machine_times, Task *task, int min_start) {
    int next_free_space_start = 0;

    sort(machine_times[task->mach].begin(), machine_times[task->mach].end(), compare_machines);

    if (machine_times.empty())
        return 0;

    for (int i = 0; i < machine_times[task->mach].size(); ++i) {
        if ((abs(next_free_space_start - machine_times[task->mach][i].a) >= task->time) and
            (min_start + task->time <= machine_times[task->mach][i].a)) {
            return max(next_free_space_start, min_start);
        } else {
            next_free_space_start = machine_times[task->mach][i].b;
        }
    }
    return max(next_free_space_start, min_start);
}

void RunAns::calc_makespan(vector<Task *> tasks) {
    vector<vector<Tuple>> machines_times; //When machine[i] will be free
    vector<int> jobs_end_times; // When job[i] will be able to start next task

    machines_times.resize((unsigned long) (num_of_tasks_for_job));

    for (int i = 0; i < num_of_jobs; ++i)
        jobs_end_times.push_back(0);

    for (auto task: tasks) {
        int t_min = jobs_end_times[task->job];
        int t = find_machine_spaces(machines_times, task, t_min);

        answer.time[task->job][task->ord] = t;
        machines_times[task->mach].push_back(Tuple{t, t + task->time});
        jobs_end_times[task->job] = t + task->time;
    }

    for (int end_time: jobs_end_times)
        if (end_time > answer.span)
            answer.span = end_time;
}

void mix(vector<Task *> &tasks, uniform_real_distribution<double> &distibution, default_random_engine &generator) {
    for (int i = 0; i < tasks.size() - 1; i++)
        if (distibution(generator) < tasks[i]->func.dropout)
            swap(tasks[i], tasks[i + 1]);
}

void init_sort(vector<Task *> &poss_choices, vector<Job> &tasked_jobs) {
    int best_index = 0, best_val = INT32_MAX;
    for (int i = 0; i < poss_choices.size(); i++) {
        int left_for_job = 0;
        int job = poss_choices[i]->job;

        for (int j = tasked_jobs[job].next_task; j < tasked_jobs[job].tasks.size(); ++j)
            left_for_job += tasked_jobs[job].tasks[j].time;

        if (left_for_job < best_val) {
            best_index = i;
            best_val = left_for_job;
        }
    }

    swap(poss_choices[0], poss_choices[best_index]);
}


RunAns run(vector<Job> &tasked_jobs, bool init) {
    int finished = 0;
    vector<Task *> alignment;

    long seed = system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int iter = 0; finished != tasked_jobs.size();) {
        vector<Task *> poss_choices;
        Comperator comperator(iter);

        for (Job &job : tasked_jobs)
            if (!job.finished)
                poss_choices.push_back(&job.tasks[job.next_task]);

        if (!init)
            sort(poss_choices.begin(), poss_choices.end(), comperator);
        else init_sort(poss_choices, tasked_jobs);
//        mix(poss_choices, distribution, generator);

        bool selected = false;

        for (Task *task: poss_choices) {
            if (distribution(generator) < task->func.dropout and !init)
                continue;

            alignment.push_back(task);
            selected = true;
            int job = task->job;
            tasked_jobs[job].next_task += 1;
            iter++;
            if (tasked_jobs[job].next_task == tasked_jobs[job].tasks.size()) {
                tasked_jobs[job].finished = true;
                finished += 1;
            }
            break;
        }
        if (!selected) { alignment.push_back(poss_choices[0]); }
    }

    return RunAns((int) tasked_jobs.size(), (int) tasked_jobs[0].tasks.size(), alignment);
}

void RunAns::back_propagate(bool better) {
    for (int i = 0; i < alignment.size(); i++) {
        alignment[i]->func.update_values(i, better);
    }
}

void solve_mcts(Jobs &jobs, Ans &answer) {

    auto tasked_jobs = jobs_as_tasks(jobs);
    int best_span = INT32_MAX;
    Ans best_ans(tasked_jobs.size(), tasked_jobs[0].tasks.size());

    auto p0 = time_now();

    for (int i = 0; i < 10000; ++i) {
        auto p1 = time_now();
        if (time_diff_ms(p0, p1) >= 1000 * 60 * 4)
            break;
        RunAns run_ans = run(tasked_jobs, i == 0);

        bool better = (run_ans.answer.span <= best_span);
        if (better) {
            best_span = run_ans.answer.span;
            best_ans = run_ans.answer;
//            cout << "Better span, i:" << i << " span: " << run_ans.answer.span << endl;
        }

        //propagation and clean-up
        run_ans.back_propagate(better);
        for (auto &job: tasked_jobs) {
            job.finished = false;
            job.next_task = 0;
        }
    }
    answer = best_ans;

}

vector<Job> jobs_as_tasks(Jobs &jobs) {
    vector<Job> tasked_jobs;

    for (int i = 0; i < jobs.J; ++i) {
        vector<Task> tasked_job;
        for (int j = 0; j < jobs.M; ++j)
            tasked_job.push_back(
                    Task(i, j, jobs.time[i][j], jobs.mach[i][j], jobs.J)); //int job, ord, time, mach, len;

        tasked_jobs.push_back(Job(tasked_job));
    }

    return tasked_jobs;

}