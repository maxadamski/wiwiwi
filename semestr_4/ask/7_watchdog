; program realizujacy timer modulo 60 za pomoca TIC
; na przerwanie zegar zatrzymuje sie
; na kolejne przerwanie zegar zeruje sie i startuje od nowa
; dodatkowo P. Giera chcial, zeby zaprezentowac dzialanie watchdoga
; wystarczy wywolac `call watchdog` w dowolnym miejscu np. przerwaniu

$mod842

org  00h
jmp  start
org  03h
jmp  inter

inter:
;call watchdog
mov  r7, sec     ; wylaczenie TIC resetuje sec, wiec musimy zapisac
mov  a, timecon
xrl  a, #1
mov  timecon, a
jz   noreset     ; resetujemy sec tylko po ponownym wlaczeniu timera
mov  r7, #0
noreset:
mov  sec, r7
call delay_long
reti

start:
mov  sec, #0
mov  timecon, #0
setb ex0
setb ea
loop:
mov  a, sec
call display
call refresh
jmp  loop

watchdog:
clr  ea
setb wdwr
mov  wdcon, #72h
setb ea
ret

display:
mov  b,  #10
div  ab
mov  r0, b
add  a,  #00010000b
mov  r1, a
ret

refresh:
mov  p2, r0
call delay
mov  p2, r1
call delay
ret

delay:
mov  r7, #00Fh
delay0:
mov  r6, #0FFh
djnz r6, $
djnz r7, delay0
ret

delay_long:
mov  r7, #007h
delay_long0:
mov  r6, #0FFh
delay_long1:
mov  r5, #0FFh
djnz r5, $
djnz r6, delay_long1
djnz r7, delay_long0
ret

end
