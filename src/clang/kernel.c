#include <kstdlib.h>
#include <keyboard.h>
#include <shell.h>

extern VGA_t vga_args;

void print_OK(char *s, bool r)
{
	vga_args.color = VGA_WHITE_ON_BLACK;
	if (r) kprint("\r[");
	else kprint("[");
	vga_args.color = VGA_LIGHT_GREEN_ON_BLACK;
	kprint("OK");
	vga_args.color = VGA_WHITE_ON_BLACK;
	kprint("] ");
	kprint(s);
}

void kmain()
{
	// ========== INITIALIZATIONS ==========
	print_OK("Switched to Kernel\n", false);

	// THIS PART HAS TO BE LIKE THIS, DO NOT CHANGE OR QUESTION
	// explanation: this is to change the first [OK] to green
	// initializer.asm should NOT BE changed EXCEPT in case of error
	*(address)(VGA_ADDRESS + 3) = VGA_LIGHT_GREEN_ON_BLACK;
	*(address)(VGA_ADDRESS + 5) = VGA_LIGHT_GREEN_ON_BLACK;

	// Initialize heap to zero
	kprint("[  ] Zeroing heap memory...");
	kmem_zero(HEAP_START, HEAP_END);
	// print confirmation message
	print_OK("Zeroed heap memory        \n", true);

	// ========== TESTS ==========
	// test kmalloc
	kprint("[TEST] Testing kmalloc...");

	char *test = kmalloc(128);
	if (test == NULL) goto kmalloc_error;

	*test = 'A';
	kfree(test);
	test = kmalloc(128);
	if (test == NULL || *test == 'A') goto kmalloc_error;

	print_OK("kmalloc WORKS              \n", true);

	// ========== SHELL ==========
	// start shell
	program_t *programs[] = {};
	char *program_names[] = {};
	shell(programs, program_names, 0);

	// notify that shell ended
	kprint("\nSHELL ended\n");

	// print that there is nothing more to execute, and stop
	kclear_vga_buffer();
	kprint("\n\n===== END OF KERNEL CODE, HALTING =====");
	goto stop;

	// ========== ERROR LABELS ==========
	kmalloc_error:
	vga_args.color = VGA_RED_ON_WHITE;
	kprint("\r[FAILED] kmalloc DOESN'T WORK, HALTING\n");
	goto stop;



	// end
	stop:
	while (1) {			// NOLINT
			__asm__ volatile("hlt");	
	}
}
