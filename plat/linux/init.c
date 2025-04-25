#include <stdlib.h>

#include <evrnet/state.h>
#include <evrnet/plat/linux.h>
#include <evrnet/plat/linux_gfx.h>
int PLAT_Init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	STATE_Init();
	G_State.exitCallbacks.exit = exit;

	LINUX_GatherInfo();
	LINUX_GfxInit();

	/* Nothing important to do on Linux yet, just return success.
	 * When this project is more complete, we'll probably
	 * want to do things like initializing an X11/Wayland
	 * window here.
	 */
	return 0;
}
