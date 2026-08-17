#include <idt.h>
#include <stdint.h>


#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1


extern void pit_isr();
extern void gpf_isr();
extern void keyboard_isr();
extern void outb(uint16_t port, uint8_t data);
extern void kpanic();

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

void pic_remap() {
	// 1. ICW1: Inicijalizacija, traži ICW4
	outb(PIC1_COMMAND, 0x11);
	outb(PIC2_COMMAND, 0x11);

	// 2. ICW2: Offset vektora (Master na 32, Slave na 40)
	outb(PIC1_DATA, 0x20); // 32
	outb(PIC2_DATA, 0x28); // 40

	// 3. ICW3: Veza mastera i slave-a
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);

	// 4. ICW4: 8086 mode
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	// 5. Maskiraj sve prekide (opciono, možeš ih paliti jedan po jedan kasnije)
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}

void idt_set(uint8_t n, uint32_t handler);

void set_needed_handlers()
{
	idt_set(32, (uint32_t)(uintptr_t)pit_isr);
	idt_set(13, (uint32_t)(uintptr_t)gpf_isr);
	idt_set(33, (uint32_t)(uintptr_t)keyboard_isr);
}


void load_idt()
{
	set_needed_handlers();

	idt_ptr_t idt_ptr = {
		.limit = sizeof(idt) - 1,
		.base = (uint32_t)(uintptr_t)idt,
	};
	__asm__ volatile ("lidt %0" :: "m"(idt_ptr));

	pic_remap();

	__asm__ volatile ("sti");
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
	kpanic();
}