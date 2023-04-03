import colosseum as col
from sys import argv

f = col.open(argv[1], 'r')
while True:
    tag, data = col.recv(f)
    if not data: continue
    if tag == 1:
        num1, str1, arr1 = data
        print(f'<-- {num1} "{str1}" {arr1.shape}')
    elif tag == 2:
        num1, num2, arr1, num3, num4 = data
        print(f'<-- {num1} {num2} {arr1.shape} {num3} {num4}')
    elif tag == 3:
        arr1, = data
        print(f'<-- {arr1.shape}')
