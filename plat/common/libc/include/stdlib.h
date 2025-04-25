#include <stddef.h>

extern void *malloc(size_t size);
extern void free(void *ptr);
extern void __attribute__((noreturn)) exit(int status);
