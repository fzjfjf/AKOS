#include "calc.h"
#include <kstdlib.h>

#define start 1

bool is_sign(char* s)
{
	if (kstrcmp(s, "+") || kstrcmp(s, "-") || kstrcmp(s, "*") || kstrcmp(s, "/")) return true;
	return false;
}

void calc(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		kprintf("%s\n", argv[i]);
	}

	int *nums = kmalloc(sizeof(int));
	int size_nums = 1;

	for (int i = start; i < argc; i++) {
		if (!is_sign(argv[i])) {
			if (i == size_nums) krealloc(nums, ++size_nums * sizeof(int));
			nums[i] = atoi(argv[i]);
		}
	}







}
