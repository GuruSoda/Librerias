#if !defined __GURUBASE64__
	#define	__GURUBASE64__

#include <ctype.h>
#include <stddef.h>

#ifdef __cplusplus
	extern "C" {
#endif

#define 	Assert(Cond)   if (!(Cond)) abort()

int 	base64_encode (char const *src, size_t srclength, char *target, size_t targsize);
int 	base64_decode (char const *src, char *target, size_t targsize);

#ifdef __cplusplus
	}
#endif
 
#endif
