[bits 32]

global pit_isr
global gpr_isr

extern pit_handler
extern gpf_handler

pit_isr:
    pusha
    call pit_handler
    popa
    iret

gpf_isr:
    pusha
    call gpf_handler
    popa
    iret