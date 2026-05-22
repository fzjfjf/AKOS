#include <kstdlib.h>


typedef struct {
	address ptr;
	address next_ptr;
	size_t size;
	bool is_free;
} heap_mem_header_t;

extern void outb_wrapper(uint16_t port, uint8_t data);

void zero_memory(address start, address end)
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

void update_cursor(uint16_t pos)
{
	outb_wrapper(0x3D4, 14);
	outb_wrapper(0x3D5, (pos >> 8) & 0xFF);

	outb_wrapper(0x3D4, 15);
	outb_wrapper(0x3D5, pos & 0xFF);
}

void* kmalloc(size_t size)
{
	address p = HEAP_START;
	while (p < HEAP_END) {
		if (p[0] == 0) {
			heap_mem_header_t *header = (heap_mem_header_t *)p;
			header->ptr = p + sizeof(heap_mem_header_t);
			header->next_ptr = p + size + sizeof(heap_mem_header_t);
			header->size = size;
			header->is_free = false;
			return (void *)header->ptr;
		}

		heap_mem_header_t *header = (heap_mem_header_t *)p;
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

void kprint(char *s, VGA_t *vga_args)	//NOLINT
{
	int i = 0;	

	while (s[i] != 0) {

		if (s[i] == 10) {
			// 10 == line feed ('\n'), need to switch to new line
			if (vga_args->line_number >= MAX_NUM_LINES) {
				vga_args->line_number = 0;
				vga_args->vga = (address)VGA_ADDRESS;
			} else {
				vga_args->line_number++;
				vga_args->vga = vga_args->line_number * 160 + (address)VGA_ADDRESS;
			}
			// clear current line and next line 
			for (int j = 0; j < 320; j+=2) {
				vga_args->vga[j] = ' ';
				vga_args->vga[j + 1] = 0x0f;
			}

			i++;
			continue;
		}

		*vga_args->vga = s[i];
		vga_args->vga++;
		*vga_args->vga = 0x0f;
		vga_args->vga++; 
		i++;

		if (((int)(vga_args->vga - VGA_ADDRESS) / 2 ) - (vga_args->line_number) * 80 > 80) {
			vga_args->line_number++;
		}

	}

	// put cursor in place
	update_cursor((((int)vga_args->vga - VGA_ADDRESS) / 2));

	return;	//NOLINT
}

