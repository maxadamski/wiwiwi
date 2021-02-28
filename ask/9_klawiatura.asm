$MOD842

MOV R0, #0
MOV R1, #0
MOV R2, #0
MOV R3, #0

loop:
MOV P3, #11111011b
JNB P3.4, siedem

MOV P3, #11111011b
JNB P3.5, osiem

MOV P3, #11111011b
JNB P3.6, dziewiec

MOV P3, #11111011b
JNB P3.5, zero

MOV P2, R0
CALL DELAY
MOV P2, R1
CALL DELAY
MOV P2, R2
CALL DELAY
MOV P2, R3
CALL DELAY

jmp  loop

siedem:
call shift
mov  R0, #7d
jmp  loop

osiem:
call shift
mov  R0, #8d
jmp  loop

dziewiec:
call shift
mov  R0, #9d
jmp  loop

zero:
call shift
mov  R0, #0d
jmp  loop

shift:
mov  A, R2
add  A, #16d
mov  R3, A

mov  A, R1
add  A, #16d
mov  R2, A

mov  A, R0
add  A, #16d
mov  R1, A
ret

delay:
mov  R6, #0fh
delay2:
mov  R7, #0ffh
djnz R7, $
djnz R6, delay2
ret

end
