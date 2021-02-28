; program symuluje bezpieczne przejście dla pieszych (światło miga przed końcem)
; wersja do debuggowania - reaguje na przycisk

$mod842
org  00h
jmp  start
org  03h
jmp  inter

start:
setb ex0
setb ea
mov  p2, #00010001b ; RED RED
jmp  $

inter:
clr  ea
mov  p2, #01000001b ; RED YLW
call delay
mov  p2, #10000001b ; RED GRN
call delay
mov  p2, #01000001b ; RED YLW
call delay
mov  p2, #00010001b ; RED RED
call delay
mov  p2, #00011000b ; GRN RED
call delay
call hurry
mov  p2, #00010001b ; RED RED
call delay
setb ea
reti

hurry:
mov  a, #20
hurry_loop:
mov  p2, #00010000b ; --- RED
call delay2
mov  p2, #00011000b ; GRN RED
call delay2
djnz a, hurry_loop
ret

delay3:
mov  r1, #00Fh
delay3_0:
mov  r2, #0FFh
delay3_1:
mov  r3, #0FFh
djnz r3, $
djnz r2, delay3_1
djnz r1, delay3_0
ret

delay2:
mov  r2, #0FFh
delay2_0:
mov  r3, #0FFh
djnz r3, $
djnz r2, delay2_0
ret

end
