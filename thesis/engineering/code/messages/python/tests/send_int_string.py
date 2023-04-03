import colosseum as co
from time import time
from sys import argv

f = co.open(argv[1], 'w')
while True:
    t = time()
    co.send(f, 42, (999, co.i32), 'Hello, World!')
    t = time() - t
    print(f'elapsed {t*1e9:.0f}ns')
