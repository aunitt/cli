
#include <gtest/gtest.h>

#include "debug.h"
#include "cli.h"

static int got_action = false;

static void action_handler(CLI *cli, CliCommand *cmd)
{
    got_action = true;
}

static char obuff[1024];
static int obuff_idx = 0;

static void cli_reset()
{
    obuff_idx = 0;
    obuff[0] = '\0';
}

static void cli_puts(const char *s)
{
    //  Save output
    const int len = strlen(s);
    ASSERT_TRUE((len + obuff_idx + 1) < sizeof(obuff));

    strcat(obuff, s);
    obuff_idx += len;

    fprintf(stderr, "%s", s);
}

static void cli_send(CLI *cli, const char* s)
{
    for (; *s; s++)
    {
        cli_process(cli, *s);
    }
}

    /*
     *
     */

static CLI cli = {
    .output = cli_puts,
    .prompt = "> ",
    .eol = "\r\n",
};

TEST(CliGroup, Create)
{
    static CliCommand action = {
        .cmd = "help",
        .handler = action_handler,
        .help = "help text",
    };

    got_action = false;
    cli_init(& cli, 64);
    cli_register(& cli, & action);

    // Check that chars are stored in the buffer
    EXPECT_STREQ("", cli.buff);
    cli_close(& cli);
}

static void cli_die(CLI *cli, CliCommand *cmd)
{
    ASSERT(false);
}

TEST(CliGroup, Help)
{
#define HELP0 "one line of text" 
#define HELP1 "another help line"
#define HELP2 "some more help" 

    static CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = HELP0,
    };
    static CliCommand a1 = {
        .cmd = "anything",
        .handler = cli_die,
        .help = HELP1,
    };
    static CliCommand a2 = {
        .cmd = "another",
        .handler = cli_die,
        .help = HELP2,
    };

    got_action = false;
    cli_init(& cli, 64);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);
    cli_register(& cli, & a2);

    cli_reset();
    cli_send(& cli, "help\r\n");

    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    // Check the output
    EXPECT_STREQ("help\n" HELP0 "\r\n" HELP1 "\r\n" HELP2 "\r\n> ", obuff);

    cli_reset();

    cli_close(& cli);
}

    /*
     *
     */
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

