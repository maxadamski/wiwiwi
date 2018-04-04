from faker import Faker
from time import time
import pandas as pd

fake = Faker('ja_JP')
for i in range(10_000_000):
    first = fake.first_romanized_name()[:12]
    last = fake.last_romanized_name()[:12]
    print('{} {} {:07}'.format(first, last, i))

