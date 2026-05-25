#include <kstdlib.h>
#include <keyboard.h>
#include <shell.h>

void kmain()
{
	// ========== INITIALIZATIONS ==========
	// Initialize heap to zero
	zero_memory(HEAP_START, HEAP_END);
	// print confirmation message
	kprint("[OK] Switched to Kernel\n");

	// ========== TESTS ==========
	// test kmalloc
	kprint("[TEST] Testing kmalloc...\n");

	char *test = kmalloc(128);
	if (test == NULL) goto kmalloc_error;

	*test = 'A';
	kfree(test);
	test = kmalloc(128);
	if (test == NULL || *test != 'A') goto kmalloc_error;

	kprint("[OK] kmalloc WORKS\n");

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
	kprint("[FAILED] kmalloc DOESN'T WORK, HALTING\n");
	goto stop;



	// end
	stop:
	while (1) {			// NOLINT
			__asm__ volatile("hlt");	
	}
}
