#include <stdlib.h>
#include <evrnet/state.h>
int PLAT_Init(int argc, char *argv[]) {
	G_State.exitCallbacks.exit = exit;

	return 0;
}
