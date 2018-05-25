import matplotlib.pyplot as plt
from scipy.stats import linregress
import pandas as pd
import numpy as np

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


bench = pd.read_csv('output/bench.csv')

bench['hc'] = bench[[f'hc-{i}' for i in range(1, 25+1)]].mean(axis=1)

#
# plot euler cycles (by phi)
#

fig, ax = plt.subplots(figsize=(6, 5))
for key, group in bench.groupby(['phi']):
    ax = group.plot(ax=ax, kind='line', x='v', y='ec', label=f'φ = {key}%', style='-+')
    plt.xlabel('ilość wierzchołków')
    plt.ylabel('czas (sekundy)')
    plt.xlim(min(group['v']), max(group['v']))

plt.legend(loc='best')
plt.savefig(f'output/euler.png')

#
# plot hamilton cycles (by phi)
#

fig, ax = plt.subplots(figsize=(6, 5))
for key, group in bench.groupby(['phi']):
    ax = group.plot(ax=ax, kind='line', x='v', y='hc', label=f'φ = {key}%', style='-+')
    plt.xlabel('ilość wierzchołków')
    plt.ylabel('czas (sekundy)')
    plt.xlim(min(group['v']), max(group['v']))

plt.legend(loc='best')
plt.savefig(f'output/hamil.png')

#
# plot hamilton cycles (by v)
#

fig, ax = plt.subplots(figsize=(6, 5))
for key, group in bench.groupby(['v']):
    ax = group.plot(ax=ax, kind='line', x='phi', y='hc', label=f'|V| = {key}', style='-+')
    plt.xlabel('nasycenie krawędziami (procent)')
    plt.ylabel('czas (sekundy)')
    plt.xlim(10, 95)

plt.legend(loc='best')
plt.savefig(f'output/hamil_by_v.png')
