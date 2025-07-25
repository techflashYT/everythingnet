#ifndef _NODETYPE_H
#define _NODETYPE_H

#include <stdint.h>

typedef struct {
	/* Entire node list size in bytes */
	uint32_t len;

	/* Node list version */
	uint32_t version;

	/* Node entry format:
	 * All 16, 32, or 64-bit integers will be stored in
	 * native endianness by default for use, but swapped to BE
	 * to be sent over the wire (will be recieved in BE).
	 *
	 * Any char[] is NULL terminated ASCII unless specified.
	 *
	 * 1.  u16, node entry size, for fast entry traversal
	 *       (avoids needing to parse the entire structure every time)
	 * 2.  u8, number of IP addresses in list
	 * 3.  char[], node name
	 * 4.  padding to 4 byte alignment
	 * 5.  array of u32s for IP addresses (see entry 1 for length)
	 * 6.  padding to 8 byte alignment
	 * 7.  u64[2], node UUID
	 * 8.  char[], EverythingNet version
	 * 9.  padding to 4 byte alignment
	 * 10. packed PLAT_Info
	 *     a. u32, cap
	 *     b. i32, memSz
	 *     c. char[], os
	 *     d. char[], arch
	 *     e. char[], cpu
	 *     f. char[], gpu
	 * 11. padding to 8 byte alignment
	 * 12. [end of entry, next entry follows immediately after]
	 */
	uint8_t entries[];
} __attribute__((packed)) nodeList_t;

#endif
