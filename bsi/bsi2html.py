#!/usr/bin/env python3
from sys import argv
from datetime import datetime

# path to '.dat' file
path = argv[1]
# HINT: files created on polish windows are sometimes encoded with 'cp1250' :/
code = argv[2] if len(argv) > 2 else 'utf-8'
with open(path, 'rb') as file:
    lines = str(file.read(), code).split('\n')
    lines = [x.strip() for x in lines]

print('''<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<title>Baza pytań BSI</title>
</head>
<style>
* { font-family: Helvetica, sans-serif; }
body { max-width: 1000px; margin: 20px auto; padding: 0 5px; }
h1 { font-size: 2em; }
h2 { font-size: 1.1em; font-weight: normal; }
div { max-width: 80ch; margin-bottom: 30px; }
li { margin-bottom: 5px; max-width: 50ch; }
b { color: green }
</style>
<body>
<h1>Baza pytań BSI</h1>
''')

print('Liczba pytań: '+str(len(lines))+'<br>')
print('Data wygenerowania: '+str(datetime.now())+'<br>')

for line in lines:
    id, line = line.split(' ', 1)
    parts = [x.strip() for x in line.split('*')]
    question, answers = parts[0], parts[1:]
    print('<div>')
    print('<h2>'+id+'. '+question+'</h2>')
    print('<ul>')
    for answer in answers:
        if answer.lower().startswith('[x]'):
            answer = '<b>'+answer[3:]+'</b>'
        print('<li>'+answer+'</li>')
    print('</ul>')
    print('</div>')

print('</body></html>')

