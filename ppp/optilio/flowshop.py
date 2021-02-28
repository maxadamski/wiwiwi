#!/usr/bin/python3

from itertools import combinations
import sys, time, random

#
# IO
#

def init(filename):
    with open(filename, 'r') as file:
        data = [list(map(int, it.strip().split())) for it in file.readlines()]
        return int(data[0][0]), int(data[0][1]), data[1:]

def read():
    try:
        return list(map(int, sys.stdin.readline().strip().split(' ')))
    except:
        exit(0)

def write(permutation):
    print(' '.join(map(str, permutation)))
    sys.stdout.flush()

#
# Scoring
#

def score_optilio(permutation, data):
    job_count, machine_count, init_data = data
    res = [0] * machine_count
    for i in range(job_count):
        res[0] += init_data[0][permutation[i]]
        for j in range(1, machine_count):
            res[j] = max(res[j], res[j-1]) + init_data[j][permutation[i]]
    return res[-1]

#
# Heuristics
#

def calc_dummy(perm, score):
    return perm

def calc_random(perm, score):
    L, best = len(perm), list(perm)
    passes = 32

    for _ in range(passes):
        for _ in range(L):
            a = random.randrange(L)
            b = random.randrange(L)
            copy = list(best)
            copy[a], copy[b] = copy[b], copy[a]
            if score(copy) < score(best):
                best = copy
                break

    return best

def calc_greedy(perm, score):
    L, best = len(perm), list(perm)
    passes = 2

    for _ in range(passes):
        for a, b in combinations(range(L), 2):
            copy = list(best)
            copy[a], copy[b] = copy[b], copy[a]
            if score(copy) < score(best):
                best = copy

    return best

def calc_nearest(perm, score):
    L, best = len(perm), list(perm)
    passes = 10

    def nearest(p):
        for a, b in combinations(range(L), 2):
            copy = list(p)
            copy[a], copy[b] = copy[b], copy[a]
            if score(copy) < score(p):
                return copy
        return p

    for _ in range(passes):
        best = nearest(best)

    return best

def calc_depth(perm, score):
    L, best = len(perm), list(perm)
    maxlevel = 3

    levels = [[] for _ in range(maxlevel)]
    levels[0].append(best)

    for level in range(maxlevel - 1):
        for node in levels[level]:
            for i in range(2):
                better = calc_random(node, score)
                if better != node:
                    levels[level + 1].append(better)
    # remove empty levels
    levels = [it for it in levels if it]
    return min(levels[-1], key=score)

def calc_rgreed(perm, score):
    L, best = len(perm), list(perm)
    samples = 2
    tests = [calc_random(perm, score) for _ in range(samples)]
    greed = [calc_greedy(it, score) for it in tests]
    return min(greed, key=score)

def calc_fullmin(perm, score):
    globalbest = list(perm)
    passes = 2
    for _ in range(passes):
        localbest = list(globalbest)
        copy = localbest
        for a, b in combinations(range(len(localbest)), 2):
            copy[a], copy[b] = copy[b], copy[a]
            if not score(copy) < score(globalbest):
                copy[a], copy[b] = copy[b], copy[a]
        globalbest = copy
    return globalbest

#
# Main
#

# choose the heuristic
calc = calc_random
# enable debug messages
debug = False

def main(argv):
    data = init(argv[1])
    score = lambda perm: score_optilio(perm, data)
    while True:
        perm = read()
        start_time = time.time()
        solution = calc(perm, score)
        write(solution)
        if debug:
            print(score(perm), "->", score(solution))
            print("finished in: ", time.time() - start_time, "seconds")

if __name__ == "__main__":
    main(sys.argv)

