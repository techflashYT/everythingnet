/*
 * EverythingNet - Linux Platform Specific Features - Graphics Support - GLX
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <evrnet/state.h>
#include <evrnet/plat.h>
#include <evrnet/plat/linux_gfx.h>

static Display *dpy;
static Window win;
static GLXContext ctx;

static void LINUX_GfxCleanupGLX(void) {
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, ctx);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

int LINUX_GfxInitXorgGLX(void) {
	int screen;
	XVisualInfo *vi;
	Window root;
	Colormap cmap;
	XSetWindowAttributes swa;
	char gpuNameStr[512];
	const GLubyte *renderer, *vendor;
	char *chrptr;

	int attribs[] = {
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER,
		None
	};
	memset(gpuNameStr, 0, sizeof(gpuNameStr));

	/* try to reach out to X */
	dpy = XOpenDisplay(NULL);
	if (!dpy) { /* it's dead */
		printf("GFX: Xorg/GLX: failed to open X display\n");
		return GFX_FAIL;
	}

	/* default screen (there should only be one anyways) */
	screen = DefaultScreen(dpy);

	/* idk what this does but it's more boilerplate */
	vi = glXChooseVisual(dpy, screen, attribs);
	if (!vi) {
		printf("GFX: Xorg/GLX: no appropriate visual found\n");
		XCloseDisplay(dpy);
		return GFX_FAIL;
	}

	/* create simple X11 window */
	root = RootWindow(dpy, vi->screen);
	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;

	/* make it */
	win = XCreateWindow(
		dpy, root,
		0, 0, 100, 100, 0,
		vi->depth,
		InputOutput,
		vi->visual,
		CWColormap | CWEventMask,
		&swa
	);

	/* make the window visible
	 * XMapWindow(dpy, win);
	 */

	/* try to create a GLX context */
	ctx = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	if (!ctx) {
		printf("GFX: Xorg/GLX: failed to create GLX context\n");
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
		return -1;
	}

	/* set GLX to use this X window */
	glXMakeCurrent(dpy, win, ctx);

	/* get the renderer, version, and vendor */
	renderer = glGetString(GL_RENDERER);
	vendor   = glGetString(GL_VENDOR);
	/*ver      = glGetString(GL_VERSION);*/

	/*
	printf("GPU Renderer: %s\n", renderer);
	printf("GPU Vendor:   %s\n", vendor);
	printf("GL Version:   %s\n", ver);*/

	/* clean it up a bit */

	/* step 1: does the renderer string have the vendor
	 * name in it?  if not, copy the vendor name in.
	 * if it does, don't repeat ourselves.
	 */
	if (strlen((char *)renderer) > strlen((char *)vendor) &&
	    memcmp(renderer, vendor, strlen((char *)vendor)) == 0) {
		/* yeah it matches, skip copying the vendor over */
		goto copyRenderer;
	}

	/* it does not, copy the vendor */
	strcpy(gpuNameStr, (char *)vendor);
	strcat(gpuNameStr, " ");

copyRenderer:
	/* step 2: clean up the renderer string
	 * it could often be something like:
	 *
	 * llvmpipe (LLVM 19.1.7, 256 bits)
	 * AMD Radeon Graphics (radeonsi, renoir, ACO, DRM 3.61, 6.14.2-arch1-1)
	 *
	 * Technically there is some useful info in there, but
	 * more often than not we don't care.
	 * Perhaps it could be useful to extract the GPU code name,
	 * like 'renoir' if present, but probably more effort
	 * than it's worth.
	 */
	chrptr = strchr((char *)renderer, '(');
	if (chrptr && (chrptr - 1) >= (char *)renderer)
		*(chrptr - 1) = '\0';

	/* copy it over */
	strcat(gpuNameStr, (char *)renderer);

	/* done manipulating the string, go copy it over to plat_info */
	PLAT_Info.gpu = malloc(strlen(gpuNameStr) + 1);
	strcpy(PLAT_Info.gpu, gpuNameStr);

	/* success!  set the callback for cleanup on exit */
	G_State.exitCallbacks.gfx = LINUX_GfxCleanupGLX;
	return 0;
}
