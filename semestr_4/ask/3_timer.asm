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
mov  tl0,  50000   ; czy to jest ok?
mov  th0,  15536   ; to samo
setb tr0
mov  r0, #0
mov  r1, #0
mov  r2, #0
mov  r3, #0

displ:
mov  a,  #00000000b ; wyświetlacz 0
add  a,  r0
mov  p2, a
mov  a,  #00010000b ; wyświetlacz 1
add  a,  r1
mov  p2, a
mov  a,  #01100000b ; wyświetlacz 2 + kropka
add  a,  r2
mov  p2, a
mov  a,  #00110000b ; wyświetlacz 3
add  a,  r3
mov  p2, 
jmp  displ

inter:
inc  r0
cjne r0, #1010b, skip0
mov  r0, #0 ; resetuj jeśli było 9
skip0:
inc  r1
cjne r1, #0111b, skip1
mov  r1, #0 ; resetuj jeśli było 6
skip1:
inc  r2
cjne r2, #1010b, skip2
mov  r2, #0 ; resetuj jeśli było 9
skip2:
inc  r3
cjne r3, #0111b, skip1
mov  r3, #0 ; resetuj jeśli było 6
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

