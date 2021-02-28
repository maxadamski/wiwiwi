arr = [14, 11, 8, 5, 2, -1, -1]

def sw(i):
    return 'space' if i == -1 else f'sw({i} downto {i-2})'

print('begin')
for i in range(7):
    mux = ', '.join(sw(arr[(j+i) % len(arr)]) for j in range(len(arr)))
    print(f'\tmux{i}: mux8to1 port map ({sw(17)}, {mux}, space, chr{i});')
print()
for i in range(7):
    print(f'\ttco{i}: chr7seg port map (chr{i}, hex{i});')
print('end;')