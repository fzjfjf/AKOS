#include <kstdlib.h>
#include <keyboard.h>
#include <shell.h>

// Make and initialize global struct
VGA_t vga_args = {
	.vga = (address)(VGA_ADDRESS + 160),	// adjust since initializer prints some text
	.line_number = 1,
	.remove_line_below = false,
};

void kmain()
{

	zero_memory(HEAP_START, HEAP_END);

	kprint("[OK] Switched to Kernel\n\0");

	char *test = kmalloc(128);

	if (test != NULL) kprint("kmalloc WORKED\n\0");
	else kprint("kmalloc didnt work\n\0");

	// size_t var1 = kinput_b(test);
	//
	// kprint(test, &vga_args);

	program_t *programs[] = {};
	char *program_names[] = {};
	shell(programs, program_names, 0, &vga_args);
	kprint("\nSHELL ended\n");
	kprint("\n\n===== END OF KERNEL CODE, HALTING =====");
	// end
	while (1) {
			__asm__ volatile("hlt");	
	}
}
