#pragma once

#include <vector>
#include <map>
#include "jobshop.hh"


void recurr_find_all_spans(Graph &graph, std::vector<std::vector<int>> &spans, int task, std::vector<int> curr_span);

void solve_mcts(Jobs &jobs, Ans &ans);

void print_longest_spans(std::map<int, std::vector<int>> &longest_spans);