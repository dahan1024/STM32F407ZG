/*
 * types.h
 *
 *  Created on: 2019Äê5ÔÂ15ÈÕ
 *      Author: yanglinjun
 */

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#include <stdint.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#undef FAIL
#define FAIL -1

#undef SUCC
#define SUCC 0

#ifndef BOOL
//#define BOOL int
#define BOOL int32_t
#endif

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#undef NULL
#define NULL ((void *)0)

#if 0
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef		__s8		int8_t;
typedef		__s16		int16_t;
typedef		__s32		int32_t;

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;

#endif

typedef unsigned char BYTE;

typedef union {
	float f;
	uint32_t ui32;
} f_ui32_union_t;

typedef union {
	float f;
	uint8_t bytes[4];
} f_ui8_union_t;

typedef union {
	uint8_t bytes[4];
	uint32_t ui32;
} ui8_ui32_union_t;

#endif /* INCLUDE_TYPES_H_ */
