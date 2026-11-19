#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef INCLUDE_COLORS_AUTO
#include <vga_colors.h>
#endif

#define VGA_ADDRESS 0xb8000
#define MAX_NUM_LINES 24
#define HEAP_START (address8)0xb0000000
#define HEAP_END (address8)0xbfffffff

#define SIZE(x) ( sizeof((x)) / sizeof((x)[0]) )						// x HAS TO BE AN ARRAY, NOT A POINTER
#define RANGE_FOR(cont, it) for (int it = 0; it < (cont).size; it++)	// cont HAS TO HAVE FIELD `size`; `it` is the name for the iterator, eg. i, j, k, index...

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
	uint8_t line_number;
	uint8_t column_number;
	uint8_t color;
	bool remove_line_below;
} VGA_t;


// =====================================================================================================================
// MEMORY
// =====================================================================================================================
void kfree(void *p);	// POINTER MUST COME FROM KMALLOC
void* kmalloc(size_t size) __attribute__((malloc));
void* krealloc(void* ptr, size_t size) __attribute__((alloc_size(2)));
void kmem_zero(address8 start, address8 end);
void kclear_vga_buffer();
size_t kmemcpy(address8 dest, address8 src, size_t n);
// =====================================================================================================================
// OUTPUT
// =====================================================================================================================
int kprintf(char *format, ...);
void kputc(char c);
void kprint(char *s);
// =====================================================================================================================
// INTEGER TO STRING CONVERSIONS
// =====================================================================================================================
void itoa(int integer, char *string);						// Note: `string` MUST BE AT LEAST 12 bytes, otherwise UB
void uitoa(unsigned int integer, char *string);				// Note: `string` MUST BE AT LEAST 11 bytes, otherwise UB
void xtoa(unsigned int integer, char *string);				// Note: `string` MUST BE AT LEAST 11 bytes, otherwise UB
void litoa(int64_t long_integer, char *string);				// Note: `string` MUST BE AT LEAST 21 bytes, otherwise UB
void lutoa(uint64_t long_integer, char *string);			// Note: `string` MUST BE AT LEAST 21 bytes, otherwise UB
void lxtoa(uint64_t long_integer, char *string);			// Note: `string` MUST BE AT LEAST 19 bytes, otherwise UB
// =====================================================================================================================
// STRING TO INTEGER CONVERSIONS
// =====================================================================================================================
int atoi(const char *s);
// =====================================================================================================================
// STRING FUNCTIONS
// =====================================================================================================================
bool kstrcmp(char *s1, char *s2);			// Note: Returns true if strings are equal
size_t kstrlen(const char *s);				// Note: Returns true if strings are equal
bool kstrcmp_ws(char*, char*);				// Note: Returns true if strings are equal
char *strchr(char *, char);					// Note: Returns true if strings are equal
char *strrchr(char *, char*);				// Note: Returns true if strings are equal
bool kstrncmp(char *, char *, size_t);		// Note: Returns true if strings are equal
// =====================================================================================================================
// OTHER
// =====================================================================================================================
void kupdate_cursor(uint16_t pos);
void reboot(uint64_t passwd);
int kdo_nothing();