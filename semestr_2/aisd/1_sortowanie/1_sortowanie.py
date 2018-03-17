import pandas as pd
import matplotlib.pyplot as plt

def plot(df, title, log=False):
    df.plot(figsize=(6, 5))
    plt.xlabel('ilość elementów')
    plt.ylabel('czas sortowania [s]')
    plt.xlim(1000, 14000)
    if log: plt.yscale('log')
    filename = 'plots/' + title + '.png'
    plt.savefig(filename.replace(' ', '_'))

def plot_with_log(df, title):
    plot(df, title + '_log', log=True)
    plot(df, title, log=False)

def column(algorithm, generator):
    return algorithm + ' - ' + generator

def plot_zad1():
    algorithms = ['bubble sort', 'heap sort', 'counting sort', 'shell sort']
    cols = [column(algorithm, 'losowe') for algorithm in algorithms]
    plot_with_log(df[cols], 'los')

def plot_zad2():
    generators=[['losowe', 'stałe', 'ls'], ['rosnące', 'malejące', 'rm'], ['a-kształtne', 'v-kształtne', 'av']]
    algorithms=['quick sort', 'heap sort', 'c++ sort'] # + merge sort
    for (gen1, gen2, title) in generators:
        cols =  [column(algorithm, gen1) for algorithm in algorithms]
        cols += [column(algorithm, gen2) for algorithm in algorithms]
        plot_with_log(df[cols], title)

df = pd.read_csv('1_sortowanie.csv', index_col='length')
plot_zad1()
plot_zad2()
