[bits 32]

global pit_isr
global gpf_isr
global keyboard_isr

extern keyboard_handler
extern pit_handler
extern gpf_handler

pit_isr:
    pushad
    call pit_handler
    mov al, 0x20
    out 0x20, al
    popad
    iret

gpf_isr:
    pushad
    call gpf_handler
    popad
    add esp, 4
    iret

keyboard_isr:
	pushad
	call keyboard_handler
	mov al, 0x20
	out 0x20, al
	popad
	iret