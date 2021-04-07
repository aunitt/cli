
#include <stdlib.h>

#include <cli_debug.h>

#include "test_io.h"

    /*
     *
     */

static int io_fprintf(void *ctx, const char *fmt, va_list va)
{
    ASSERT(ctx);
    FILE *f = (FILE*) ctx;
    return vfprintf(f, fmt, va);
}

    /*
     *
     */

CliOutput *IO::open()
{
    memio = open_memstream(& mem_buf, & mem_size);

    out.ctx = memio;
    out.fprintf = io_fprintf;

    return & out;
}

void IO::close()
{
    fclose(memio);
    free(mem_buf);
    out.ctx = 0;
    out.fprintf = 0;
}

void IO::reset()
{
    // reset to start of buffer
    fseek(memio, 0, SEEK_SET);
    // ensure the buffer is '\0' terminated
    fwrite("\0", 1, 1, memio);
    fseek(memio, 0, SEEK_SET);
}

char* IO::get()
{
    // Ensures that all data is written to buffer
    // terminate it with a '\0' after the last char
    fwrite("\0", 1, 1, memio);
    fseek(memio, -1, SEEK_CUR);
    fflush(memio);
    return mem_buf;
}

IO io;

//  FIN
