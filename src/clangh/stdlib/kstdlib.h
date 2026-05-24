#pragma once

#include <stddef.h>
#include <stdint.h>

#define VGA_ADDRESS 0xb8000
#define MAX_NUM_LINES 24
#define HEAP_START (address)0xb0000000
#define HEAP_END (address)0xbfffffff

#define true 1
#define false 0
typedef volatile unsigned char* address;
//typedef unsigned int size_t;
//typedef unsigned char byte;
typedef uint8_t bool;

// structs with global variables
typedef struct {
	volatile unsigned char* vga;
	int line_number;
	bool remove_line_below;
} VGA_t;

void* kmalloc(size_t size);
void kprint(char *s);
int kdo_nothing();
void zero_memory(address start, address end);
bool kstrcmp(char *s1, char *s2);
size_t kstrlen(char *s);
void kclear_vga_buffer();