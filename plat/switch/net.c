/*
 * EverythingNet - Switch Platform Specific Features - Networking Support
 * Copyright (C) 2025 Techflash
 *
 * Based on plat/linux/net.c:
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>

#include <evrnet/state.h>
#include <evrnet/net.h>
#include <evrnet/netType.h>
#include <evrnet/nodeType.h>
#include <evrnet/plat/switch_perror.h>

#include <switch.h>

static int bcastSock;
static struct pollfd socketPollFd;
static struct sockaddr_in bcastAddr;
static struct sockaddr_in anyAddr;
static uint32_t addrlen;


int SWITCH_NetInit(void) {
	int ret, family, flags, i, option = 1;
	char netMaskStr[4], *host;
	uint32_t address, bcastMask;

	/* clean up all state, just in case; networking can be fiddly */
	addrlen = sizeof(struct sockaddr_in);
	bcastSock = 0;
	memset(&socketPollFd, 0, sizeof(struct pollfd));
	memset(&bcastAddr, 0, addrlen);

	/* init the libnx socket system */
	socketInitializeDefault();

	/* make the socket */
	bcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (bcastSock < 0) {
		perror("socket");
		PLAT_FlushOutput();
		APP_CleanupAndExit(1);
	}

	/* set SO_REUSEADDR so that it isn't weird */
	ret = setsockopt(bcastSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (ret < 0) {
		perror("setsockopt");
		PLAT_FlushOutput();
		APP_CleanupAndExit(1);
	}

	/* let us write to broadcast */
	ret = setsockopt(bcastSock, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option));
	if (ret < 0) {
		perror("setsockopt");
		PLAT_FlushOutput();
		APP_CleanupAndExit(1);
	}

	/* bind the socket to any address */
	anyAddr.sin_family = AF_INET;
	anyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	anyAddr.sin_port = htons(EVRNET_BCAST_PORT);

	ret = bind(bcastSock, (struct sockaddr*)&anyAddr, addrlen);
	if (ret < 0) {
		perror("bind");
		PLAT_FlushOutput();
		APP_CleanupAndExit(1);
	}

	/* set it into nonblocking mode */
	flags = fcntl(bcastSock, F_GETFL, 0);
	fcntl(bcastSock, F_SETFL, flags | O_NONBLOCK);

	/* set up pollfd */
	socketPollFd.fd = bcastSock;
	socketPollFd.events = POLLIN;

	/* set up the address */
	bcastAddr.sin_family = AF_INET;
	puts("SWITCH: Unable to determine netmask on this platform: guessing /24");
	PLAT_FlushOutput();
	/* Switch runs in little endian, so need to set the MSBs in order to set the least significant address bits */
	bcastMask = 0xff000000;

	bcastAddr.sin_addr.s_addr = gethostid() | bcastMask;
	bcastAddr.sin_port = htons(EVRNET_BCAST_PORT);

	host = inet_ntoa(bcastAddr.sin_addr);

	for (i = 0; ; i++) {
	if (bcastMask & (1 << i) || i >= 32)
		break;
	}
	printf("using broadcast ip %s/%d\n", host, i);
	PLAT_FlushOutput();

	NET_Init();
	return 0;
}

int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg) {
	int ret, i;

	ret = poll(&socketPollFd, 1, 0);
	if (ret < 0) {
		perror("poll");
		return -1; /* error */
	}

	if (ret <= 0 || !(socketPollFd.revents & POLLIN))
		return 0; /* no data */

	ret = recvfrom(bcastSock, msg, CONFIG_NET_MAX_PKT_KB * 1024,
		0, NULL, NULL);

	if (ret <= 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return 0; /* no data */

	else if (ret <= 0) {
		perror("recvfrom");
		return -1;
	}

	/* we don't byteswap anything here, even though
	 * we use the byteswapped values repeatedly.
	 * byteswapping will be done by the networking core
	 * after we return, the platform glue must always return
	 * the packet as-is.
	 */
	if (ntohl(msg->magic) != EVRNET_BCAST_MAGIC)
		return 0; /* invalid magic */

	if (ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t)) != ret) {
		fprintf(stderr,
			"Malformed (or malicious?) packet received, "
			"reported length (%d) != received (%d).  "
			"Malicious packet attempting to buffer-overflow, "
			"or just corruption on flakey network?  Ignoring.\n",
			(uint32_t)(ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t))),
			ret
		);
		return 0; /* length mismatch */
	}

	return 1; /* new message */
}

int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg) {
	int ret, i;
	ret = sendto(bcastSock, msg, ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t)), 0,
		(struct sockaddr*)&bcastAddr, addrlen);
	if (ret < 0) {
		perror("sendto");
		return -1;
	}

	return 0;
}
