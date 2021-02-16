
#include "CppUTest/TestHarness.h"

#include "cli.h"

TEST_GROUP(CliGroup)
{
};

void action_handler(CliCommand *cmd, int argc, char **argv)
{
}

TEST(CliGroup, FirstTest)
{
    static CliCommand action = {
        .cmd = "hello",
        .handler = action_handler,
        .help = "help",
    };

    cli_register(& action);
}

//  FIN
