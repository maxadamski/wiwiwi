; program wyswietla wartosc potencjometru na wyswietlaczu (max 3 cyfry)
; rejestr adcdata = |....****|****....| = |adcdatah|adcdatal|
; interesuje nas tylko 8 bitow oznaczonych *
; bity po prawej stronie w adcdatal sa malo interesujace, bo zmieniaja sie zbyt gwaltownie
; musimy zlaczyc gwiazdki do jednego rejestru, a pozniej to wyswietlic (procedura display i refresh)
; przesuwamy bity na lewo w adcdatah, na prawo w adcdatal, maskujemy i zlaczamy ORem (procedura readadc)

$mod842

org  00h
jmp  start
org  33h
jmp  inter

inter:
call readadc
call display
reti

start:
setb tr2
setb ea
setb eadc
mov  adccon1, #10000010b
loop:
call display
jmp  loop

readadc:
mov  a, adcdatal
rr   a
rr   a
rr   a
rr   a
anl  a, #00001111b
mov  b, a
mov  a, adcdatah
rl   a
rl   a
rl   a
rl   a
anl  a, #11110000b
orl  a, b
ret                 ; zwracamy w akumulatorze

display:
mov  b,  #10        ; oczekujemy liczby w akumulatorze
div  ab             ; A, B == XYZ div 10, XYZ mod 10 == XY, Z
mov  r0, b          ; r0 na pierwszym wyswietlaczu
mov  b,  #10
div  ab             ; A, B == XY  div 10, XY  mod 10 == X , Y
mov  r1, b
mov  r2, a
mov  a,  r1
add  a,  #00010000b ; r1 na drugim wyswietlaczu
mov  r1, a
mov  a,  r2
add  a,  #00100000b ; r2 na trzecim wyswietlaczu
mov  r2, a
ret

refresh:
mov  p2, r0
call delay
mov  p2, r1
call delay
mov  p2, r2
call delay
ret

delay:
mov  r7, #00Fh
delay0:
mov  r6, #0FFh
djnz r6, $
djnz r7, delay0
ret

end
