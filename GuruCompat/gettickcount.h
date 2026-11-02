#if !defined __GETTICKCOUNT_H__
#define __GETTICKCOUNT_H__

#include <GuruCompat.h>

#ifdef __GURULINUX__

#include <stdint.h>
#include <sys/time.h>

#ifdef __cplusplus
	extern "C" {
#endif
	uint64_t GetTickCount();
#ifdef __cplusplus
	}
#endif

#endif

#endif
