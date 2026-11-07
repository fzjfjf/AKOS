#pragma once

#include <stdint.h>
#include <stddef.h>

void keyboard_handler();

__attribute__((deprecated))
size_t kinput_b(char *s);
char kgetchar_nb();