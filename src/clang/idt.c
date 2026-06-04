#include <idt.h>
#include <stdint.h>

extern void pit_isr();
extern void gpf_isr();


typedef struct {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idt_ptr_t;

idt_entry_t idt[256];

void load_idt()
{
	idt_ptr_t idt_ptr = {
		.limit = sizeof(idt) - 1,
		.base = (uint32_t)idt,
	};
	__asm__ volatile ("lidt %0" :: "m"(idt_ptr));
}

void idt_set(uint8_t n, uint32_t handler)
{
	idt[n].offset_low = handler & 0xFFFF;
	idt[n].selector = 0x08;
	idt[n].zero = 0;
	idt[n].type_attr = 0x8e;
	idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void pit_handler(uint32_t hz)
{

}

void gpf_handler()
{

}