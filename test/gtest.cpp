
#include <stdio.h>

#include <gtest/gtest.h>

#include <cli_debug.h>
#include "io.h"
#include "cli.h"
#include "test_io.h"

extern CLI cli;

    /*
     *
     */
 
int main(int argc, char **argv) {
    log_open();

    cli.output = io.open();

    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();

    io.close();
    log_close();
    return result;
}

