
#if !defined(__IO_H__)

#define __IO_H__

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef FILE Output;

Output *fopen_debug();

int ovprintf(Output *, const char *fmt, va_list va);
int oprintf(Output *, const char *fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif  //  __IO_H__

//  FIN
