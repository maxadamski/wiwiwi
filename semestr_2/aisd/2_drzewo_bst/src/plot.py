import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

plt.style.use('seaborn-whitegrid')
WATERMARK = False

def plot(df, title, ylabel="czas [s]", log=False, style=None):
    df.plot(figsize=(6, 5), style=style)
    plt.xlim(min(df.index), max(df.index))
    plt.xlabel("liczba elementów")
    plt.ylabel(ylabel)
    if log:
        plt.yscale('log')
    if WATERMARK:
        plt.text(0, 0, 'dadamsky', fontsize=80, alpha=0.2)
    plt.savefig(f'output/{title}.png')

def plot_all(df):
    plot(df[['list-append', 'bst-append', 'bst-rand-append']], 'append', style='-+')
    plot(df[['list-remove', 'bst-remove', 'bst-rand-remove']], 'remove', style='-+')
    plot(df[['list-find', 'bst-find', 'bst-rand-find', 'bbst-find']], 'find',
         ylabel="czas [ms]", style=['-+']*3 + ['.:'])

df = pd.read_csv('output/results.csv', index_col='n')
plot_all(df)
