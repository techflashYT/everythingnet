/*
 * EverythingNet - Internal Libc - unistd
 * Copyright (C) 2025 Techflash
 */

#include "internal_libc.h"

unsigned int sleep(unsigned int seconds) {
#ifdef SYS_HAS_SLEEP
	SYS_sleep(seconds);
#endif
}


