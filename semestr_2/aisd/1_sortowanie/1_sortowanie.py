import pandas as pd
import matplotlib.pyplot as plt

def plot(df, title, log=False, style=None):
    df.plot(figsize=(6, 5), style=style)
    plt.style.use('seaborn-whitegrid')
    plt.xlabel('ilość elementów')
    plt.ylabel('czas (sekundy)')
    plt.xlim(1000, 15000)
    if log: plt.yscale('log')
    filename = 'plots/' + title + '.png'
    plt.savefig(filename.replace(' ', '_'))

def plot_with_log(df, title, style=None):
    plot(df, title + '_log', log=True, style=style)
    plot(df, title, log=False, style=style)

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

df = pd.read_csv('1_sortowanie.csv', index_col='length')
plot_zad2()
plot_zad1()
