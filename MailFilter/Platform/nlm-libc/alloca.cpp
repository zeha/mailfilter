#define MACHINE_WORD   int
#define UMACHINE_WORD   unsigned MACHINE_WORD
typedef UMACHINE_WORD   size_t;  /* type yielded by sizeof() */

extern void MFD_Out_func(int attr, const char* fmt, ...);

extern "C" {

void* alloca(size_t size)
{
	MFD_Out_func(0x1,"alloca %d.\n", size);
	return __builtin_alloca(size);
}
}

