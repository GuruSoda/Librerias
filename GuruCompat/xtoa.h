#if !defined __XTOA__
#define __XTOA__

#include <GuruCompat.h>

#ifdef __GURULINUX__

#ifdef __cplusplus
        extern "C" {
#endif
 
void xtoa ( unsigned long val, char *buf, unsigned radix, int is_neg);
char* _itoa (int val, char *buf, int radix);
char* _ltoa (long val, char *buf, int radix);
char* _ultoa (unsigned long val, char *buf, int radix);

#ifdef __cplusplus
        }
#endif  

#endif

#endif
