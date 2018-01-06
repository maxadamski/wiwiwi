{$ASMMODE intel}
var
	i: Integer;
begin
asm
	mov ax, 0
	mov bx, 0
	mov cx, 0
	mov dx, 0

	mov dx, 3
	mov ax, 1
@loop:
	add ax, ax
	sub dx, 1
	jg @loop
	mov bx, 5
	div bx
	add ax, dx
end;
end.

