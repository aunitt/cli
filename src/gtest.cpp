
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
    cli_init(& cli, 64, 0);
    cli_register(& cli, & action);

    // Check that chars are stored in the buffer
    EXPECT_STREQ("", cli.buff);
    cli_reset();
    cli_send(& cli, "help");
    EXPECT_STREQ("help", cli.buff);

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

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);
    cli_register(& cli, & a2);

    cli_reset();
    cli_send(& cli, "help\r\n");

    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    // Check the output
    EXPECT_STREQ("help\r\n" HELP0 "\r\n" HELP1 "\r\n" HELP2 "\r\n> ", obuff);

    cli_reset();

    cli_close(& cli);
}

TEST(CliGroup, Edit)
{
    static CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);

    cli_reset();
    cli_send(& cli, "heldx");
    EXPECT_STREQ("heldx", cli.buff);
    EXPECT_STREQ("heldx", obuff);

    cli_send(& cli, "\b");
    EXPECT_STREQ("held", cli.buff);
    EXPECT_STREQ("heldx\b \b", obuff);

    cli_send(& cli, "\b");
    EXPECT_STREQ("hel", cli.buff);
    EXPECT_STREQ("heldx\b \b\b \b", obuff);

    cli_send(& cli, "p\r\n");
    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_reset();

    cli_close(& cli);
}

static const char *ctx_text = "hello world";
static bool ctx_ran = false;

void check_ctx(CLI *cli, CliCommand *cmd)
{
    ctx_ran = true;
    EXPECT_EQ(cli->ctx, ctx_text);
}

TEST(CliGroup, Context)
{
    static CliCommand a0 = {
        .cmd = "help",
        .handler = check_ctx,
        .help = "help!",
    };

    cli_init(& cli, 64, (void*) ctx_text);
    cli_register(& cli, & a0);

    ctx_ran = false;
    cli_send(& cli, "help\r\n");
    EXPECT_TRUE(ctx_ran);

    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_reset();

    cli_close(& cli);
}

TEST(CliGroup, EmptyLine)
{
    static CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);

    cli_send(& cli, "\r\n");

    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_reset();

    cli_close(& cli);
}

TEST(CliGroup, OverflowLine)
{
    static CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
    };

    cli_init(& cli, 10, 0);
    cli_register(& cli, & a0);

    for (int i = 0; i < 10; i++)
    {
        cli_send(& cli, "x");
    }

    // Currently silenty ignores the too-long command
    EXPECT_STREQ("> xxxxxxxxx\r\n> ", obuff);
    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

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

