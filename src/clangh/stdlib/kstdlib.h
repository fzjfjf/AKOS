#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vga_colors.h>

#define VGA_ADDRESS 0xb8000
#define MAX_NUM_LINES 24
#define HEAP_START (address8)0xb0000000
#define HEAP_END (address8)0xbfffffff

#define true 1
#define false 0
typedef volatile unsigned char* address8;
typedef volatile uint16_t* address16;
typedef volatile uint32_t* address32;
//typedef unsigned int size_t;
typedef unsigned char byte;
typedef uint8_t bool;
typedef unsigned char uchar;			// this is to semantically diferentiate from byte
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

// structs with global variables
typedef struct {
	volatile unsigned char* vga;
	int line_number;
	int column_number;
	int color;
	bool remove_line_below;
} VGA_t;

void* kmalloc(size_t size);
void kprint(char *s);
int kdo_nothing();
void kmem_zero(address8 start, address8 end);
bool kstrcmp(char *s1, char *s2);			// returns true if strings are equal
size_t kstrlen(const char *s);
void kclear_vga_buffer();
void kfree(void *p);
void kupdate_cursor(uint16_t pos);
void reboot(uint64_t passwd);
int kprintf(char *format, ...);
void kputc(char c);
char *itoa(int integer);