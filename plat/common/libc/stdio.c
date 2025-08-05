/*
 * EverythingNet - Internal Libc - stdio
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include "internal_libc.h"

int putchar(int c) {
#ifdef SYS_HAS_PUTCHAR
	return SYS_putchar(c);
#else
	return 0;
#endif
}

int puts(const char *s) {
	int i = 0;
	while (*s) {
		putchar(*s);
		s++; i++;
	}
	putchar('\r');
	putchar('\n');

	return i;
}

void perror(const char *s) {
	printf("%s: %s\n", s, "stub error");
}
