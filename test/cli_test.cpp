
#include <gtest/gtest.h>

#include <cli_debug.h>
#include "list.h"
#include "cli.h"
#include "io.h"
#include "test_io.h"

    /*
     *
     */

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

CLI cli = {
    .output = 0,
    .prompt = "> ",
    .eol = "\r\n",
};

    /*
     *
     */

static int got_action = false;

static void action_handler(CLI *cli, CliCommand *cmd)
{
    UNUSED(cli);
    UNUSED(cmd);
    got_action = true;
}

TEST(CLI, Create)
{
    CliCommand action = {
        .cmd = "help",
        .handler = action_handler,
        .help = "help text",
    };

    got_action = false;
    cli_init(& cli, 64, 0);
    cli_register(& cli, & action);

    // Check that chars are stored in the buffer
    EXPECT_STREQ("", cli.buff);
    cli_send(& cli, "help");
    EXPECT_STREQ("help", cli.buff);

    cli_close(& cli);
}

TEST(CLI, Close)
{
    CliCommand action = {
        .cmd = "help",
        .handler = action_handler,
        .help = "help text",
    };
    CliCommand more = {
        .cmd = "more",
        .handler = action_handler,
        .help = "help text",
    };

    got_action = false;
    cli_init(& cli, 64, 0);
    cli_register(& cli, & action);
    cli_register(& cli, & more);

    // Check that chars are stored in the buffer
    EXPECT_STREQ("", cli.buff);
    cli_send(& cli, "help");
    EXPECT_STREQ("help", cli.buff);

    cli_close(& cli);
    // Check the actions are unlinked
    EXPECT_EQ(action.next, (void*)0);
    EXPECT_EQ(more.next, (void*)0);
}

static void cli_die(CLI *cli, CliCommand *cmd)
{
    UNUSED(cli);
    UNUSED(cmd);
    ASSERT(false);
}

