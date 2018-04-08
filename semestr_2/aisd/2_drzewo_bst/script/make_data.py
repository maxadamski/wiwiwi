from faker import Faker
from time import time

fake = Faker('ja_JP')
for i in range(10_000_000):
    first = fake.first_romanized_name()[:12]
    last = fake.last_romanized_name()[:12]
    print(f'{first} {last} {i:07}')

