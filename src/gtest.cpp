
#include <gtest/gtest.h>

#include "cli.h"

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
    EXPECT_STREQ("", cli.buff);
    cli_process(& cli, 'h');
    EXPECT_STREQ("h", cli.buff);
    cli_process(& cli, 'e');
    EXPECT_STREQ("he", cli.buff);
    cli_process(& cli, 'l');
    EXPECT_STREQ("hel", cli.buff);
    cli_process(& cli, 'p');
    EXPECT_STREQ("help", cli.buff);

    // The last char should execute the command
    cli_process(& cli, '\n');
    EXPECT_TRUE(got_action);

    // Buffer should be cleared again
    EXPECT_STREQ("", cli.buff);
    cli_close(& cli);
}

    /*
     *
     */
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

