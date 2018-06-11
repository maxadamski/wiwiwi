import matplotlib.pyplot as plt
from scipy.stats import linregress
import pandas as pd
import numpy as np

plt.style.use('seaborn-whitegrid')
WATERMARK = False

def plot(df, title, ylabel="czas [s]", legend=None, log=False, style=None):
    df.plot(figsize=(6, 5), style=style)
    plt.xlim(min(df.index), max(df.index))
    plt.xlabel("liczba wierzchołków")
    plt.ylabel(ylabel)
    if legend: plt.legend(legend)
    if log: plt.yscale('log')
    if WATERMARK:
        plt.text(0, 0, 'dadamsky', fontsize=80, alpha=0.2)
    plt.savefig(f'output/{title}.png')

edges = pd.read_csv('output/edges.csv', index_col='n')
tsort = pd.read_csv('output/tsort.csv', index_col='n')

tsort *= 2
edges *= 2
for n in edges.index:
    edges.loc[n] /= n;

def fitted(x, col, degree):
    return df[col].apply(np.poly1d(fit))

for col in ['ms', 'li']:
    fit = np.polyfit(edges.index, edges[col], 1)
    fit_fun = np.poly1d(fit)
    edges[col+'-reg'] = list(map(fit_fun, edges.index))

print (edges)

plot(edges[['ms', 'mi', 'lk', 'li']], 'wyszukaj', 
        style=['-+', '-+', '-+', '.:'], log=False,
        legend=['AM macierz sąsiedztwa', 'IM macierz incydencji', 'EL lista krawędzi', 'AL lista incydencji'])

plot(edges[['ms', 'li', 'ms-reg', 'li-reg']] * 10**9, 'wyszukaj-nano', 
        style=['b +', 'r +', 'b-', 'r-'], log=False, ylabel="czas [ns]",
        legend=['AM macierz sąsiedztwa', 'AL lista incydencji'])

plot(tsort[['topsort']], 'sortuj',
        style=['b-+'],
        legend=['AL lista incydencji'])
