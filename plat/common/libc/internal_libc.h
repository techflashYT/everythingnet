#include <stddef.h>
#ifdef CONFIG_PLAT_DOS
#include <evrnet/plat/dos_libc_impls.h>
/* other platforms here...
 * if none of these trigger, then SYS_HAS_*
 * won't be defined, and none of these matter.
 */
#endif

extern void SYS_putchar(int c);
extern void SYS_exit(int status);
extern void *SYS_alloc(size_t size);
extern void SYS_free(void *ptr);
