#include "calc.h"
#include <kstdlib.h>
#include <vector.h>

typedef char* cptr;
DEFINE_VECTOR(cptr, StrVec)

enum : uint8_t{
	PLUS, MINUS, TIMES, DIVIDE, SQRT, EQUALS, NUM
};

#define start 1

uint8_t get_sign(char* s)
{
	if (kstrcmp(s, "+")) return PLUS;
	if (kstrcmp(s, "-")) return MINUS;
	if (kstrcmp(s, "*")) return TIMES;
	if (kstrcmp(s, "/")) return DIVIDE;
	if (kstrcmp(s, "=")) return EQUALS;
	if (kstrcmp(s, "sqrt")) return SQRT;
	return NUM;
}

void calc(int argc, char *argv[])
{
	StrVec vec;
	StrVec_init(&vec);

	kprintf("CALC\n");

	for (int i = 0; i < argc; i++) {
		kprintf("%sEND\n", argv[i]);
		StrVec_push(&vec, argv[i]);
	}

	RANGE_FOR(vec, i) {
		kprintf("%s\n", *StrVec_atp(&vec, i));
	}





	StrVec_free(&vec);
}
