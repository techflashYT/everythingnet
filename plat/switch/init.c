#include <stdio.h>
#include <unistd.h>
#include <switch.h>
#include <evrnet/plat.h>
#include <evrnet/plat/switch.h>
#include <evrnet/state.h>

PadState SWITCH_GamePadState;
int PLAT_Init(int argc, char *argv[]) {
	/* init text console, eventually swap to OpenGL */
	consoleInit(NULL);

	/* 1 controller */
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);

	/* initialize the controller */
	padInitializeDefault(&SWITCH_GamePadState);

	/* set up exit path */
	STATE_Init();
	G_State.exitCallbacks.exit = SWITCH_Exit;

	/* so we can gather info */
	setsysInitialize();

	/* gather console info */
	SWITCH_GatherInfo();
	return 0;
}
