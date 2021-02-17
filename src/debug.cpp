
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

#include "debug.h"

static int severity(Severity s)
{
    switch (s)
    {
        case SEVERITY_DEBUG :   return LOG_DEBUG;
        case SEVERITY_INFO  :   return LOG_INFO;
        case SEVERITY_WARN  :   return LOG_WARNING;
        case SEVERITY_ERROR :   return LOG_ERR;
        case SEVERITY_FATAL :   return LOG_CRIT;
        default :   ASSERT(false);
    }
    return 0;
}

void log_print(Severity s, const char *fmt, ...)
{
    char buff[1024];

    va_list va;
    va_start(va, fmt);

    vsnprintf(buff, sizeof(buff), fmt, va);

    va_end(va);

    syslog(severity(s), "%s", buff);
}

void log_die()
{
    ALOG_ERROR("%s", "");
    exit(-1);
}

//  FIN
