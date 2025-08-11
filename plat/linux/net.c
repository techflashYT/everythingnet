/*
 * EverythingNet - Linux Platform Specific Features - Networking Support
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include <evrnet/state.h>
#include <evrnet/net.h>
#include <evrnet/netType.h>
#include <evrnet/nodeType.h>

static int mcastSock, bcastSock;
static struct pollfd bcastSocketPollFd, mcastSocketPollFd;
static struct sockaddr_in bcastAddr[CONFIG_NET_MAX_INT];
static struct sockaddr_in mcastAddr, anyAddr;
static uint32_t addrlen;
static int knownIfaces;


static void addIface(const char *ipStr, int num, uint32_t bcastMask) {
	int i;
	char netMaskStr[4];
	for (i = 0; ; i++) {
		if (bcastMask & (1 << i) || i >= 32)
			break;
	}
	printf("adding iface %d with ip %s/%d\n", num, ipStr, i);

	/* set up the address */
	bcastAddr[num].sin_family = AF_INET;
	inet_pton(AF_INET, ipStr, &bcastAddr[num].sin_addr);
	bcastAddr[num].sin_port = htons(EVRNET_BCAST_PORT);
}

int LINUX_NetInit(void) {
	struct ifaddrs *ifaces, *ifacesHead;
	int ret, family, flags, option = 1, option2 = 0;
	uint32_t address, bcastMask;
	char host[NI_MAXHOST];

	/* clean up all state, just in case; networking can be fiddly */
	addrlen = sizeof(struct sockaddr_in);
	bcastSock = 0, mcastSock = 0;
	memset(&bcastSocketPollFd, 0, sizeof(struct pollfd));
	memset(&mcastSocketPollFd, 0, sizeof(struct pollfd));
	memset(bcastAddr, 0, addrlen * CONFIG_NET_MAX_INT);
	memset(&mcastAddr, 0, addrlen);
	memset(&anyAddr, 0, addrlen);
	knownIfaces = 0;

	/* make the socket */
	bcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (bcastSock < 0) {
		perror("socket");
		APP_CleanupAndExit(1);
	}

	/* set SO_REUSEADDR so that it isn't weird */
	ret = setsockopt(bcastSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (ret < 0) {
		perror("setsockopt");
		APP_CleanupAndExit(1);
	}

	/* let us write to broadcast */
	ret = setsockopt(bcastSock, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option));
	if (ret < 0) {
		perror("setsockopt");
		APP_CleanupAndExit(1);
	}

	/* bind the socket to any address */
	anyAddr.sin_family = AF_INET;
	anyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	anyAddr.sin_port = htons(EVRNET_BCAST_PORT);

	ret = bind(bcastSock, (struct sockaddr*)&anyAddr, addrlen);
	if (ret < 0) {
		perror("bind");
		APP_CleanupAndExit(1);
	}

	/* set it into nonblocking mode */
	flags = fcntl(bcastSock, F_GETFL, 0);
	fcntl(bcastSock, F_SETFL, flags | O_NONBLOCK);


	/* set up pollfd */
	bcastSocketPollFd.fd = bcastSock;
	bcastSocketPollFd.events = POLLIN;

	/* loop over every interface, and see if it has an IP.
	 * if it has an IP (we can talk on it), add it with addIface().
	 */
	ret = getifaddrs(&ifaces);
	if (ret < 0) {
		perror("getifaddrs");
		APP_CleanupAndExit(1);
	}

	/* maintain head of linked list so we can free it later */
	ifacesHead = ifaces;
	while (ifaces) {
		if (knownIfaces >= CONFIG_NET_MAX_INT) {
			fprintf(stderr, "Exceeded maximum number of broadcast interfaces (%d)!  Rebuild with a larger CONFIG_NET_MAX_INT, or use less interfaces.\n", CONFIG_NET_MAX_INT);
			break;
		}

		if (!ifaces->ifa_addr)
			goto out; /* no address */

		family = ifaces->ifa_addr->sa_family;

		if (family != AF_INET)
			goto out; /* no ipv4 addr */

		if (strcmp(ifaces->ifa_name, "lo") == 0)
			goto out; /* don't care about loopback */


		/* set all bits outside of the netmask, in
		 * order to get the broadcast address for
		 * this network
		 */
		bcastMask = ((struct sockaddr_in *)ifaces->ifa_netmask)->sin_addr.s_addr;
		bcastMask = ~bcastMask;
		((struct sockaddr_in *)ifaces->ifa_addr)->sin_addr.s_addr |= bcastMask;

		/* get IP */
		ret = getnameinfo(ifaces->ifa_addr, addrlen,
				host, NI_MAXHOST,
				NULL, 0, NI_NUMERICHOST);

		if (ret) {
			fprintf(stderr, "getnameinfo failed on interface %s: %s; continuing without it\n", ifaces->ifa_name, strerror(errno));
			continue;
		}

		/* got a good interface, add it */
		addIface(host, knownIfaces, bcastMask);
		knownIfaces++;
out:
		ifaces = ifaces->ifa_next;
	}
	freeifaddrs(ifacesHead);

	/* now set up multicast */
	/* make the socket */
	mcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mcastSock < 0) {
		perror("socket");
		APP_CleanupAndExit(1);
	}

	/* set SO_REUSEADDR so that it isn't weird */
	ret = setsockopt(mcastSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (ret < 0) {
		perror("setsockopt");
		APP_CleanupAndExit(1);
	}

	/* set up the address */
	mcastAddr.sin_family = AF_INET;
	mcastAddr.sin_addr.s_addr = inet_addr(EVRNET_MCAST_ADDR);
	mcastAddr.sin_port = htons(EVRNET_MCAST_PORT);

	/* disable loopback */
	ret = setsockopt(mcastSock, IPPROTO_IP, IP_MULTICAST_LOOP, &option2, sizeof(option2));
	if (ret < 0) {
		perror("setsockopt");
		APP_CleanupAndExit(1);
	}

	/* set up pollfd */
	mcastSocketPollFd.fd = mcastSock;
	mcastSocketPollFd.events = POLLIN;

	NET_Init();
	return 0;
}

