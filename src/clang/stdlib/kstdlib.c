#define INCLUDE_COLORS_AUTO
#include <kstdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
	address8 ptr;
	address8 next_ptr;
	size_t size;
	bool is_free;
} heap_mem_header_t;

// Make and initialize global struct
VGA_t vga_args = {
	.vga = (address8)(VGA_ADDRESS + 160),	// adjust since initializer prints some text
	.line_number = 1,
	.remove_line_below = false,
	.column_number = 255,					// 255 since kstdlib.c doesnt use this, and it is uint8_t
	.color = VGA_WHITE_ON_BLACK,
};

extern void outb(uint16_t port, uint8_t data);
extern uint16_t inb(uint16_t port);

size_t kstrlen(const char *s)
{
	size_t len = 0;
	while (s[len] != 0) {
		len++;
	}
	return len;
}

void reboot(uint64_t passwd)
{
	if ((passwd ^ 0x98dfcbab2478249cULL) == 0x1b0542541062eaa8ULL) {
		outb(0x64,	0xfe);
	}
}

bool kstrcmp(char *s1, char *s2)
{
	// kprint("\nS1: ", vga_args);
	// kprint(s1, vga_args);
	// kprint("\nS2: ", vga_args);
	// kprint(s2, vga_args);

	size_t len1 = kstrlen(s1);
	size_t len2 = kstrlen(s2);
	if (len1 != len2) {
		return false;
	}
	for (int i = 0; i < len1; i++) {
		if (s1[i] != s2[i]) {
			return false;
		}
	}
	return true;
}

void kmem_zero(address8 start, address8 end)
{
	uint32_t *p = (uint32_t *)start;
	while (p < (uint32_t *)end) {
		*p++ = 0;
	}
}

int kdo_nothing()
{
	return 0;
}

void kupdate_cursor(uint16_t pos)
{
	outb(0x3D4, 0x0A);
	uint8_t start = inb(0x3D5);
	outb(0x3D4, 0x0A);
	outb(0x3D5, start & 0x1F);

	outb(0x3D4, 0x0A);
	outb(0x3D5, 14);

	outb(0x3D4, 0x0B);
	outb(0x3D5, 15);

	outb(0x3D4, 14);
	outb(0x3D5, (pos >> 8) & 0xFF);

	outb(0x3D4, 15);
	outb(0x3D5, pos & 0xFF);
}

void* kmalloc(size_t size)
{
	// TODO: add reusing blocks instead of only being a bump allocator - fixed, kinda. eh, good enough. \
	   TODO: defragmentation isnt necessary right now
	address8 p = HEAP_START;
	while (p < HEAP_END) {
		if (p[0] == 0) {
			if (size > HEAP_END - HEAP_START ||
				p + sizeof(heap_mem_header_t) + size > HEAP_END
				)
					return NULL;

			heap_mem_header_t *header = (heap_mem_header_t *)p;
			header->ptr = p + sizeof(heap_mem_header_t);
			header->next_ptr = p + size + sizeof(heap_mem_header_t);
			header->size = size;
			header->is_free = false;
			kmem_zero(header->ptr, header->ptr + header->size);
			return (void *)header->ptr;
		}

		heap_mem_header_t *header = (heap_mem_header_t *)p;

		if (header->is_free && header->size >= size) {
			header->is_free = false;
			kmem_zero(header->ptr, header->ptr + header->size);
			return (void *)header->ptr;
		}

		p = header->next_ptr;
	}

	return NULL;
}

void kfree(void *p)
{
	heap_mem_header_t *header = (heap_mem_header_t *)p - 1;
	header->is_free = true;
	return;		//NOLINT
}

