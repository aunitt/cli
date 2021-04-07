

#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>

#include <cli_mutex.h>

#include "io.h"

#define UNUSED(x) ((x) = (x))

typedef struct {
    Mutex *mutex;
}   DebugCookie;

static ssize_t debug_read(void *cookie, char *buf, size_t size)
{
    UNUSED(cookie);
    UNUSED(buf);
    UNUSED(size);
    return 0;
}

static ssize_t debug_write(void *cookie, const char *buf, size_t size)
{
    DebugCookie *dc = (DebugCookie*) cookie;

    Lock lock(dc->mutex);

    // TODO : the buff has no '\0' termination!
    char *s = strndup(buf, size);

    //syslog(LOG_DEBUG, "%s", s);
    printf("%s\r\n", s);

    free(s);

    return 0;
}

static int debug_seek(void *cookie, off64_t *offset, int whence)
{
    UNUSED(cookie);
    UNUSED(offset);
    UNUSED(whence);
    return 0;
}

static int debug_close(void *cookie)
{
    DebugCookie *dc = (DebugCookie*) cookie;
    delete dc->mutex;
    return 0;
}

static cookie_io_functions_t debug_cookie_fns {
    .read = debug_read,
    .write = debug_write,
    .seek = debug_seek,
    .close = debug_close,
};

static DebugCookie debug_cookie;

extern "C" {

FILE *fopen_debug()
{
    debug_cookie.mutex = Mutex::create();
    FILE *f = fopencookie(& debug_cookie, "w", debug_cookie_fns);

    setvbuf(f, 0, _IONBF, 0); // set unbuffered

    return f;
}

}   //  extern "C"

//  FIN
