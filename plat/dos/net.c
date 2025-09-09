/*
 * EverythingNet - DOS Platform Specific Features - Networking
 * Copyright (C) 2025 Techflash
 */

#include <evrnet/netType.h>

int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg) {
	(void)msg;
	return 0;
}


int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg) {
	(void)msg;
	return -1;
}
