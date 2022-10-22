
#include <stdint.h>
#include <sys/types.h>

uintptr_t __stack_chk_guard =
#if defined(__LP64__)
0x1234567812345678UL
#else
0x12345678;
#endif
;