TEST(CLI, Help)
{
#define HELP0 "one line of text" 
#define HELP1 "another help line"
#define HELP2 "some more help" 

    CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = HELP0,
        .ctx = & cli.head,
    };
    CliCommand a1 = {
        .cmd = "anything",
        .handler = cli_die,
        .help = HELP1,
    };
    CliCommand a2 = {
        .cmd = "another",
        .handler = cli_die,
        .help = HELP2,
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);
    cli_register(& cli, & a2);

    io.reset();
    cli_send(& cli, "help\r\n");
    EXPECT_STREQ("help\r\n" "help : " HELP0 "\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "help anything\r\n");
    EXPECT_STREQ("help anything\r\n" "anything : " HELP1 "\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "help another\r\n");
    EXPECT_STREQ("help another\r\n" "another : " HELP2 "\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "help xx\r\n");
    EXPECT_STREQ("help xx\r\n" "'xx' not found\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "help another xx\r\n");
    EXPECT_STREQ("help another xx\r\n" "'xx' not found\r\n> ", io.get());

    // check that cli_help with no help text lists the available commands
    io.reset();
    a0.help = 0;
    cli_send(& cli, "help\r\n");
    EXPECT_STREQ("help\r\n" "help : \r\nanything : " HELP1 "\r\nanother : " HELP2 "\r\n> ", io.get());

    cli_close(& cli);
}

TEST(CLI, HelpSub)
{
    CliCommand a2 = {
        .cmd = "another",
        .handler = cli_die,
        .help = HELP2,
    };
    CliCommand a1 = {
        .cmd = "anything",
        .handler = cli_die,
        .help = HELP1,
        .subcommand = & a2,
    };
    CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = HELP0,
        .ctx = & cli.head,
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);

    // Check help <command>
    io.reset();
    cli_send(& cli, "help anything\r\n");
    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("help anything\r\n" "anything : " HELP1 "\r\n> ", io.get());

    // Check help <unknown>
    io.reset();
    cli_send(& cli, "help nowt\r\n");
    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("help nowt\r\n" "'nowt' not found\r\n> ", io.get());

    // Check help <command> <subcommand>
    io.reset();
    cli_send(& cli, "help anything another\r\n");
    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("help anything another\r\n" "another : " HELP2 "\r\n> ", io.get());

    cli_close(& cli);
}

TEST(CLI, Edit)
{
    CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
        .ctx = & cli.head,
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);

    io.reset();
    cli_send(& cli, "heldx");
    EXPECT_STREQ("heldx", cli.buff);
    EXPECT_STREQ("heldx", io.get());

    cli_send(& cli, "\b");
    EXPECT_STREQ("held", cli.buff);
    EXPECT_STREQ("heldx\b \b", io.get());

    cli_send(& cli, "\b");
    EXPECT_STREQ("hel", cli.buff);
    EXPECT_STREQ("heldx\b \b\b \b", io.get());

    cli_send(& cli, "p\r\n");
    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_close(& cli);
}

static const char *ctx_text = "hello world";
static bool ctx_ran = false;

void check_ctx(CLI *cli, CliCommand *cmd)
{
    UNUSED(cmd);
    ctx_ran = true;
    EXPECT_EQ(cli->ctx, ctx_text);
}

TEST(CLI, Context)
{
    CliCommand a0 = {
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

    cli_close(& cli);
}

TEST(CLI, EmptyLine)
{
    CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);

    cli_send(& cli, "\r\n");

    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_close(& cli);
}

TEST(CLI, OverflowLine)
{
    CliCommand a0 = {
        .cmd = "help",
        .handler = cli_help,
        .help = "help!",
    };

    io.reset();
    cli_init(& cli, 10, 0);
    cli_register(& cli, & a0);

    for (int i = 0; i < 10; i++)
    {
        cli_send(& cli, "x");
    }

    // Currently silently ignores the too-long command
    EXPECT_STREQ("> xxxxxxxxx\r\n> ", io.get());
    // Buffer should be cleared
    EXPECT_STREQ("", cli.buff);

    cli_close(& cli);
}

    /*
     *
     */

typedef struct Device
{
    const char* name;
    bool (*set)(int value);
    int  (*get)();
    bool (*power)(bool on);
}   Device;

static int laser_value;
bool set_laser(int v) { laser_value = v; return true; }
int get_laser() { return laser_value; }
bool power_laser(bool on) { UNUSED(on); return true; }

static Device laser = { "laser", set_laser, get_laser, 0 };

static void dev_power(CLI *cli, CliCommand *cmd)
{
    ASSERT(cmd->ctx);
    Device *dev = (Device*) cmd->ctx;

    const char *s = cli_get_arg(cli, 0);

    if (!strcmp(s, "?"))
    {
        // query the device
        ASSERT(dev->get);
        cli_print(cli, "%d%s", dev->get(), cli->eol);
        return;
    }

    char *end = 0;
    const long int v = strtol(s, & end, 10);
    if (*end != '\0')
    {
        // Number not fully converted, so an error
        LOG_DEBUG("error in value '%s'", s);
        return;
    }

    ASSERT(dev->set);
    const bool ok = dev->set((int) v);
    cli_print(cli, "%s%s", ok ? "ok" : "error", cli->eol);
}

    /*
     *
     */

TEST(CLI, Power)
{
    CliCommand s0 = {
        .cmd = "laser",
        .handler = dev_power,
        .help = "laser <on>|<off>|?",
        .ctx = & laser,
    };
    CliCommand a0 = {
        .cmd = "power",
        .handler = cli_help,
        .help = "power <device>",
        .subcommand = & s0,
        .ctx = & cli.head,
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);

    // Query the device
    io.reset();
    cli_send(& cli, "power laser ?\r\n");

    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("power laser ?\r\n0\r\n> ", io.get());

    // Set the device
    io.reset();
    cli_send(& cli, "power laser 1\r\n");
    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("power laser 1\r\nok\r\n> ", io.get());
    EXPECT_EQ(1, laser_value); 

    // Set the device
    io.reset();
    cli_send(& cli, "power laser 10\r\n");
    EXPECT_STREQ("", cli.buff);
    EXPECT_STREQ("power laser 10\r\nok\r\n> ", io.get());
    EXPECT_EQ(10, laser_value); 

    cli_close(& cli);
}

    /*
     *
     */

struct ctx {
    bool done;
};

static void hello(CLI *cli, CliCommand *cmd)
{
    UNUSED(cmd);
    const char *s = cli->args[1];
    EXPECT_STREQ("world", s);
    LOG_DEBUG("%s", s);
}

static void bye(CLI *cli, CliCommand *cmd)
{
    UNUSED(cmd);
    LOG_DEBUG("");

    struct ctx *ctx = (struct ctx*) cli->ctx;
    ctx->done = true;
}

TEST(CLI, Input)
{
    CliCommand a0 = {
        .cmd = "hello",
        .handler = hello,
        .help = "hello",
    };
    CliCommand a1 = {
        .cmd = "bye",
        .handler = bye,
        .help = "bye",
    };

    struct ctx ctx = { .done = false };

    cli_init(& cli, 64, & ctx);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);

    io.reset();

    FILE *in = fopen("test/input.txt", "r");

    while (!feof(in))
    {
        char buff[64];

        const char *s = fgets(buff, sizeof(buff), in);
        if (!s)
        {
            break;
        }

        cli_send(& cli, buff);
    }

    fclose(in);

    EXPECT_TRUE(ctx.done);

    cli_close(& cli);
}

    /*
     *
     */

