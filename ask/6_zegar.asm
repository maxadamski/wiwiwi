; zegar godzin, minut i sekund, z uzyciem TIC
; godzina wyswietlana na zmiane z minutami i sekundami

$mod842

org  00h
jmp  start

start:
mov  hour, #9            ; jakis poczatkowy czas
mov  min,  #59
mov  sec,  #50
mov  timecon, #10000001b ; tryb 24h

loop:

mov  r4, #06Fh      ; wyswietlaj minuty i sekundy r4 razy
disp_mins:
mov  a,  sec        ; A    == XY == sec
mov  b,  #10
div  ab             ; A, B == XY div 10, XY mod 10 == X, Y
mov  r0, b
add  a,  #00010000b ; r1 na drugim wyswietlaczu
mov  r1, a
mov  a,  min        ; A    == XY == min
mov  b,  #10        ; A, B == XY div 10, XY mod 10 == X, Y
div  ab
mov  r2, b
mov  r3, a
mov  a,  r2
add  a,  #01100000b ; r2 na trzecim wyswietlaczu + kropka
mov  r2, a
mov  a,  r3
add  a,  #00110000b ; r3 na czwartym wyswietlaczu
mov  r3, a
call displ
djnz  r4, disp_mins

mov  r4, #06Fh      ; wyswietlaj godzine r4 razy
mov  r2, #00101111b ; trzeci wyswietlacz pusty
mov  r3, #00111111b ; czwarty wyswietlacz pusty
disp_hour:
mov  a,  hour       ; A    == XY == hour
mov  b,  #10
div  ab             ; A, B == XY div 10, XY mod 10 == X, Y
mov  r0, b
add  a,  #00010000b ; r1 na drugim wyswietlaczu
mov  r1, a
call displ
djnz  r4, disp_hour

jmp  loop

displ:
mov  p2, r0
call delay
mov  p2, r1
call delay
mov  p2, r2
call delay
mov  p2, r3
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
