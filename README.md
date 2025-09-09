# EverythingNet - Your applications, on everything
[title not final]

EverythingNet is a utility that lets you mesh together many devices, and display applications from any (normal) device, on any device.

## Basic goals
- Decentralized mesh of all devices on your network
- Allows you to drag application windows from any device¹ to any device²
- Allows input from any device³  ⁴ to affect the global state
- One global shared state, with one cursor across every machine.  Other input devices (gamepads, keyboards, etc) follow the cursor.  The machine that the cursor is on is known as the "target machine"
- Extremely resilient zero-configuration networking
  - Broadcast address based machine discovery
    - For platforms that do not support operating on the broadcast address (for example, the Nintendo Switch under libnx) can input the IP address of any other machine in the network, and that machine will act as a proxy, advertising the broadcast-less machine to the rest of the network, and forwarding the state of the network back to the broadcast-less machine
  - Incredibly fault-tolerant
    - If any machine cannot reach another machine, it will be able to jump through any machine that can reach it (through any number of hops, if necessary)
      - Through principle of this, any machine that is connected to 2 separate networks can acts as a gateway between those 2, even if neither network can directly access machines on the other.  They will jump through the one that's on both
      - Through principle of this, any machine that is behind a firewall, but can be accessed by at least one node, is still accessible
    - If a machine is found via broadcast / broadcast-proxy, but is unreachable (perhaps behind a firewall), the rest of the network gracefully falls back without it, and alerts the user to it's unreachability
  - Automatic negotiation of quality between every node and every other node, to optimize for a good experience no matter what the underlying network conditions look like
- **It should always "just work", by any means necessary, if it is possible to do so in the slightest**

¹ Any multitasking device that is capable of running traditional apps (Windows, Mac, Linux, BSD, etc)  
² Any device with a screen  
³ Any device that is capable of providing input  
⁴ Some devices may not support relative input (such as a mouse, trackpad, or joystick).  In this case, whenever it's input method is actively in use, it will be restricted to the contents of that device's view only.  (Examples of this type of device are: touchscreen based devices like tablets, as well as pointer based devices like the Nintendo Wii)  

## Supported platforms
The following platforms are planned to be supported for the initial release:
- **Full Support** - Can host apps, provide input, and display apps
  - Windows (unsure about minimum version, will need to see as I go), x86, x64, ARM64, ARM32 (Surface RT / 2), Itanium, PowerPC, MIPS, Alpha
  - MacOS for i386 (untested), x86\_64, ARM64/Apple Silicon (untested), 32-bit PowerPC (tested on G4, *probably* runs on G3 and 60x), 64-bit PowerPC (G5) (untested).
  - Linux (any version that you can get it to build for), for any supported architecture
  - BSDs (any version that you can get it to build for), for any supported architecture
- **Input/View Only** - These platforms can't host apps due to software limitations
  - DOS or compatible (MS-DOS, DR-DOS, PC-DOS, FreeDOS, etc) with a 386 or newer CPU -- see Kconfig help text for full requirements
  - Nintendo GameCube (requires Broadband Adapter)
  - Nintendo Wii (Wii Remote cursor limited to the area of the console's TV; other control methods such as Nunchuck, Classic Controller, or GameCube controller offer full range)
  - Nintendo Wii U (GamePad and TV screen are seperate; GamePad touchscreen is limited to the area of itself; Wii Remote cursor limited to the area of the TV; GamePad joysticks, Pro Controller, Wii Remote Nunchuck, and Wii Remote Classic Controller offer full range)
  - Nintendo Switch (Touchscreen limited to it's own screen; controllers offer full range; likely will not be able to dynamically switch between docked/undocked unless entire 1080p space is reserved upfront)
  - PlayStation 3
  - Android
The following platforms are planned for support later on:
- Full Support
  - More obscure multitasking OSes for generic hardware
- Input/View Only
  - PlayStation 2
  - PlayStation 4
  - Xbox 360
  - Xbox One / Series (Dev Mode)
  - iOS / iPadOS
- View Only (no input, no apps)
  - Apple tvOS
  - Tizen (Samsung Smart TVs)

The following platforms will not yet be supported due to legal concerns:
- Nintendo 64 (primary method of interacting with the graphics is through copyrighted microcode.  If a fully FOSS one is available, and halfway decent, this could change)
- Original Xbox (primary method of developing homebrew is to use leaked SDKs last I checked.  If a fully FOSS one is available, and halfway decent, this could change)


## Design
This project is built to be *incredibly* easy to port (see `PORTING.md`), and incredibly easy to build these ports.  It uses the popular Kbuild/Kconfig system, which was created for configuring and building the Linux kernel.  The specific version used in this project originates from Busybox, as it was easier to hammer into this project's goals, vs Linux's.

## Licensing
This project is licensed under the GNU GPL, version 2.
