#pragma once
#include <stddef.h>

typedef void (*program_t)(int argc, char *argv[]);

void shell(program_t programs[], char *names[], size_t count);