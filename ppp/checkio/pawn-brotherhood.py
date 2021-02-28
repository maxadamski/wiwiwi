from itertools import permutations

def issafe(a, b):
    ax, ay = a.encode()
    bx, by = b.encode()
    return abs(ax - bx) == ay - by == 1

def safe_pawns(pawns):
    return len({a for a, b in permutations(pawns, 2) if issafe(a, b)})