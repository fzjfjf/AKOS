[bits 16]
global protected
start:
	;sti 					; enable interrupts

	mov ax, 0x0				; set up the fricking segments
	mov ds, ax
	mov es, ax
	
	;mov bx, start_switch			; move pointer to bx
	;mov ah, 0x0e				; set tty mode
	;start_loop1:
	;	mov al, [bx]			; move character at address in bx into al
	;	cmp al, 0x0			; check if null terminator
	;	je end_loop1			; jump to end if end reached
	;	int 0x10			; call the interrupt
	;	inc bx				; increment the pointer
	;	jmp start_loop1			; jump back to start
	;end_loop1:
	
	; all code from here to line 72 MUST NOT BE TOUCHED! 
	; ================ SWITCH TO PROTECTED MODE =================
	CODE_SEG equ code_descriptor - gdt_start
	DATA_SEG equ data_descriptor - gdt_start

	cli
	lgdt [dword gdt_descriptor]	
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax				; 32-bit mode switch here!
		
	jmp dword CODE_SEG:protected		; far jump! 

	; ================ GLOBAL DESCRIPTOR TABLE ================
	gdt_start:
		null_descriptor:
			dd 0x0
			dd 0x0
		code_descriptor:
			dw 0xffff
			dw 0x0
			db 0x0
			db 0b10011010
			db 0b11001111
			db 0
		data_descriptor:	
			dw 0xffff	
			dw 0x0
			db 0x0
			db 0b10010010
			db 0b11001111
			db 0x0
		gdt_end:
	
		gdt_descriptor:
			dw gdt_end - gdt_start - 1
			dd gdt_start
		

	
[bits 32]
extern kmain
extern signature
protected:
	mov eax, DATA_SEG
	mov ds, ax
	mov es, ax	
	mov ss, ax
	
	mov esp, 0x90000
	mov ebp, esp
	
	; all code above this comment MUST NOT BE TOUCHED!!!!!
	;mov al, '3'
	;mov ah, 0x0f
	;mov [0xb8000], ax

	; clear screen
	mov ebx, 0xb8000
	start_loop2:
		cmp ebx, 0xb8af0
		je end_loop2
		
		mov byte [ebx], ' '
		mov byte [ebx+1], 0x0f
	
		add ebx, 2
		jmp start_loop2	

	end_loop2:

	mov byte [0xb8000], 'S'
	mov byte [0xb0001], 0x0f

	
	; TODO: fix			is it fixed? i dont know and dont care since it works
	; print success message
	mov eax, [vga_buffer_pointer]
	mov ebx, switch_success
	start_loop3:
		mov dl, byte [ebx]
		cmp dl, 0x0
		je end_loop3
	
		mov [eax], dl
		mov byte [eax+1], 0x0f
		
		add eax, 2
		inc ebx
	
		jmp start_loop3

	end_loop3:
		mov [vga_buffer_pointer], eax
	; check if kernel is fully loaded
	mov eax, 0x004b4100		; signature
	mov ebx, [signature]
	cmp eax, ebx
	jne error_incomplete_kernel 

	call kmain			; call C code
			
	stop:
	hlt
	jmp stop

	error_incomplete_kernel:
		mov eax, [vga_buffer_pointer]
		mov ebx, error_incomplete_load
		start_loop4:
			mov dl, byte [ebx]
			cmp dl, 0x0	
			je stop
			
			mov [eax], dl
			mov byte [eax+1], 0xf4

			add eax, 2
			inc ebx

			jmp start_loop4
		end_loop4:

error_incomplete_load: db "[FAILED] Kernel is NOT FULLY LOADED. Aborting boot.", 0	
switch_success: db "[OK] Switched to 32-bit Protected Mode", 0		; dont include `10` and `13` bc they are not \r\n
start_switch: db "[OK] Starting switching process...", 13, 10, 0

vga_buffer_pointer: dd 0xb8000

times 512-($-$$) db 0
