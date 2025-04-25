#ifndef _LINUX_GFX_H
#define _LINUX_GFX_H

#define GFX_OK         0
#define GFX_FAIL      -1
#define GFX_UNACCEL   -2
#define GFX_NO_DEVICE -ENODEV

extern int LINUX_GfxInitWaylandEGL();
extern int LINUX_GfxInitXorgGLX();
extern int LINUX_GfxInitDRM();
extern int LINUX_GfxInitFB();
extern int LINUX_GfxInit();

#endif
