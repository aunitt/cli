
#undef CPPUTEST_USE_MEM_LEAK_DETECTION

#include "CppUTest/TestHarness.h"

#include "cli.h"

TEST_GROUP(CliGroup)
{
};

static int got_action = false;

void action_handler(CLI *cli, CliCommand *cmd)
{
    got_action = true;
}

void cli_puts(const char *s)
{
    printf("%s", s);
}

TEST(CliGroup, FirstTest)
{
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();

    static CliCommand action = {
        .cmd = "help",
        .handler = action_handler,
        .help = "help text",
    };

    CLI cli = {
        .output = cli_puts,
        .prompt = "> ",
    };

    got_action = false;
    cli_init(& cli, 64);
    cli_register(& cli, & action);

    // Check that chars are stored in the buffer
    STRCMP_EQUAL("", cli.buff);
    cli_process(& cli, 'h');
    STRCMP_EQUAL("h", cli.buff);
    cli_process(& cli, 'e');
    STRCMP_EQUAL("he", cli.buff);
    cli_process(& cli, 'l');
    STRCMP_EQUAL("hel", cli.buff);
    cli_process(& cli, 'p');
    STRCMP_EQUAL("help", cli.buff);

    // The last char should execute the command
    cli_process(& cli, '\n');
    CHECK(got_action);

    // Buffer should be cleared again
    STRCMP_EQUAL("", cli.buff);
    cli_close(& cli);
}

//  FIN
