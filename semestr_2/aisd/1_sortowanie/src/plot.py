import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def plot(df, title, log=False, style=None, min=50000, max=1000000):
    df[df.index <= max].plot(figsize=(6, 5), style=style)
    plt.style.use('seaborn-whitegrid')
    plt.xlabel('ilość elementów')
    plt.ylabel('czas (sekundy)')
    plt.xlim(min, max)
    if log: plt.yscale('log')
    plt.savefig(f'output/{title}.png'.replace(' ', '_'))

def plot_with_log(df, title, style=None):
    plot(df, title,                log=False, style=style, min=50000, max=1000000)
    plot(df, title + '_small',     log=False, style=style, min=1000, max=15000)
    plot(df, title + '_log',       log=True,  style=style, min=50000, max=1000000)
    plot(df, title + '_small_log', log=True,  style=style, min=1000, max=15000)

def column(algorithm, generator):
    return algorithm + ' - ' + generator

def plot_zad1():
    algorithms = ['heap sort', 'counting sort', 'shell sort', 'c++ sort']
    cols = [column(algorithm, 'losowe') for algorithm in algorithms]
    plot_with_log(df[cols], 'los', style='+-')
    plot_with_log(df[cols + ['bubble sort - losowe']], 'los_bub', style='+-')

def plot_zad2():
    generators=[['losowe', 'stałe', 'ls'], ['rosnące', 'malejące', 'rm'], ['a-kształtne', 'v-kształtne', 'av']]
    algorithms=['quick sort', 'heap sort', 'merge sort', 'c++ sort'] # + merge sort
    for (gen1, gen2, title) in generators:
        cols =  [column(algorithm, gen1) for algorithm in algorithms]
        cols += [column(algorithm, gen2) for algorithm in algorithms]
        plot_with_log(df[cols], title, style=['+-']*4 + ['.:']*4)

generators = ['losowe', 'stałe', 'rosnące', 'malejące', 'a-kształtne', 'v-kształtne' ]
disabled =  ['quick sort - a-kształtne', 'quick sort - v-kształtne']
disabled += [column('bubble sort', it) for it in generators]
df = pd.read_csv('output/bench.csv', index_col='length')
df = df.replace(0, np.nan)
#df[df.index > 15000][disabled] = np.nan

plot_zad2()
plot_zad1()
