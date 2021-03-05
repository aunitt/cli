
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "io.h"
#include "debug.h"

static Output *out = 0;

void log_open()
{
    out = fopen_debug();
}

void log_close()
{
    fclose(out);
    out = 0;
}

void log_print(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    ovprintf(out, fmt, va);

    va_end(va);
}

void log_die()
{
    log_print("FATAL %s", "");
    exit(-1);
}

//  FIN
