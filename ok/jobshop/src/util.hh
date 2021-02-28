#pragma once

#include "jobshop.hh"

#include <string>
#include <vector>
#include <chrono>
#include <random>

#define time_diff time_diff_ns

typedef std::chrono::steady_clock::time_point Time;

/** Random Numbers **/

struct RNG {
public:
    void seed();
    int random(int min, int max);

private:
    std::mt19937 generator;
};

/** Time **/

inline Time time_now() {
    return std::chrono::steady_clock::now();
};

inline long time_diff_ms(Time t0, Time t1) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
};

inline long time_diff_ns(Time t0, Time t1) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
};

/** Command Line Arguments **/

std::vector<std::string> parse_args(int argc, char **argv);

/** Pretty Printing **/

std::string repr(double &x);

std::string repr(int &x);

std::string repr(std::string &x);

std::string repr(Jobs &obj);

std::string repr(Ans &obj);

std::string visual(Jobs &jobs, Ans &obj);

