%macro Print1Hex 2

%deftok Reg %1

    push Reg

    push ax

    shr Reg, %2*4

    and Reg, 0xF

    mov al, [HexDig + Reg]

    mov [msg + len - 2 - %2], al

    pop ax

    pop Reg 

%endmacro

%macro printReg 1

    %defstr Regi %1

    %strcat eRegi 'e', Regi

    push eax

    Print1Hex eRegi, 0

    Print1Hex eRegi, 1

    Print1Hex eRegi, 2

    Print1Hex eRegi, 3

    mov ax, Regi

    mov [msg], ax ; msg= 'rr = hhhh'

    push ebx

    push ecx

    push edx

    mov edx, len

    mov ecx, msg

    mov ebx, 1

    mov eax, 4

    int	0x80      ;sys_write

    pop edx

    pop ecx

    pop ebx

    pop eax

%endmacro

%macro return0 0

    mov	eax, 1

    int	0x80        ;sys_exit

%endmacro
