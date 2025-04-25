#include <stddef.h>
#include <evrnet/state.h>

state_t G_State;

void STATE_Init() {
	G_State.exitCallbacks.snd  = NULL;
	G_State.exitCallbacks.gfx  = NULL;
	G_State.exitCallbacks.plat = NULL;
	G_State.exitCallbacks.exit = NULL;
}
