#include "evrnet/plat.h"
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
	uint8_t *e;
	/* generous buffer to make our local entry */
	NODE_NodeList = malloc(sizeof(nodeList_t) + 4096);
	if (!NODE_NodeList) {
		perror("malloc");
		APP_CleanupAndExit(1);
	}

	NODE_NodeList->len = sizeof(nodeList_t);
	NODE_NodeList->version = EVRNET_NODELIST_V1;

	/* set up local entry */
	e = NODE_NodeList->entries;
	*ENTRY_NUM_IP(e) = 0;
	strcpy(ENTRY_NODE_NAME(e), NODE_LocalName);
	ENTRY_NODE_UUID(e)[0] = NODE_LocalUUID[0];
	ENTRY_NODE_UUID(e)[1] = NODE_LocalUUID[1];
	strcpy(ENTRY_NODE_EVRNET_VER(e), "somever");
	*ENTRY_SIZE(e) = ENTRY_CALC_SIZE(e);
	/* clear out the next few bytes */
	memset(ENTRY_NEXT(e), 0, 8);

	NODE_NodeList->len += *ENTRY_SIZE(e) + 8;
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
		e = ALIGN4(e);

		for (i = 0; i < numIPs; i++) {
			inet_ntop(AF_INET, e, ip, INET_ADDRSTRLEN);
			printf("IP address: %s\n", ip);
			e += 4;
		}
		e++;

		/* alignment */
		e = ALIGN8(e);

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
		e = ALIGN4(e);

		e += sizeof(platInfo_t);

		/* alignment */
		e = ALIGN8(e);
	}
}
