#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <evrnet/nodeType.h>
#include <evrnet/node.h>
#include <evrnet/state.h>
#include <evrnet/net.h>
#include <sys/socket.h>
nodeList_t *NODE_NodeList;
char NODE_LocalName[64];
uint64_t NODE_LocalUUID[2];

void NODE_Init(void) {
	NODE_NodeList = malloc(sizeof(nodeList_t));
	if (!NODE_NodeList) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	NODE_NodeList->len = sizeof(nodeList_t);
	NODE_NodeList->version = EVRNET_NODELIST_V1;
}

#ifdef EVRNET_CPU_IS_LE
void NODE_ListToBE(void) {

}

void NODE_ListToNative(void) {

}
#endif

void NODE_CheckForNewNodes(evrnet_bcast_msg_t *msg) {
	nodeList_t *nl;
	uint8_t *e;
	int i = 0, numIPs;
	char nameTemp[64];
	char evrnetVer[64];
	char ip[INET_ADDRSTRLEN];
	puts("trying to parse nodes");
	nl = &msg->nodeList;
	e = nl->entries;
	memset(nameTemp, 0, sizeof(nameTemp));
	memset(evrnetVer, 0, sizeof(evrnetVer));

	/* 0 size = end of list */
	while (*(uint16_t *)e != 0) {
		printf("entry size: %u\n", *((uint16_t *)e));
		e += 2;
		printf("num IPS: %u\n", *e);
		numIPs = *e;
		e++;
		while (*e) {
			nameTemp[i] = *e;
			e++;
			i++;
		}
		e++;
		printf("node name: %s\n", nameTemp);

		/* alignment */
		e += (uintptr_t)e % 4;

		for (i = 0; i < numIPs; i++) {
			inet_ntop(AF_INET, e, ip, INET_ADDRSTRLEN);
			printf("IP address: %s\n", ip);
			e += 4;
		}
		e++;

		/* alignment */
		e += (uintptr_t)e % 8;

		printf("UUID 0: 0x%16lX\n", *((uint64_t *)e));
		e += 8;
		printf("UUID 1: 0x%16lX\n", *((uint64_t *)e));
		e += 8;

		i = 0;
		while (*e) {
			evrnetVer[i] = *e;
			e++;
			i++;
		}
		e++;

		printf("everythingnet version: %s\n", evrnetVer);

		/* alignment */
		e += (uintptr_t)e % 4;

		/* TODO: PLAT_Info */


		/* alignment */
		e += (uintptr_t)e % 8;
	}
}
