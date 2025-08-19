/*
 * EverythingNet - Common headers - Networking
 * Copyright (C) 2025 Techflash
 */

#ifndef _NET_H
#define _NET_H

#include <stdint.h>
#include <evrnet/nodeType.h>
#include <evrnet/plat.h>

#define EVRNET_BCAST_PORT 6000
#define EVRNET_MCAST_PORT 6000
#define EVRNET_MCAST_ADDR "239.0.0.1"
#define EVRNET_BCAST_MAGIC 0x92DCC748

/* Initialize the networking subsystem */
extern void NET_Init(void);

/* Handles receiving any packets from other nodes
 * across any broadcast interface, and transmitting
 * our own data every once in a while.
 *
 * Should be called every 500ms.
 * Will transmit data once every 15 calls.
 */
extern void NET_HandleBcast(void);

#endif
