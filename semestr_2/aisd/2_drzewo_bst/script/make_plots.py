import matplotlib.pyplot as plt
import pandas as pd
import numpy as np


WATERMARK = False


def plot(df, title, ylabel="czas [s]", log=False, style=None):
    df.plot(figsize=(6, 5), style=style)
    plt.style.use('seaborn-whitegrid')
    plt.xlim(min(df.index), max(df.index))
    plt.xlabel("liczba elementów")
    plt.ylabel(ylabel)
    if log:
        plt.yscale('log')
    if WATERMARK:
        plt.text(0, 0, 'dadamsky', fontsize=80, alpha=0.2)
    plt.savefig(f'data/plots/{title}.png')


def plot_all(df):
    plot(df[['list-append', 'bst-append']], 'append', style='-+')
    plot(df[['list-find', 'bst-find', 'bbst-find']], 'find', ylabel="czas [ms]", style='-+')
    plot(df[['list-remove', 'bst-remove', 'bbst-remove']], 'remove', style='-+')


def process(df):
    for col in ['list-find', 'bbst-find', 'bst-find']:
        df[col] /= df.index
        df[col] *= 10**3


df = pd.read_csv('data/bench.csv', index_col='n').replace(0, np.nan)
process(df)
plot_all(df)
print(df)
