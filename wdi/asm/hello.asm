%include "AuxMacros.asm"
section	.text
global _start
_start:
	mov ax, 2
	push ax
	mov ax, 4
	pop ax

	printReg ax
	printReg bx
	printReg dx

	return0
section	.data
HexDig	db '0', '1', '2', '3'
	db '4', '5', '6', '7'
	db '8', '9', 'A', 'B'
	db 'C', 'D', 'E', 'F'
msg	db '12 = 0000', 0xa
len     equ $ - msg
