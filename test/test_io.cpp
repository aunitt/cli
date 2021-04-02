
#include <stdlib.h>

#include "test_io.h"

    /*
     *
     */

FILE *IO::open()
{
    memio = open_memstream(& mem_buf, & mem_size);
    return memio;
}

void IO::close()
{
    fclose(memio);
    free(mem_buf);
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
