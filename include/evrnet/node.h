#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>
#include <evrnet/plat.h>
#include <evrnet/net.h>
#include <evrnet/nodeType.h>

#define EVRNET_NODELIST_V1 0x00000001

/* Local UUID */
extern uint64_t NODE_LocalUUID[2];

/* Local name */
extern char *NODE_LocalName;

/* Node list */
extern nodeList_t *NODE_NodeList;

/* Check for any new nodes in a broadcast message */
extern void NODE_CheckForNewNodes(evrnet_bcast_msg_t *msg);

/* Endianness handling functions
 * To convert the list from native endianness to Big Endian,
 * (to be transferred over the network), or from Big Endian
 * back to native endianness
 */
#ifdef EVRNET_CPU_IS_BE
/* These do nothing on BE, since native endianness is already
 * what we want to transfer
 */
#define NODE_ListToNative (void)0
#define NODE_ListToBE (void)0
#else
extern void NODE_ListToNative(void);
extern void NODE_ListToBE(void);
#endif

#endif
