import numpy as np
import torch as T
import pickle as pkl

from math import ceil
from time import time
from datetime import datetime

cuda = T.cuda.is_available()

device = T.device('cuda:0' if cuda else 'cpu')

def unzip(xs):
    return [list(x) for x in zip(*xs)]

def tick():
    return time()

def tock(t):
    return time() - t

def timestamp():
    return datetime.now().strftime('%Y%m%dT%H%M%S')

def pad_int(n, maximum):
    return str(n).rjust(len(str(maximum)), ' ')

def prog(count, total):
    return pad_int(count, total) + '/' + str(total)

def param_count(module):
    # return total number of parameters (individual floating-point numbers) in nn.Module
    return sum(T.numel(param) for param in module.parameters())

def freeze(module):
    for param in module.parameters(): param.requires_grad = False

def unfreeze(module):
    for param in module.parameters(): param.requires_grad = True

def minibatches_iter(permutation, batch_size):
    batch_count = int(ceil(len(permutation) / batch_size))
    for batch_idx in range(batch_count):
        yield permutation[batch_idx*batch_size:][:batch_size]

def minibatches(permutation, batch_size):
    return list(minibatches_iter(permutation, batch_size))

def pickle(obj, *, to):
    path = to
    assert isinstance(path, str)
    with open(path, 'wb') as f:
        pkl.dump(obj, f)

def unpickle(path):
    with open(path, 'rb') as f:
        return pkl.load(f)

def random_choice(choices, *, exclude):
    result = exclude
    while result == exclude:
        result = np.random.choice(choices)
    return result
