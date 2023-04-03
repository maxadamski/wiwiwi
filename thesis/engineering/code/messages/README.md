# Message Protocol


## Binary interface

```
section | field | bytes | value
--------|-------|-------|----------
header  | Tag   | 1     | any
header  | TypeS | 4     | len(Type)
header  | DataS | 4     | len(Data)
payload | Type  | TypeS | any
payload | Data  | DataS | any


type    |hex |bin
--------|----|-----------
u8      | 00 | 0 000 0000
u16     | 01 | 0 000 0001
u32     | 02 | 0 000 0010
u64     | 03 | 0 000 0011
i8      | 04 | 0 000 0100
i16     | 05 | 0 000 0101
i32     | 06 | 0 000 0110
i64     | 07 | 0 000 0111
f32     | 08 | 0 000 1000
f64     | 09 | 0 000 1001
char    | 0A | 0 000 1010
bool    | 0B | 0 000 1011
---     | 0C | 0 000 1100
---     | 0D | 0 000 1101
---     | 0E | 0 000 1110
---     | 0F | 0 000 1111
arr T 1 | 1X | 0 001 TTTT [d1]                         
arr T 2 | 2X | 0 010 TTTT [d1] [d2]                    
arr T 3 | 3X | 0 011 TTTT [d1] [d2] [d3]               
arr T 4 | 4X | 0 100 TTTT [d1] [d2] [d3] [d4]          
arr T 5 | 5X | 0 101 TTTT [d1] [d2] [d3] [d4] [d5]     
arr T 6 | 6X | 0 110 TTTT [d1] [d2] [d3] [d4] [d5] [d6]
arr T N | 7X | 0 111 TTTT [Nd] [d1] [..] [dN]          
---     | 8X | 1 000 TTTT
---     | 9X | 1 001 TTTT
---     | AX | 1 010 TTTT
---     | BX | 1 011 TTTT
---     | CX | 1 100 TTTT
---     | DX | 1 101 TTTT
---     | EX | 1 110 TTTT
---     | FX | 1 111 TTTT
```

**Note**

Array dimensions are always unsigned 32-bit integers.


## Format syntax

```peg
Dim <- ('%' / [0-9]+) ('<=' [0-9]+)?
Typ <- '%'? [uUiIfFcb] ('[' Dim (',' Dim)* ']')?
Fmt <- Typ*
```

**Examples**

Send a short number and a simple string.

```c
char str[] = "hello, world!";
msendf(fd, "%u %c[%]", 64, str, strlen(str));
```

Receive a short number and a string.

```c
char str[128];
u32 str_len;
u8 flag;
mscanf(buf, "%u %c[%<=128]", &flag, str, &str_len);
```

Receive a big array and it's dimensions.

```c
i32 boards[36][6][6];
u32 board_count;
mscanf(buf, "%I[%<=36,6,6]", boards, &board_count)
```


## C message API

```c
i32  msendf(int f, i8 tag, char const *fmt, ...);

void mscanf(u8 const *data, char const *fmt, ...);

i32  msend(int f, i8 tag, u8 const *buf, u32 size);

i32  mrecv(int f, i8 *tag, u8 *buf, u32 size);
```

## Pentago spec

```
board_size() -> i8
next_moves() -> (x y r: i8[n])
next_states() -> (b: i8[n, 2*(2*board_size+1)])
do_move(x y r: i8)
commit_move(x y r: i8)
undo_move()
```