static void nowt(CLI *cli, CliCommand *cmd)
{
    UNUSED(cli);
    UNUSED(cmd);
}

TEST(CLI, AutoComplete)
{
    CliCommand s2 = {
        .cmd = "three",
        .handler = nowt,
    };
    CliCommand s1 = {
        .cmd = "two",
        .handler = nowt,
        .next = & s2,
    };
    CliCommand s0 = {
        .cmd = "one",
        .handler = nowt,
        .next = & s1,
    };
    CliCommand a0 = {
        .cmd = "hello",
        .handler = nowt,
        .help = "hello",
        .subcommand = & s0,
    };
    CliCommand a1 = {
        .cmd = "bye",
        .handler = nowt,
        .help = "bye",
    };
    CliCommand a2 = {
        .cmd = "partial",
        .handler = nowt,
        .help = "partial",
    };
    CliCommand a3 = {
        .cmd = "part",
        .handler = nowt,
        .help = "part",
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a0);
    cli_register(& cli, & a1);
    cli_register(& cli, & a2);
    cli_register(& cli, & a3);

    io.reset();
    // should list all the commands
    cli_process(& cli, '\t');
    EXPECT_STREQ("\r\nhello\r\nbye\r\npartial\r\npart\r\n> ", io.get());

    // 'h' '\t' should complete 'hello '
    io.reset();
    cli_send(& cli, "h\t");
    EXPECT_STREQ("hello ", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // "par\t" should match 'part' and 'partial'
    io.reset();
    cli_send(& cli, "par\t");
    EXPECT_STREQ("par\r\npartial\r\npart\r\n> par", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // "part\t" should match 'part' and 'partial'
    io.reset();
    cli_send(& cli, "part\t");
    EXPECT_STREQ("part\r\npartial\r\npart\r\n> part", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // "parti\t" should match 'partial'
    io.reset();
    cli_send(& cli, "parti\t");
    EXPECT_STREQ("partial ", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // "partial \t" should match 'partial' and do nothing
    io.reset();
    cli_send(& cli, "partial \t");
    EXPECT_STREQ("partial ", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // test autocomplete for subcommands

    // "part \t" should invoke subcommand handler
    io.reset();
    cli_send(& cli, "part \t");
    EXPECT_STREQ("part ", io.get());
    cli_send(& cli, "\r\n"); // complete the command

    // test autocomplete for subcommands

#if 0
    io.reset();
    cli_send(& cli, "hello o\t");
    EXPECT_STREQ("hello one", io.get());
    cli_send(& cli, "\r\n"); // complete the command

#endif
    cli_close(& cli);
}

    /*
     *
     */

static void sub(CLI *cli, CliCommand *cmd)
{
    void *ctx = cmd->ctx;
    int v = * (int *) ctx;
    const char *s = cli_get_arg(cli, 0);
    s = s ? s : "";
    cli_print(cli, "got %s '%s' %d\r\n", cmd->cmd, s, v);
}

TEST(Cli, Subcommand)
{
    int i0 = 3;
    CliCommand a0 = {
        .cmd = "three",
        .handler = sub,
        .subcommand = 0,
        .ctx = & i0,
    };
    int i1 = 2;
    CliCommand a1 = {
        .cmd = "two",
        .handler = sub,
        .subcommand = & a0,
        .ctx = & i1,
    };
    int i2 = 1;
    CliCommand a2 = {
        .cmd = "one",
        .handler = sub,
        .subcommand = & a1,
        .ctx = & i2,
    };
    int i3 = 0;
    CliCommand a3 = {
        .cmd = "top",
        .handler = sub,
        .subcommand = & a2,
        .ctx = & i3,
    };

    cli_init(& cli, 64, 0);
    cli_register(& cli, & a3);

    io.reset();

    io.reset();
    cli_send(& cli, "top\r\n");
    EXPECT_STREQ("top\r\ngot top '' 0\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top xx\r\n");
    EXPECT_STREQ("top xx\r\ngot top 'xx' 0\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one\r\n");
    EXPECT_STREQ("top one\r\ngot one '' 1\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one xx\r\n");
    EXPECT_STREQ("top one xx\r\ngot one 'xx' 1\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one two\r\n");
    EXPECT_STREQ("top one two\r\ngot two '' 2\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one two xx\r\n");
    EXPECT_STREQ("top one two xx\r\ngot two 'xx' 2\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one two three\r\n");
    EXPECT_STREQ("top one two three\r\ngot three '' 3\r\n> ", io.get());

    io.reset();
    cli_send(& cli, "top one two three xx\r\n");
    EXPECT_STREQ("top one two three xx\r\ngot three 'xx' 3\r\n> ", io.get());

    cli_close(& cli);
}

//  FIN
