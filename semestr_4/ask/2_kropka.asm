; program wyświetla kropkę po kolei na czterech wyświetlaczach
; reaguje na przycisk

$mod842
org  00h
jmp  start
org  03h
jmp  inter

start:
mov ie, #10000010b
mov a,  #01001111b
jmp $

inter:
mov  p2, a
add  a, #00010000b
cjne a, #10000000b, noreset
mov  a, #00000000b
noreset:
call delay
reti

delay:
mov  r1, #007h
delay_0:
mov  r2, #0FFh
delay_1:
mov  r3, #0FFh
djnz r3, $
djnz r2, delay_1
djnz r1, delay_0
ret

end
