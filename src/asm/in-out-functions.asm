[bits 32]

global outb
global inb

outb:
    mov edx, [esp + 4]                  ; get first two arguments
    mov eax, [esp + 8]                  ;

    out dx, al

    ret

inb:
    mov dx, [esp + 4]
    in al, dx
    ret 