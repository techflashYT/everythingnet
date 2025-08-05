/*
 * EverythingNet - Common headers - Capabilities
 * Copyright (C) 2025 Techflash
 */

#ifndef _CAP_H
#define _CAP_H

/*
 * Capabilities
 */
#define CAP_NONE      0 
#define CAP_HOST      (1 << 0)
#define CAP_DISP      (1 << 1)
#define CAP_GFX_ACCEL (1 << 2)
#define CAP_SND       (1 << 3)
#define CAP_INPUT     (1 << 4)
#define CAP_INPUT_REL (1 << 5)
#define CAP_INPUT_ABS (1 << 6)

extern void CAP_Cap2Str(int cap, char *out);

#endif
