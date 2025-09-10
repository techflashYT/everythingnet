/*
 * EverythingNet - Common headers - Platform specific features - Linux - Graphics
 * Copyright (C) 2025 Techflash
 */

#ifndef _LINUX_GFX_H
#define _LINUX_GFX_H

#define GFX_OK         0
#define GFX_FAIL      -1
#define GFX_UNACCEL   -2
#define GFX_NO_DEVICE -ENODEV

extern int LINUX_GfxInitWaylandEGL(void);
extern int LINUX_GfxInitXorgGLX(void);
extern int LINUX_GfxInitDRM(void);
extern int LINUX_GfxInitFB(void);
extern int LINUX_GfxInit(void);

#endif
