{$ASMMODE intel}
var
	n, x, result: Integer;
begin
	n := 3;
	x := -1;
asm
	mov bx, 1	{ bx = 1 }
	mov cx, n	{ cx = n }
	mov ax, bx	{ ax = bx }
@loop:
	cmp cx, 0
	je  @fini	{ if cx == 0 goto fini }
	mov dx, x	{ dx = x }
	mul dx		{ dx:ax = ax * dx }
	call @a		{ goto a }
	add ax, bx	{ ax += bx }
	sub cx, 1	{ cx -= 1 }
	jmp @loop
@a: 
	neg bx		{ bx = -bx }
	ret
@fini:
	mov result, ax
end;
	{ -4 = FFFC, two's complement }
	writeln(result);
end.