int PLAT_NetCheckBcastData(evrnet_bcast_msg_t *msg) {
	int ret, i;

	ret = poll(&bcastSocketPollFd, 1, 0);
	if (ret < 0) {
		perror("poll");
		return -1; /* error */
	}

	if (ret <= 0 || !(bcastSocketPollFd.revents & POLLIN))
		goto mcast; /* no data */

	for (i = 0; i < knownIfaces; i++) {
		ret = recvfrom(bcastSock, msg, CONFIG_NET_MAX_PKT_KB * 1024,
			0, NULL, NULL);

		if (ret <= 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
			continue; /* no data from that interface */

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
			continue; /* invalid magic */

		if (ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t)) != ret) {
			fprintf(stderr,
				"Malformed (or malicious?) packet received, "
				"reported length (%d) != received (%d).  "
				"Malicious packet attempting to buffer-overflow, "
				"or just corruption on flakey network?  Ignoring.\n",
				(uint32_t)(ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t))),
				ret
			);
			continue; /* length mismatch */
		}

		return 1; /* new message */
	}

mcast:
	ret = poll(&mcastSocketPollFd, 1, 0);
	if (ret < 0) {
		perror("poll");
		return -1; /* error */
	}

	if (ret <= 0 || !(mcastSocketPollFd.revents & POLLIN))
		return 0; /* no data */

	ret = recvfrom(mcastSock, msg, CONFIG_NET_MAX_PKT_KB * 1024,
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
		return 0; /* got data, but none were valid */
	}

	return 1; /* new message */
}

int PLAT_NetCheckMcastData(evrnet_bcast_msg_t *msg) {
	return 0;
}

int PLAT_NetDoBroadcast(evrnet_bcast_msg_t *msg) {
	int ret, i;
	size_t size = ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t));
	for (i = 0; i < knownIfaces; i++) {
		ret = sendto(bcastSock, msg, size, 0, (struct sockaddr*)&bcastAddr[i], addrlen);
		if (ret < 0) {
			perror("sendto");
			return -1;
		}
	}

	return 0;
}

int PLAT_NetDoMulticast(evrnet_bcast_msg_t *msg) {
	int ret;
	size_t size = ntohl(msg->nodeList.len) + (sizeof(evrnet_bcast_msg_t) - sizeof(nodeList_t));

	ret = sendto(mcastSock, msg, size, 0, (struct sockaddr*)&mcastAddr, addrlen);
	if (ret < 0) {
		perror("sendto");
		return -1;
	}

	return 0;
}

