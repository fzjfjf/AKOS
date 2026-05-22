global outb_wrapper

[bits 32]
outb_wrapper:
    mov edx, [esp + 4]                  ; get first two arguments
    mov eax, [esp + 8]                  ;

    out dx, al

    ret