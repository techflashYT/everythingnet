#include "internal_libc.h"

unsigned int sleep(unsigned int seconds) {
#ifdef SYS_HAS_SLEEP
	SYS_sleep(seconds);
#endif
}


