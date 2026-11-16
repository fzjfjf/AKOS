#pragma once

#include <stdint.h>
#include <stddef.h>

void keyboard_handler();
void keyboard_init();

__attribute__((deprecated))
size_t kinput_b(char *s);
char kgetchar_nb();