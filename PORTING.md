# Porting EverythingNet to new platforms

## Preequisites

### Required
- A functional C compiler for your platform (preferably a GCC-alike, unless you want to do a significant amount of tweaks to the build system)
- A functional Linker for your platform (preferably a binutils ld-alike, unless you want to do a significant amount of tweaks to the build system)
- A functional set of math helpers for your platform (usually provided by libgcc)
- A way to start the application on your device (e.g. a functional Linux userspace, U-Boot, being flashed directly to SPI ROM, etc)
- Device has some form of networking (Ethernet, RNDIS / USB, WiFi, Bluetooth PAN, heck even a UART/Serial port with PPP or SLIP would suffice)
- Device should have at least a few hundred KB of memory for a base configuration, or (4 * framebuffer size) + 512KB for one capable of display
- **(if acting as a host)** a method to capture the contents application windows, and hide them / move them off-screen

### Highly Recommended
- Some method of producing output, either wired to `putchar` via your libc, or implemented in-house with `SYS_putchar` via the internal libc.  This could go to, for example: UART, audio modem, framebuffer, your OS's stdout, etc.
- A functional libc (a barebones stubbed one is provided but not very good), providing at least the following functions (some may be stubs):
  - putchar
  - puts
  - printf (a good freestanding one is provided for convinience)
  - mem{cpy,cmp,set} and str{cpy,ncpy,cmp,ncmp,cat,ncat}
- Device has input and/or display (it could only serve as purely a proxy node if it has neither - which is a supported configuration, just not a very useful one)
- Knowledge about your device's static hardware and software configuration, **OR** a way to detect your device's dynamic hardware and software configuration at runtime

## Porting
"foo" will be used throughout this guide as a placeholder for the name of your platform.  Specifics of how Kbuild/Kconfig works are not covered here, and it is expected that you have a basic understanding of them already, or can figure it out by looking at how it is done in other platforms.

1. Define a new platform in `plat/Kconfig`, and add a `source plat/foo/Kconfig` at the bottom.
2. Add it in `plat/Makefile`.
3. Add any necessary device-specific configuration parameters under `plat/foo/Kconfig`
4. Define `PLAT_Init()` and `PLAT_Info`.  These are by convention in `init.c` and `info.c` respectively, though if you have a good reason to, they can be in any file.  Note that `PLAT_Init()` is expected to perform all initialization tasks - see the `PLAT_Init` and `PLAT_Info` sections of the documentation (under the `Documentation` directory) for more information on the roles of each.
5. Define the exit callbacks (preferably in `PLAT_Init`)
6. Define other platform-sepcific callbacks (e.g. `PLAT_FlushOutput`) if necessary
7. For baremetal platforms that lack a proper libc, you'll probably want to define some of the internal-libc stubs for your platform, so you can get output, allocate memory, etc.
8. Add any C source files you created under `plat/foo` to `plat/foo/Makefile`.
9. If necessary for your platform, define any necessary CFLAGS, LDFLAGS, LDLIBS, post-link steps, etc, under `Makefile.flags`.  If your platform is one specific (type of) device, and can only use one specific toolchain (e.g. Wii/GameCube with devkitPPC, Switch with devkitA64), you may force said toolchain in `Makefile.flags` as well.
11. Define `PLAT_NetDoBroadcast` and `PLAT_NetCheckBcastData`.  These can be stubs for an initial port.  It's convention to also have a `FOO_NetInit` that you call under `PLAT_Init`, though this may or may not be necessary for your platform.
12. EverythingNet should now compile and build, at least in a no-functionality mode.  That should be all of the necessary platform glue to get a binary that starts.
13. [TBD - Graphics?  Audio? etc]

### Optional Platform Hooks

Platforms can optionally define the following hooks if they require custom behavior:

- `void PLAT_FlushOutput(void)`: Called to flush any platform-specific output buffers (e.g. Switch/libnx `consoleUpdate()`)



