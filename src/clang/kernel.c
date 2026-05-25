#include <kstdlib.h>
#include <keyboard.h>
#include <shell.h>

extern VGA_t vga_args;

void kmain()
{
	// ========== INITIALIZATIONS ==========
	kprint("[OK] Switched to Kernel\n");
	// Initialize heap to zero
	kprint("[  ] Zeroing heap memory...");
	kmem_zero(HEAP_START, HEAP_END);
	// print confirmation message
	kprint("\r[OK] Zeroed heap memory        \n");

	// ========== TESTS ==========
	// test kmalloc
	kprint("[TEST] Testing kmalloc...");

	char *test = kmalloc(128);
	if (test == NULL) goto kmalloc_error;

	*test = 'A';
	kfree(test);
	test = kmalloc(128);
	if (test == NULL || *test != 'A') goto kmalloc_error;

	kprint("\r[OK] kmalloc WORKS              \n");

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
	kprint("[FAILED] kmalloc DOESN'T WORK, HALTING\n");
	goto stop;



	// end
	stop:
	while (1) {			// NOLINT
			__asm__ volatile("hlt");	
	}
}
