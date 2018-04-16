import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

PROCS = ['find', 'append', 'remove']

def struct_cols(struct):
    return [struct+'-'+proc for proc in PROCS]

def scale_find(df):
    # scales *-find to miliseconds
    for col in ['list-find', 'bbst-find', 'bst-find', 'bst-rand-find']:
        df[col] /= df.index
        df[col] *= 10**3

def dspeed(fast, slow):
    n = max(slow.index)
    d = {c: (slow[c][:n] / fast[c][:n]).mean()
         for c in slow}
    for key in PROCS:
        d['bst-rand-'+key] = d['bst-'+key]
    return d


slow = pd.read_csv('data/reference.csv', index_col='n')
fast = pd.read_csv('data/benchmark.csv', index_col='n')
# compute how much slower are put's PCs
delta = dspeed(fast, slow)
# scale faster results down
df = fast.copy()
for c in df: df[c] *= delta[c]
scale_find(df)
# save scaled results to csv
df.to_csv('data/results.csv', float_format='%.6f')
# save separate csv for each proc
for proc in PROCS:
    structs = ['list', 'bst', 'bst-rand']
    if proc == 'find': structs += ['bbst']
    cols = [s+'-'+proc for s in structs]
    df[cols].to_csv(f'data/{proc}.csv',
        header=structs, float_format='%.6f')
