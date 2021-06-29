import requests
import numpy as np
from time import sleep

api = 'http://localhost:8080'
freq = 1
rng = np.random.default_rng()

while True:
    elec = 100 * rng.integers(0, 3)
    temp = round(rng.normal(20, 5), 4)
    snow = rng.integers(0, 10)
    spee = round(rng.uniform(0, 100), 4)
    win1 = rng.integers(0, 10)
    for id, val in enumerate([elec, temp, snow, spee, win1]):
        requests.post(f'{api}/sensor/{id}', data=str(val))
    print('.', end='', flush=True)
    sleep(1/freq)
