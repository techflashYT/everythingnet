/*
 * EverythingNet - Microsoft Windows Platform Specific Features - Windows NT 4.0 - Networking
 * Copyright (C) 2025 Techflash
 */

#include <stdint.h>
#include <windows.h>
#include <winsock.h>

#include <evrnet/net.h>
#include <evrnet/node.h>

int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg) {
	return 0;
}

int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg) {
	return 0;
}
