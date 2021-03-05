
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mutex.h"

#include "io.h"

#define UNUSED(x) ((x) = (x))

extern "C" {

Output fopen_debug()
{
    // TODO
    return 0;
}

int ovprintf(Output out, const char *fmt, va_list va)
{
    return vfprintf(out, fmt, va);
}

int oprintf(Output out, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    const int n = ovprintf(out, fmt, va);

    va_end(va);
    return n;
}

}   //  extern "C"

//  FIN
