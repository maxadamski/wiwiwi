{$ASMMODE intel}
var
	i: Integer;
begin
asm
@start:
	mov ax, $FFFE
	mov bx, 0
	mov cx, $0002
	mov dx, 0
	div cx
	jmp @start
end;
end.

