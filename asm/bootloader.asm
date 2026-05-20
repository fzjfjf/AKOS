[bits 16]
[org 0x7c00]

start:
	cli				; disable interrupts, we dont need them now

	; =============== SEGMENTS AND STACK ===============
	mov ax, 0x0			; all segments are 0x0 for now
	mov ss, ax			; ss = stack segment
	mov ds, ax			; ds = data segment
	mov es, ax			; es = extra segment
	mov sp, 0x7c00			; the stack grows downward so starting it at 0x7c00 if fine for now
	mov [disk_number], dl		; save the disk number
	sti				; enable interrupts to allow printing to screen

	mov ah, 0x0e			; tty mode
	mov bx, success_loading_bootloader
	start_loop1:
		mov al, [bx]		; move character to lower 8 bits of a register
		cmp byte [bx], 0	; check if \0 was reached
		je end_loop1		; if reached go to end
		int 0x10		; output to console
		inc bx			; increment bx to get next character
		jmp start_loop1		; if not reached go to start again
	end_loop1:			; a label to signify to us the loop is done
	
	; =============== LOAD INITIALIZER ===============
	load_kernel:
	mov ax, 0x8000			; space to load init into
	mov es, ax			; load like this since 8086 is shit
	mov bx, 0x00			; start at offset 0
	; init is loaded into 0x8000:0x0000 or 0x80000, a lot of numbers, i know
	
	mov ah, 0x02			; 0x02 for reading disk
	mov al, 0x02			; number of sectors to read	
	mov ch, 0x00			; cylinder 0, whatever that is
	mov cl, 0x02			; sector at which we are starting
	mov dh, 0x00			; head 0
	mov dl, [disk_number]		; drive 0

	int 0x13			; call the interrupt
	jc error_reading		; if carry is set, stop now
	
	jmp 0x8000:0x0000		; jump to the initializer, goodbye cruel 8086 world!
	
	; =============== ERROR HANDLING ===============
	error_reading:
	mov ah, 0x0e
	mov bx, disk_error
	start_loop2:
		mov al, [bx]
		cmp byte [bx], 0x0
		je stop
		int 0x10
		inc bx
		jmp start_loop2
	end_loop2:

	
	; =============== STOP EXECUTING ===============
	stop:
	cli				; stop interrupts
	hlt				; halt the cpu
	jmp stop			; jump back up if cpu gets woken up


; basic text output to let us know what happened
disk_error: db "[FAILED] Error loading INITIALIZER", 13, 10, 0
success_loading_bootloader: db "[OK] Bootloader loaded", 13, 10, 0
disk_number: db 0

times 510-($-$$) db 0			; padding
dw 0xaa55				; the magic number
