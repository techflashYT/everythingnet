#include <unistd.h>
#include <stdlib.h>

#include <switch.h>

void __attribute__((noreturn)) SWITCH_Exit(int status) {
	/* flush the console to the display,
	 * and give the user a few seconds to
	 * see it.
	 */
	consoleUpdate(NULL);
	sleep(5);

	/* clean up resources used by the console
	 * (actually bad things will happen if
	 *  we don't do this, so we should)
	 */
	consoleExit(NULL);

	/* buh bye! */
	exit(status);
}
