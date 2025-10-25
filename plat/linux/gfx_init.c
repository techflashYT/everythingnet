/*
 * EverythingNet - Linux Platform Specific Features - Graphics Support
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <evrnet/cap.h>
#include <evrnet/plat.h>
#include <evrnet/plat/linux_gfx.h>

int LINUX_GfxInit(void) {
	int err = GFX_FAIL;

#ifdef CONFIG_GFX_WAYLAND
	/* Try wayland with EGL first */
	if (getenv("WAYLAND_DISPLAY")) {
		/*err = LINUX_GfxInitWaylandEGL();*/
		if (err == GFX_OK) {
			PLAT_Info.cap |= CAP_GFX_ACCEL;
			return GFX_OK;
		}
		else if (err == GFX_FAIL)
			puts("GFX: Wayland detected but failed to initialize.");
		else if (err == GFX_UNACCEL) {
			puts("GFX: WARNING: Unaccelerated graphics output detected,"
			     "consider switching to DRM/KMS or fbdev if you don't"
			     "plan on using hosting functionality from this machine.");
			return GFX_UNACCEL;
		}
		else
			printf("Unknown failure (%d) from Wayland w/ EGL\n", err);
	}
#endif

#ifdef CONFIG_GFX_GLX
	/* No Wayland or it failed, try Xorg */
	if (getenv("DISPLAY")) {
		err = LINUX_GfxInitXorgGLX();
		if (err == GFX_OK) {
			PLAT_Info.cap |= CAP_GFX_ACCEL;
			return GFX_OK;
		}
		else if (err == GFX_FAIL)
			puts("GFX: Xorg detected but failed to initialize GLX.");
		else if (err == GFX_UNACCEL) {
			puts("GFX: WARNING: Unaccelerated graphics output detected,"
			     "consider switching to DRM/KMS or fbdev if you don't"
			     "plan on using hosting functionality from this machine.");
			return GFX_UNACCEL;
		}
		else
			printf("Unknown failure (%d) from Xorg w/ GLX\n", err);
	}
#endif

	/* No Wayland *or* Xorg, we can't host anything */
	PLAT_Info.cap &= ~CAP_HOST;

#ifdef CONFIG_GFX_DRM
	/* direct DRM/KMS */
	/*err = LINUX_GfxInitDRM();*/
	if (err == GFX_OK) {
		PLAT_Info.cap |= CAP_GFX_ACCEL;
		return GFX_OK;
	}
	else if (err == GFX_FAIL)
		puts("GFX: Failed to initialize DRM/KMS");
	else if (err == GFX_NO_DEVICE) {} /* we don't have a DRM device */
	else
		printf("Unknown failure (%d) from DRM/KMS\n", err);
#endif

#ifdef CONFIG_GFX_FB
	/* direct fbdev */
	/*err = LINUX_GfxInitFB();*/
	if (err == 0) {
		PLAT_Info.cap &= ~CAP_GFX_ACCEL; /* fbdev is unaccelerated */
		return 0;
	}
	else if (err == GFX_FAIL)
		puts("GFX: Failed to initialize fbdev");
	else if (err == GFX_NO_DEVICE) {} /* we don't have a framebuffer device */
	else
		printf("Unknown failure (%d) from fbdev\n", err);
#endif

	puts("GFX: ERROR: All graphics backends failed.  Dropping to headless mode.");
	PLAT_Info.cap &= ~(CAP_GFX_ACCEL | CAP_HOST | CAP_DISP);
	PLAT_Info.gpu = "None (headless)";

	return GFX_FAIL;
}
