    /*
     *
     */

#include "cli.h"

#include "io.h"

class IO
{
    FILE *memio = 0;
    char *mem_buf = 0;
    size_t mem_size = 0;

    CliOutput out;

public:

    CliOutput *open();
    void close();
    void reset();
    char* get();
};

extern IO io;

//  FIN
