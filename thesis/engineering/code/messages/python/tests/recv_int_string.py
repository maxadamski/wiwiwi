import colosseum as col
from sys import argv
from time import time

f = col.open(argv[1], 'r')
while True:
    t = time()
    tag, data = col.recv(f)
    t = time() - t
    if not data: continue
    print(f"tag {tag} <- ({data[0]}, {len(data[1])}, '{data[1]}')")
    print(f"elapsed {t*1e9:.0f}ns")
