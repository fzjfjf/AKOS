#include <exceptions.h>
#include <kstdlib.h>

#ifdef __CLION_IDE__

void __builtin_longjmp(void **buf, int val);
#endif


extern void kpanic();

ExcFrame *exc_top = NULL;

void THROW(int code)
{
	if (!exc_top) kpanic();
	exc_top->code = code;
	__builtin_longjmp(exc_top->buf, 1);
}