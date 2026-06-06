#pragma once

#include <keyboard/keyboard.h>
#include <stdlib/kstdlib.h>

typedef void (*program_t)();

void shell(program_t *programs[], char *names[], size_t count);