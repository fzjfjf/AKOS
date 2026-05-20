void kprint(char *s, volatile unsigned char *vga) {
	int i = 0;	

	while (s[i] != 0) {
		*vga = s[i];
		vga++;
		*vga = 0x0f;
		vga++;
		i++;
	}

	return;
}

void kmain() {
	volatile unsigned char *vga = (unsigned char *)0xb8000;

	vga += 160;
	char *success = "[OK] Switched to Kernel\0";
	kprint(success, vga);

	// end
	while (1) {
			__asm__ volatile("hlt");	
	}
}
