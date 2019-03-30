; program pokazuje ile kroków silnika potrzeba do okrążenia + ta głupia częstotliwość
; 200 krokow
; 360 / (50*4) = 1.8 [deg/krok]
; f_graniczna = 2_000_000 [cykl/s] / (7*255 [cykl]) ~= 1120 Hz
; wersja do debugowania - reaguje na przycisk

$mod842
org  00h
jmp  start
org  03h
jmp  inter

start:
setb ex0
setb ea
jmp  $

inter:
clr  ea
mov  a,  #0
loop:
mov  p2, #0001b
call delay
mov  p2, #0010b
call delay
mov  p2, #0100b
call delay
mov  p2, #1000b
call delay
inc  a
cjne a,  #50, loop ; 50*4 kroków dobrane doświadczalnie (sic!)
setb ea
reti

delay:
delay0:
mov r2,  #7        ; 7*255 cykli dobrane doświadczalnie
delay1:
mov  r3, #0FFh
djnz r3, $
djnz r2, delay1
ret

end
