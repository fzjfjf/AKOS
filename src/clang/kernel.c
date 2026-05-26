#include <kstdlib.h>
#include <keyboard.h>
#include <shell.h>

extern VGA_t vga_args;

void print_OK(char *s, bool r)
{
	vga_args.color = 0x0f;
	if (r) kprint("\r[");
	else kprint("[");
	vga_args.color = 0x0a;
	kprint("OK");
	vga_args.color = 0x0f;
	kprint("] ");
	kprint(s);
}

void kmain()
{
	// ========== INITIALIZATIONS ==========
	print_OK("Switched to Kernel\n", false);

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
	kprint("\n\n===== END OF KERNEL CODE, HALTING =====");
	goto stop;

	// ========== ERROR LABELS ==========
	kmalloc_error:
	vga_args.color = 0xf4;
	kprint("\r[FAILED] kmalloc DOESN'T WORK, HALTING\n");
	goto stop;



	// end
	stop:
	while (1) {			// NOLINT
			__asm__ volatile("hlt");	
	}
}
