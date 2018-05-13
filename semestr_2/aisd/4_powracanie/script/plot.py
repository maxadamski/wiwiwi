import matplotlib.pyplot as plt
from scipy.stats import linregress
import pandas as pd
import numpy as np


plt.style.use('seaborn-whitegrid')


def plot(df, title, xlabel="", ylabel="", xscale=None, yscale=None,
         legend=None, style=None, watermark=True):
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


bench = pd.read_csv('output/bench.csv', index_col='phi')

bench['hc'] = bench[[f'hc-{i}' for i in range(1, 25+1)]].mean(axis=1)

plot(bench[['ec']], 'euler', style=['-+'],
     xlabel='nasycenie grafu (procent)', ylabel='czas (sekundy)',
     legend=['EC - Cykl Eulera'])

plot(bench[['hc']], 'hamil', style=['-+'],
     xlabel='nasycenie grafu (procent)', ylabel='czas (sekundy)',
     legend=['HC - Cykl Hamiltona'])
