#include "calc.h"
#include <kstdlib.h>

void calc(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		kprintf("%s\n", argv[i]);
	}


}