int kprintf(char *format, ...)
{
	va_list args;

	int count = 0;
	char *temp_format = format;
	// get count
	while (*temp_format) {
		if (*temp_format == '%') {
			count++;
		}
		temp_format++;
	}

	va_start(args, format);

	while (*format) {
		if (*format == '%') {
			switch (*++format) {
				case '%':
					kputc('%');
					break;
				case 'c':

					kputc((char)va_arg(args, int));

					break;
				case 'b':

					int b = va_arg(args, int);

					if (b) kprint("true");
					else kprint("false");

					break;
				case 's':

					kprint(va_arg(args, char *));

					break;
				case 'i':

					char* temp_si = kmalloc(12);
					itoa(va_arg(args, int), temp_si);
					kprint(temp_si);
					kfree(temp_si);

					break;
				case 'u':

					char *temp_su = kmalloc(11);
					uitoa(va_arg(args, unsigned int), temp_su);
					kprint(temp_su);
					kfree(temp_su);

					break;
				case 'p':				// intentional
				case 'x':

					char *temp_sx = kmalloc(11);
					xtoa(va_arg(args, unsigned int), temp_sx);
					kprint(temp_sx);
					kfree(temp_sx);

					break;
				case 'l':
					switch (*++format) {
						case 'u':				// NOLINT - placeholder temporarily
							char *temp_slu = kmalloc(21);
							lutoa(va_arg(args, uint64_t), temp_slu);
							kprint(temp_slu);
							kfree(temp_slu);

							break;
						case 'i':
							char *temp_sli = kmalloc(21);
							litoa(va_arg(args, int64_t), temp_sli);
							kprint(temp_sli);
							kfree(temp_sli);

							break;
						case 'x':
							char *temp_slx = kmalloc(21);
							litoa(va_arg(args, int64_t), temp_slx);
							kprint(temp_slx);
							kfree(temp_slx);

							break;
						default:
							va_end(args);
							return -1;
					}
				case 'f':
				default:
					va_end(args);
					return -1;
			}
		} else {
			kputc(*format);
		}

		format++;
	}

	va_end(args);

	return count;
}

void kprint(char *s)	//NOLINT
{
	int i = 0;	

	if (s == NULL) return;

	while (s[i] != 0) {

		if (s[i] == 10) {
			// 10 == line feed ('\n'), need to switch to new line
			if (vga_args.line_number >= MAX_NUM_LINES) {
				vga_args.line_number = 0;
				vga_args.vga = (address8)VGA_ADDRESS;
			} else {
				vga_args.line_number++;
				vga_args.vga = vga_args.line_number * 160 + (address8)VGA_ADDRESS;
			}
			// clear current line and next line 
			for (int j = 0; j < 320; j+=2) {
				vga_args.vga[j] = ' ';
				vga_args.vga[j + 1] = vga_args.color;		// this HAS to be white on black, otherwise cursor cant	\
															   be seen!
			}

			i++;
			continue;
		} else if (s[i] == 13) {
			vga_args.vga = (address8)(VGA_ADDRESS + vga_args.line_number * 160);
			i++;
		} else if (s[i] == '\b') {
			vga_args.vga -= 2;
			i++;
			continue;
		}

		*vga_args.vga = s[i];
		vga_args.vga++;
		*vga_args.vga = vga_args.color;
		vga_args.vga++;
		i++;

		// we need to check if the last character is on the 79th position. to do that we need to get the x position, /
		// which we do by doing the formula to get linear position and subtracting number of characters times number /
		// of rows
		if (((int)(vga_args.vga - VGA_ADDRESS) / 2 ) - (vga_args.line_number) * 80 > 80) {
			vga_args.line_number++;
		}

	}

	// put cursor in place
	// vga is a pointer, position is not x,y but linear, so to get the position we need to subtract by the starting /
	// address of vga buffer and divide by half because one character is two bytes
	kupdate_cursor((((int)vga_args.vga - VGA_ADDRESS) / 2));

	return;	//NOLINT
}

void kclear_vga_buffer()
{
	address8 vga_p = (address8)VGA_ADDRESS;
	for (int i = 0; i < (MAX_NUM_LINES + 1) * 80 * 2; i++) {
		vga_p[i] = 0;
	}
	vga_args.vga = (address8)VGA_ADDRESS;
	vga_args.line_number = 0;
}

