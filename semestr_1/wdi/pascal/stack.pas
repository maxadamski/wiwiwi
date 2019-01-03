{$ASMMODE intel}
var
	x, y, z, res: Integer;
begin

	y := 3;
	z := 7;
	x := 5;

asm

	mov  ax, y 
	mov  bx, z 
	mov  cx, x 
	push cx 
	push dx 
	call @f 
	jmp  @finish

@f:
	mov  si, ax 
	add  ax, ax 
	call @g 
	mul  cx 
	add  ax, si 
	ret 

@g:
	mul  cx 
	add  ax, bx 
	ret 

@finish:
	mov res, ax

end;

	writeln(res);

end.

