/*
 * common.h
 *
 *  Created on: 2019Äê5ÔÂ15ÈÕ
 *      Author: yanglinjun
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <stdio.h>

#include <config.h>
#include <types.h>

/** \brief Error codes */
enum
{
    RC_OK			=  0,	/**< Success */
    RC_ERROR		= -1,	/**< Unspecified error */
	RC_INVAL		= -2,	/**< Invalid argument */
    RC_TIMEOUT		= -3	/**< Timeout */
};

#define UPDATE_TIME	(195312.5 * 1)

//#define DEBUG

/* debug usage
 * debug("Protect off %08lX ... %08lX\n",
 * 	(ulong)flash_addr, end_addr);
 * */

#ifdef	DEBUG
#define debug(fmt, args...)	printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif	/* DEBUG */

#ifdef DEBUG
# define _DEBUG 1
#else
# define _DEBUG 0
#endif


/*
 * An assertion is run-time check done in debug mode only. If DEBUG is not
 * defined then it is skipped. If DEBUG is defined and the assertion fails,
 * then it calls panic*( which may or may not reset/halt U-Boot (see
 * CONFIG_PANIC_HANG), It is hoped that all failing assertions are found
 * before release, and after release it is hoped that they don't matter. But
 * in any case these failing assertions cannot be fixed with a reset (which
 * may just do the same assertion again).
 */
//void __assert_fail(const char *assertion, const char *file, unsigned line,
//		   const char *function);
//#define assert(x) \
//	({ if (!(x) && _DEBUG) \
//		__assert_fail(#x, __FILE__, __LINE__, __func__); })
#define assert(x) do {									\
	if (!(x) && _DEBUG)									\
		printf("ASSERT ERROR: %s\nat %s:%d/%s()\n",		\
			#x, __FILE__, __LINE__, __func__);			\
} while (0)

/* error usage
 *
 * if (len < 0) {
 * 	error("Cannot export environment: errno = %d\n", errno);
 * 	goto done;
 * }
 * */

#define error(fmt, args...) do {					\
		printf("ERROR: " fmt "at %s:%d/%s()\n\n",		\
			##args, __FILE__, __LINE__, __func__);		\
} while (0)

#ifndef BUG
#define BUG() do { \
	printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __FUNCTION__); \
	panic("BUG!"); \
} while (0)
#define BUG_ON(condition) do { if (unlikely((condition)!=0)) BUG(); } while(0)
#endif /* BUG */

#endif /* INCLUDE_COMMON_H_ */
