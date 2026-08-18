[bits 32]

global kpanic

; if we are here, shit is f***ed up BAD. we are not going back from here, so we can do whatever we want.

kpanic:
	; first lets get the esp value
	mov [last_esp], esp
	; now lets get the eip value
	mov esp, [esp]			; we can destroy esp here since we will set it up later
	mov [last_eip], esp

	; set up new stack on safe location
	mov esp, 0x90000

	; disable interrupts
	cli

	; save EVERYTHING (esp gets the old adress instead of this one)
	push cs
	push ds
	push ss
	push es
	push fs
	push gs
	pushf

	push edi
	push esi
	push ebp
	push dword [last_esp]
	push ebx
	push edx
	push ecx
	push eax

	push dword [last_eip]					; push this too to do everything in one loop
	; dont change the redoslijed in which registers are pushed, this is crucial


	call fill_screen_blue					; clear the screen

	mov eax, text1							; print kernel panic message
	call print_str

	mov byte [row], 2

	; here we need to print all registers, and their name before them, found in array text_registers
	mov eax, 0								; clear eax
	mov ebx, text_registers					; put the ptr in ebx
	mov ecx, 0								; index to know when to stop, we are printing 16 registers
	start_loop3:
		cmp ecx, 16							; check if we printed all registers
		jge end_loop3

		mov eax, ebx						; print the register name
		call print_str

		start_loop4:
			; we just need to increment ebx until we are after the null terminator
			inc ebx
			cmp byte [ebx], 0
			je end_loop4

			jmp start_loop4
		end_loop4:
		inc ebx								; go past the null terminator

		pop eax								; get the register from stack
		call print_hex						; print it

		inc byte [row]
		mov byte [col], 0
		inc ecx								; dont forget!
		jmp start_loop3						; and especially dont forget this!
	end_loop3:




	halt_loop:
		cli
		hlt
		jmp halt_loop


print_str:
	; takes a string and reapetedly calls print_char
	; eax contains the str pointer
	pushad

	mov ebx, eax 				; al (eax) needs to have the char, not ptr

	start_loop2:
		mov al, byte [ebx]		; move char to al
		cmp al, 0				; check for null terminator
		je end_loop2			; if null terminator end

		call print_char			; print char to console

		inc ebx					; increment ptr
		jmp start_loop2			; go to start
	end_loop2:

	popad
	ret

print_hex:
	; print a hex number
	; eax has the number
	; formula for printing to VGA with col and row is: 0xb8000 + (row * 80 + col) * 2
	pushad 						; save all registers

	mov ebx, eax
	; we will first print the "0x" part of hex num to screen
	mov al, '0'
	call print_char
	mov al, 'x'
	call print_char

	; ======== CONVERT ========
	; first we need to convert the hex num into a string. we are storing the char in eax
	; to convert one nibble to ascii, we need to:
	; add 0x30 if nibble is less than 10 (0xA)
	; add 0x37 if nibble is more than or equal to 10 (0xA)
	; we will move hex num to ebx, bc print_char needs the char in eax
	; we need to do this in a loop, 8 times total
	; ecx will have the index
	mov ecx, 0
	start_loop5:
		cmp ecx, 8
		jge end_loop5

		mov eax, ebx
		shr eax, 28
		if3:
			cmp eax, 0xA
			jl if3_false

			if3_true:
				add eax, 0x37
				jmp endif3
			if3_false:
				add eax, 0x30
		endif3:
		call print_char

		inc ecx
		shl ebx, 4
		jmp start_loop5
	end_loop5:

	popad						; pop them back
	ret

print_char:
	; print one character
	; eax (al) contains the char to print

	pushad

	mov edx, eax				; now edx contains the char
	movzx ebx, byte [col]
	movzx eax, byte [row]		; mow the row into eax
	imul eax, 80				; multiply by 80		0xb8000 + (__row * 80__ + col) * 2
	add eax, ebx				; add column 			0xb8000 + (__80row + col__) * 2
	shl eax, 1					; multiply by 2			0xb8000 + __80row+col * 2__
	add eax, 0xb8000  			; finally add offset 	__0xb8000 + 80row+col*2__

	; eax now contains the address to put char into
	mov byte [eax], dl			; put char into memory

	; now we need to advance col and row
	; condition:
	; if col is leass than 79, increment col
	; else if col is greater than or equal to 79, set col to zero and
	; 	if row is less than 24 increment row
	; 	else set row and col to zero
	if1:
		cmp byte [col], 79
		jge if1_true
		if1_false:
			inc byte [col]
			jmp endif1
		if1_true:
			mov byte [col], 0
			if2:
				cmp byte [row], 24
				jge if2_true
				if2_false:
					inc byte [row]
					jmp endif2
				if2_true:
					mov byte [row], 0
					jmp endif2
			endif2:
	endif1:

	popad
	ret

fill_screen_blue:
	; fill the screen with blue color
	; VGA 80x25 has 2000 characters
	push eax					; save eax since only eax is used

	mov eax, 0xb8000			; mov starting adress of VGA MMIO into eax
	start_loop1:
		cmp eax, 0xb8fa0		; compare if we reached end (2000 characters times 2 is 4000, which is 0xfa0 in hex, \
								; multiply by two since every character needs 2 bytes)
		jge end_loop1

		mov byte [eax], ' '			; move space because it is empty
		mov byte [eax + 1], 0x1f		; 0x1f is white on blue

		add eax, 2				; move to next char
		jmp start_loop1			; jump to start of loop
	end_loop1:

	pop eax						; return original value into eax

	ret							; return from function


; ==================== VARIABLES ====================
last_eip: dd 0x0			; this here holds the eip before calling this procedure
last_esp: dd 0x0			; same thinf but for esp
text_registers: db "EIP:    ", 0, \
				   "EAX:    ", 0, "ECX:    ", 0, "EDX:    ", 0, "EBX:    ", 0, \
				   "ESP:    ", 0, "EBP:    ", 0, "ESI:    ", 0, "EDI:    ", 0, \
				   "EFLAGS: ", 0, \
				   "GS:     ", 0, "FS:     ", 0, "ES:     ", 0, "SS:     ", 0, "DS:     ", 0, "CS:     ", 0
col: db 0
row: db 0
text1: db "                                  KERNEL PANIC                                  ", 0
test_str: db "0xDEADBEEF", 0