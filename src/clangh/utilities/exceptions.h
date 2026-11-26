#pragma once

typedef struct ExcFrame {
	void *buf[5];             // buffer za __builtin_setjmp (5 riječi)
	struct ExcFrame *prev;    // prethodni okvir (za ugniježđene TRY-ove)
	int code;                 // kod greške koji je THROW upisao
} ExcFrame;

extern ExcFrame *exc_top;     // vrh liste okvira, NULL kad nema nijednog TRY-a

__attribute__((noreturn, noinline)) void THROW(int code);

#define TRY \
{ \
	ExcFrame _f; \
	_f.prev = exc_top; \
	exc_top = &_f; \
	if (__builtin_setjmp(_f.buf) == 0) {

#define CATCH(var) \
		exc_top = _f.prev; \
	} else { \
		exc_top = _f.prev; \
		__attribute__((unused)) int var = _f.code;


#define END_TRY \
		} \
	}

#define NO_THROW __attribute__((nothrow))