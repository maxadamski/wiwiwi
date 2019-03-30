; program wyświela cyfry 0-9 na kolejnych wyświetlaczach
; można to było zrobić chyba w ~7 instrukcji mniej

$mod842
org 00h
mov  a, #0

loop:
anl  a, #00111111b
mov  p2, a
inc  a
call delay
mov  r4, a
anl  a, #00001111b
cjne a, #00001010b, skip
mov  a, r4
add  a, #00000110b
jmp  loop
skip:
mov  a, r4
jmp  loop

delay:
mov  r1, #00Ah
delay_0:
mov  r2, #01Bh
delay_1:
mov  r3, #0FFh
djnz r3, $
djnz r2, delay_1
djnz r1, delay_0
ret

end