void kputc(char c)
{
	if (c == '\n') {
		// 10 == line feed ('\n'), need to switch to new line
		if (vga_args.line_number >= MAX_NUM_LINES) {
			vga_args.line_number = 0;
			vga_args.vga = (address8)VGA_ADDRESS;
		} else {
			vga_args.line_number++;
			vga_args.vga = vga_args.line_number * 160 + (address8)VGA_ADDRESS;
		}
		// clear current line and next line
		for (int j = 0; j < 320; j+=2) {
			vga_args.vga[j] = ' ';
			vga_args.vga[j + 1] = vga_args.color;		// this HAS to be white on black, otherwise cursor cant	\
			be seen!
		}

		return;
	} else if (c == '\r') {
		vga_args.vga = (address8)(VGA_ADDRESS + vga_args.line_number * 160);
	} else if (c == '\b') {
		vga_args.vga -= 2;
		return;
	}

	*vga_args.vga = c;
	vga_args.vga++;
	*vga_args.vga = vga_args.color;
	vga_args.vga++;


	kupdate_cursor((((int)vga_args.vga - VGA_ADDRESS) / 2));
}

void itoa(int integer, char *string)
{
	bool skipped_zeros = false;
	int i = 0;

	if (integer & 0b10000000000000000000000000000000) {
		string[i++] = '-';
	}

	unsigned int num = (unsigned int)integer;
	if (integer < 0) num = 0 - num;


	for (int divisor = 1000000000; divisor > 0; divisor /= 10) {
		if (num / divisor != 0 || skipped_zeros) {
			skipped_zeros = true;
			string[i++] = '0' + num / divisor;
			num %= divisor;
		}
	}
	string[i] = '\0';
	if (!skipped_zeros) {string[0] = '0'; string[1] = '\0';}

}

void uitoa(unsigned int uinteger, char *string)
{
	bool skipped_zeros = false;
	int i = 0;

	for (int divisor = 1000000000; divisor > 0; divisor /= 10) {
		if (uinteger / divisor != 0 || skipped_zeros) {
			skipped_zeros = true;
			string[i++] = '0' + uinteger / divisor;
			uinteger %= divisor;
		}
	}
	string[i] = '\0';
	if (!skipped_zeros) {string[0] = '0'; string[1] = '\0';}
}

void xtoa(unsigned int hex, char *string)
{
	int i = 0;
	string[i++] = '0';
	string[i++] = 'x';

	for (int j = 28; j >= 0; j -= 4) {
		unsigned int nibble = (hex >> j) & 0x0F;

		if (nibble < 10) {
			string[i++] = '0' + nibble;
		} else {
			string[i++] = nibble - 10 + 'A';
		}
	}

	string[i] = '\0';
}

void litoa(int64_t integer, char *string)
{
	// bool skipped_zeros = false;
	// int i = 0;
	//
	// if (integer & 0b1000000000000000000000000000000000000000000000000000000000000000) {
	// 	string[i++] = '-';
	// }
	//
	// uint64_t num = (uint64_t)integer;
	// if (integer < 0) num = 0 - num;
	//
	//
	// for (uint64_t divisor = 10000000000000000000ULL; divisor > 0; divisor /= 10) {
	// 	if (num / divisor != 0 || skipped_zeros) {
	// 		skipped_zeros = true;
	// 		string[i++] = '0' + num / divisor;
	// 		num %= divisor;
	// 	}
	// }
	// string[i] = '\0';
	// if (!skipped_zeros) {string[0] = '0'; string[1] = '\0';}
}

void lutoa(uint64_t uinteger, char *string)
{
	// bool skipped_zeros = false;
	// int i = 0;
	//
	// for (uint64_t divisor = 10000000000000000000ULL; divisor > 0; divisor /= 10) {
	// 	if (uinteger / divisor != 0 || skipped_zeros) {
	// 		skipped_zeros = true;
	// 		string[i++] = '0' + uinteger / divisor;
	// 		uinteger %= divisor;
	// 	}
	// }
	// string[i] = '\0';
	// if (!skipped_zeros) {string[0] = '0'; string[1] = '\0';}
}

void lxtoa(uint64_t hex, char *string)
{
	int i = 0;
	string[i++] = '0';
	string[i++] = 'x';

	for (int j = 60; j >= 0; j -= 4) {
		unsigned int nibble = (hex >> j) & 0x0F;

		if (nibble < 10) {
			string[i++] = '0' + nibble;
		} else {
			string[i++] = nibble - 10 + 'A';
		}
	}

	string[i] = '\0';
}