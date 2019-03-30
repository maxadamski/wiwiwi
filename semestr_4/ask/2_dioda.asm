; za każdym przerwaniem dioda led zmienia stan on/off

$mod842
led  equ p3.4 ; to jest zwykły alias na poziomie asemblacji
org  00h
jmp  start
org  03h
jmp  inter

start:
mov  ie, #81h
jmp  $

inter:
cpl  led
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
