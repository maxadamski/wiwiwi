#pragma once

#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <functional>

#define RED "\033[41m"
#define GRN "\033[42m"
#define YLW "\033[43m"
#define NLC "\033[0m\n"

using namespace std;

typedef size_t usize;
typedef int i32;
typedef double f64;

auto rand_dev = random_device {};
auto rand_eng = default_random_engine { rand_dev() };

template <typename T>
void append(vector<T> &xs, T x) {
	xs.push_back(x);
}

vector<i32> range(i32 start, i32 end) {
    vector<i32> res;
    for (i32 i = start; i < end; i++)
        res.push_back(i);
    return res;
}

template <typename T>
void shuffle(vector<T> &xs) {
    shuffle(xs.begin(), xs.end(), rand_eng);
}

template <typename T>
string to_string(vector<T> xs) {
    usize N = xs.size();
    string res = "";
    for (usize i = 0; i < N; i++) {
        res += to_string(xs[i]);
        if (i < N - 1) res += ", ";
    }
    return "[" + res + "]";
}
