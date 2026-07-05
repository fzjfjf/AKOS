[bits 32]

global kpanic


print:
	; expects:
	; eax - vga pointer
	; ebx - text pointer
	; string MUST BE null terminated
	pushad									; save registers to not destroy them

	xor ecx, ecx							; use this to hold the char
	print_loop_1_start:
		mov byte cl, [ebx]					; get one char
		cmp cl, 0							; if null terminator
		je print_loop_1_end					; go to end

		inc ebx								; increment text pointer

		mov byte [eax], cl					; mov char to vga pointer
		mov byte [eax + 1], 0x1F			; mov attribute to vga pointer plus one

		add eax, 2 							; move pointer to next char position
		jmp print_loop_1_start				; go back to start of loop
	print_loop_1_end:

	popad									; restore registers
	ret


convert_to_hex:
	; register value is in eax
	push eax								; save eax
	push ebx
	push ecx

	and eax, 0x0F							; isolate last four bits (nibble)

	cmp eax, 10
	jb L0_9
	jae LA_F

	LA_F:
		add eax, 0x37
		jmp end

	L0_9:
		add eax, 0x30

	end:
	mov edi, eax
	; restore registers back
	pop ecx
	pop ebx
	pop eax

	pop ebp									; return address here
	push edi								; put returned char above return address
	push ebp								; push return address back
	ret

kpanic:
    ; we go here if shit is f***ed. we never come back from this, so we can do some things we usually wouldnt.
    ; first, put esp back to start of stack, we need the stack to work
    mov [esp_value], esp					; save the location of the stack, we need it
    mov esp, 0x90000

    cli                                     ; turn off interrupts, we dont want this procedure to be interrupted

    ; now push all registers to stack, so we can use them later without destroying data
    ; push segment registers manually
    push cs
    push ds
    push ss
    push es
    push fs
    push gs
    pushf                                   ; push eflags
    pushad                                  ; push 8 general registers

	; get the eip of last instruction before `call kpanic`
	mov eax, [esp_value]
	mov [last_eip], eax

    ; now fill vga screen with blue color
    mov eax, [vga_ptr]
    xor ebx, ebx
    loop_start1:
        mov byte [eax], ' '
        inc eax
        mov byte [eax], 0x11
        inc eax

        inc ebx

        cmp ebx, 2000
        jne loop_start1
    end_loop1:
    mov dword [vga_ptr], 0xb8000					; reset vga pointer

	; now print kernel panic message
	mov eax, [vga_ptr]
	mov ebx, text
	call print

	mov dword eax, [vga_ptr]
    add eax, 160
    mov dword [vga_ptr], eax

	mov eax, [vga_ptr]
	mov ebx, text2
	call print

	mov dword eax, [vga_ptr]
	add eax, 160
	mov dword [vga_ptr], eax







    halt_loop:
        ; halt indefinitely
        cli
        hlt
        jmp halt_loop


vga_ptr: dd 0xB8000
text: db "KERNEL PANIC!", 0
text2: db "REGISTERS:", 0
last_eip: dd 0x0
esp_value: dd 0x0