import colosseum as co
import numpy as np
import json
from time import time
from sys import argv
import flexbuffers as fb

iters = int(1e6)
iters_json = int(1e3)
board = np.zeros((248, 6, 6))
simple = [42, 1024, 42, 1024, 3.1415, 3.1415]
simple_input = [(42, co.u8), (1024, co.u32), (42, co.i8), (1024, co.i32), (3.1415, co.f32), (3.1415, co.f64)]
simple_tbuf = co.save(0, *simple_input)
simple_fbuf = fb.Dumps(simple)
board_list = board.tolist()
board_json = json.dumps(board_list)
board_tbuf = co.save(0, board)
board_fbuf = fb.Dumps(board)
simple_json = json.dumps(simple)

print('start')

total = 0
for _ in range(iters_json):
    t = time()
    x = fb.Dumps(simple)
    total += time() - t
print(f'fbuf encode simple {total * iters/iters_json:.6f}')

total = 0
for _ in range(iters_json):
    t = time()
    x = fb.Loads(simple_fbuf)
    total += time() - t
print(f'fbuf decode simple {total * iters/iters_json:.6f}')

total = 0
for _ in range(iters_json):
    t = time()
    x = fb.Dumps(board_list)
    total += time() - t
print(f'fbuf encode matrix {total * iters/iters_json:.6f}')

total = 0
for _ in range(iters_json):
    t = time()
    x = fb.Loads(board_fbuf)
    total += time() - t
print(f'fbuf decode matrix {total * iters/iters_json:.6f}')

total = 0
for _ in range(iters):
    t = time()
    x = co.save(0, *simple_input)
    total += time() - t
print(f'tbuf encode simple {total:.4f}')

total = 0
for _ in range(iters):
    t = time()
    x = co.load(simple_tbuf)
    total += time() - t
print(f'tbuf decode simple {total:.4f}')

total = 0
for _ in range(iters):
    t = time()
    x = co.save(0, board)
    total += time() - t
print(f'tbuf encode matrix {total:.4f}')

total = 0
for _ in range(iters):
    t = time()
    x = co.load(board_tbuf)
    total += time() - t
print(f'tbuf encode matrix {total:.4f}')

total = 0
for i in range(iters_json):
    t = time()
    x = json.dumps(simple)
    total += time() - t
print(f'json encode simple {total * iters/iters_json:.6f}')

total = 0
for i in range(iters_json):
    t = time()
    x = json.loads(simple_json)
    total += time() - t
print(f'json decode simple {total * iters/iters_json:.6f}')

iters_json = int(1e4)
total = 0
for i in range(iters_json):
    t = time()
    x = json.dumps(board_list)
    total += time() - t
print(f'json encode matrix {total * iters/iters_json:.6f}')

total = 0
for i in range(iters_json):
    t = time()
    x = json.loads(board_json)
    total += time() - t
print(f'json decode matrix {total * iters/iters_json:.6f}')
