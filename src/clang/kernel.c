#define INCLUDE_COLORS_AUTO
#include <kstdlib.h>
#include <shell.h>
#include <pong.h>
#include <utilities/utilities.h>
#include <idt.h>
#include <calc.h>
#include <vector.h>
#include <keyboard.h>

#include "exceptions.h"

extern VGA_t vga_args;
extern void kpanic();

DEFINE_VECTOR(int, VecInt);

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
	keyboard_init();
	print_OK("Subscribed to Kernel\n", false);
	load_idt();
	print_OK("IDT Loaded\n", false);

	{
		// THIS PART HAS TO BE LIKE THIS, DO NOT CHANGE OR QUESTION
		// explanation: this is to change the first [OK] to green
		// initializer.asm should NOT BE changed EXCEPT in case of error
		*(address8)(VGA_ADDRESS + 3) = VGA_LIGHT_GREEN_ON_BLACK;
		*(address8)(VGA_ADDRESS + 5) = VGA_LIGHT_GREEN_ON_BLACK;
	}

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
	kfree(test);
	print_OK("kmalloc WORKS              \n", true);

	// test for cursor
	// kmem_zero((address8)VGA_ADDRESS, VGA_ADDRESS + (address8)2000);
	// *((address8)0xb8001) = 0x0f;
	// kupdate_cursor(0);
	// goto stop;

	// test kprintf
	// kprintf("Testing kprintf: %c, %b, %s, %i, %i, %i, %p, %x\n", 'A', 1, "test", 100, -100, 0, test, 0x0f);
	// // test itoa
	// char *test_itoa = kmalloc(12);
	// itoa(-100, test_itoa);
	// char *test_itoa2 = kmalloc(12);
	// itoa(100, test_itoa2);
	// kprintf("100: %s\n-100: %s\n", test_itoa2, test_itoa);
	// kfree(test_itoa);
	// kfree(test_itoa2);

	// VecInt v;
	// VecInt_init(&v);
	//
	// for (int i = 0; i < 10; i++) {
	// 	VecInt_push(&v, i);
	// }
	//
	// RANGE_FOR(v, i) {
	// 	kprintf("%i\n", VecInt_at(&v, i));
	// }
	// kprintf("%i\n", VecInt_at(&v, 0));
	// kprintf("%i\n", v.data[0]);
	//
	// VecInt_free(&v);

	// ========== SHELL ==========
	// start shell
	program_t programs[] = {
		pong, sl, zemo, calc
	};
	char *program_names[] = {
		"pong", "sl", "zemo", "calc"
	};

	TRY {
		shell(programs, program_names, SIZE(programs));
	} CATCH(e) {
		kprintf("Exception happened in SHELL or in running program, with exception %d\n", e);
		kprintf("Unexpected state, waiting 5 seconds and panicing...");

		uint64_t time = millis();

		while (millis() - time < 5000) {
			__asm__ volatile("hlt");
		}

		kpanic();
	} END_TRY;

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
