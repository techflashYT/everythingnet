/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Info Gathering
 * Copyright (C) 2025 Techflash
 */

#include <stddef.h>
#include <evrnet/cap.h>
#include <evrnet/plat.h>

platInfo_t PLAT_Info = {
	NULL, /* name */
	NULL, /* OS (we're going to pull the exact rev) */
	PLAT_STR_NATIVE, /* arch */
	NULL, /* cpu */
	NULL, /* gpu */
	0,    /* memSz */
	/* cap (we're going to fill in some bits
	 * like GPU accel and sound at runtime)
	 */
	CAP_HOST | CAP_DISP | CAP_INPUT | CAP_INPUT_REL | CAP_INPUT_ABS
};
