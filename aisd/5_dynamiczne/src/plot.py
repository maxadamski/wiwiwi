
import matplotlib.pyplot as plt
from scipy.stats import linregress
import pandas as pd
import numpy as np
import scipy as sp

plt.style.use('seaborn-whitegrid')
watermark = False

def plot(df, title, xlabel="", ylabel="", xscale=None, yscale=None,
         legend=None, style=None):
    df.plot(figsize=(6, 5), style=style)
    plt.xlim(min(df.index), max(df.index))
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    if yscale:
        plt.yscale(yscale)
    if xscale:
        plt.yscale(xscale)
    if legend:
        plt.legend(legend)
    if watermark:
        plt.text(0, 0, 'dadamsky', fontsize=80, alpha=0.2)
    plt.savefig(f'output/{title}.png')


#
# speed
#

sp = pd.read_csv('output/speed.csv', index_col='n')
sp['dynamic/b'] = sp['dynamic'] / sp['b']
sp['dynamic/b*c'] = sp['dynamic/b'] * 500_000
sp['greedy*c'] = sp['greedy'] * 25_000
sp['b/c'] = sp['b'] / 250_000

sp_b = pd.read_csv('output/speed.csv').sort_values(['b', 'n'])
sp_b['dynamic/n'] = sp_b['dynamic'] / sp_b['n']
sp_b['n/c'] = sp_b['n'] / 1_000_000
sp_b.index = sp_b['b']

sp_bn = pd.read_csv('output/speed.csv')
sp_bn['b*n'] = sp_bn['n'] * sp_bn['b']
sp_bn.sort_values(['b*n'], inplace=True)
sp_bn.index = sp_bn['b*n']

plot(sp[(14 <= sp.index) & (sp.index <= 28)][['brute', 'greedy', 'dynamic']], 'speed', xlabel="n", ylabel="t (s)",
        legend=['BF', 'GA', 'DP'],
        style=['r+-', 'b--', 'g+:'])

plot(sp[sp.index >=  100][['b/c', 'greedy', 'dynamic', 'dynamic/b*c', 'greedy*c']], 'speed-n-big',
        xlabel="n", ylabel="t (s)",
        legend=['b / C', 'GA', 'DP', 'DP/b * C', 'GA * C'],
        style=['k:', 'b+-', 'g+-', 'y+:', 'c+:'])

plot(sp_b[sp_b['n'] >= 100][['n/c', 'dynamic/n']], 'speed-b',
        xlabel="b", ylabel="",
        legend=['n / C', 'DP/n'],
        style=['k:', 'y+:'])

#
# quality
#

qu = pd.read_csv('output/quality.csv', index_col='n')
#qu.sort_values(['nu'], inplace=True)
#qu.index = qu['nu']

plot(qu[qu.index < 100][['brute', 'greedy', 'dynamic']], 'quality', xlabel='n', ylabel='jakość',
        legend=['BF', 'GA', 'DP'],
        style=['yo-', 'b+-', 'g+-'])

qu['greedy'] = qu['greedy'] / qu['dynamic']
qu['dynamic'] = 1
print(qu['greedy'])
plot(qu[qu.index >= 100][['greedy', 'dynamic']], 'quality-big', xlabel='n', ylabel='jakość',
        legend=['GA', 'DP'],
        style=['b+-', 'g+-'])

