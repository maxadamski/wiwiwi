; program to licznik minut i sekund z kropką pomiędzy
; reaguje na timer

$mod842
org  00h
jmp  start
org  0Bh
jmp  inter

start:
mov  ie,   #10000010b
mov  tmod, #00000001b
mov  th0,  #high 15356
mov  tl0,  #low  50000
setb tr0
mov  r0,   #00000000b
mov  r1,   #00010000b
mov  r2,   #01100000b
mov  r3,   #00110000b

displ:
mov  p2, r0
call delay
mov  p2, r1
call delay
mov  p2, r2
call delay
mov  p2, r3
call delay
jmp  displ

inter:
djnz r4, skip3  ; czekamy 40 przerwań
mov  r4, #40
inc  r0
cjne r0, #00001010b, skip0
mov  r0, #00000000b ; resetuj jeśli było 9
skip0:
inc  r1
cjne r1, #00010111b, skip1
mov  r1, #00010000b ; resetuj jeśli było 6
skip1:
inc  r2
cjne r2, #01101010b, skip2
mov  r2, #01100000b ; resetuj jeśli było 9
skip2:
inc  r3
cjne r3, #00000111b, skip1
mov  r3, #00110000b ; resetuj jeśli było 6
skip3:
reti

delay:
mov  r5, #0Fh      ; chyba za długi, nie testowałem
delay_0:
mov  r6, #FFh
delay_1:
mov  r7, #FFh
djnz r7, $
djnz r6, delay_1
djnz r5, delay_0
ret

end

