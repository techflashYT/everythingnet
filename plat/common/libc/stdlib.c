#include <stddef.h>
#include "internal_libc.h"

void __attribute__((noreturn)) exit(int status) {
#ifdef SYS_HAS_EXIT
	SYS_exit(status);
#endif

	while (1) {}
}

void *malloc(size_t size) {
#ifdef SYS_HAS_MALLOC
	return SYS_malloc(size);
#else
	return NULL;
#endif
}

void free(void *ptr) {
#ifdef SYS_HAS_FREE
	SYS_free(ptr);
#endif
}
