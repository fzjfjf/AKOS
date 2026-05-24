#pragma once

#include <keyboard.h>
#include <kstdlib.h>

typedef void (*program_t)();

void shell(program_t *programs[], char *names[], size_t count);