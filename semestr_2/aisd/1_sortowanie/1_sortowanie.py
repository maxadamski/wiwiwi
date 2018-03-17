import pandas as pd
import matplotlib.pyplot as plt

def plot(df, title, log=False):
    df.plot(figsize=(9, 7))
    plt.style.use('seaborn-whitegrid')
    plt.xlabel('ilość elementów')
    plt.ylabel('czas sortowania [s]')
    plt.xlim(1000, 10000)
    if log: plt.yscale('log')
    plt.savefig('plots/' + title + '.png')

def plot_algorithm(df, algorithm):
    cols = [algorithm + ' - ' + it for it in generators]
    plot(df[cols], 'algorytm ' + algorithm, log=False)

def plot_generator(df, generator):
    cols = [it + ' - ' + generator for it in algorithms]
    plot(df[cols], 'dane ' + generator, log=True)

df = pd.read_csv('1_sortowanie.csv', index_col='length')
#algorithms = ['selection sort', 'insertion sort', 'bubble sort', 'quick sort', 'c++ sort']
algorithms = ['counting sort', 'insertion sort', 'quick sort', 'c++ sort']
generators = ['losowe', 'stałe', 'rosnące', 'malejące', 'a-kształtne', 'v-kształtne']
for algorithm in algorithms: plot_algorithm(df, algorithm)
for generator in generators: plot_generator(df, generator)
