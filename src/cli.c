
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <cli_debug.h>
#include "list.h"
#include "cli.h"

static pList* next_fn(pList item)
{
    CliCommand *cmd = (CliCommand *) item;
    return (pList*) & cmd->next;
}

    /*
     *
     */

void cli_print(CLI *cli, const char *fmt, ...)
{
    ASSERT(cli);
    ASSERT(cli->output);

    va_list va;
    va_start(va, fmt);

    vfprintf(cli->output, fmt, va);

    va_end(va);
}

void cli_init(CLI *cli, size_t size, void *ctx)
{
    ASSERT(size);
    cli->buff = (char*) malloc(size+1);
    cli->buff[0] = '\0';
    cli->size = size;
    cli->cursor = 0;
    cli->head = 0;
    cli->ctx = ctx;

    // Start with the initial prompt
    cli_print(cli, "%s", cli->prompt);
}

    /*
     *
     */

void cli_register(CLI *cli, CliCommand *cmd)
{
    list_append((pList*) & cli->head, (pList) cmd, next_fn, cli->mutex);
}

    /*
     *
     */

static int match_cmd(pList w, void *arg)
{
    CliCommand *cmd = (CliCommand *) w;
    const char* match = (const char*) arg;

    return strcmp(cmd->cmd, match) == 0;
}

static CliCommand* find_command(CLI *cli, const char* name)
{
    // Look up the command
    CliCommand *exec = (CliCommand*) list_find((pList*) & cli->head, next_fn, match_cmd, (void*) name, cli->mutex);
    return exec;
}

static CliCommand* find_subcommand(CLI *cli, CliCommand *cmd, const char* name)
{
    // Look up the command
    CliCommand *exec = (CliCommand*) list_find((pList*) & cmd->subcommand, next_fn, match_cmd, (void*) name, cli->mutex);
    return exec;
}

static void not_found(CLI *cli, const char *cmd)
{
    cli_print(cli, "'%s' not found%s", cmd, cli->eol);
}

    /*
     *
     */

static bool run_subcommand(CLI *cli, CliCommand* cmd, int idx)
{
    // handle subcommands, if any
    if (!cmd->subcommand)
    {
        return false;
    }

    const char *s = cli->args[idx];
    if (!s)
    {
        // no args found
        return false;
    }
 
    CliCommand *sub = find_subcommand(cli, cmd, s);
    if (!sub)
    {
        // no matching subcommand found
        return false;
    }

    printf("%s\r\n", s);
    return true;
}

static void cli_execute(CLI *cli)
{
    // Extract the words in the buffer

    cli->nest = 0;
    cli->strtok_save = 0;
    char *s = cli->buff;
    for (int i = 0; i < CLI_MAX_ARGS; i++)
    {
        const char* cmd = strtok_r(s, " ", & cli->strtok_save);
        cli->args[i] = cmd;
        if (!cmd)
        {
            break;
        }
        s = 0;
    }

    const char *cmd = cli->args[cli->nest++];

    if (!cmd)
    {
        //  Empty line. Reply with a prompt
        cli_print(cli, "%s", cli->prompt);
        return;
    }

    // Look up the command
    CliCommand *exec = find_command(cli, cmd);

    if (!exec)
    {
        // Command not found
        not_found(cli, cmd);
        return;
    }

    ASSERT(exec);

    if (run_subcommand(cli, exec, cli->nest))
    {
        return;
    }

    // Execute the command
    ASSERT(exec->handler);
    exec->handler(cli, exec);
}

static void cli_clear(CLI *cli)
{
    cli->cursor = 0;
    cli->buff[0] = '\0';
}

    /*
     *
     */

static void _help(CLI *cli, CliCommand *cmd)
{
    cli_print(cli, "%s : %s%s", cmd->cmd, cmd->help, cli->eol);
}

static int visit_help(pList w, void *arg)
{
    // Callback function : called for each command in the list
    CliCommand *cmd = (CliCommand *) w;
    CLI *cli = (CLI*) arg;

    _help(cli, cmd);
    return 0;
}

void cli_help(CLI *cli, CliCommand* cmd)
{
    UNUSED(cmd);
    // Is there a subcommand?
    const char *s = cli->args[1];

    if (s)
    {
        CliCommand *subcommand = find_command(cli, s);
        if (subcommand)
        {
            _help(cli, subcommand);
            return;
        }

        // subcommand not found
        not_found(cli, s);
        return;
    }

    // Call visit_help() on all elements of the list
    list_visit((pList*) & cli->head, next_fn, visit_help, (void*) cli, cli->mutex);
}

    /*
     *
     */

struct autocomplete
{
    CLI *cli;
    CliCommand *last;
    int count;
    bool print;
};

static int visit_auto(pList w, void *arg)
{
    // Callback function : called for each command in the list
    ASSERT(w);
    ASSERT(arg);
    CliCommand *cmd = (CliCommand *) w;
    struct autocomplete *ac = (struct autocomplete *) arg;
    CLI *cli = ac->cli;

    if (!strncmp(cli->buff, cmd->cmd, (size_t) cli->cursor))
    {
        // matches the command so far
        ac->last = cmd;
        ac->count += 1;

        if (ac->print)
        {
            cli_print(cli, "%s%s", cmd->cmd, cli->eol);
        }
    }

    return 0;
}

void cli_autocomplete(CLI *cli)
{
    // Check for partial match of command handlers
    struct autocomplete ac = { .cli = cli, .count = 0, .last = 0, .print = false };

    list_visit((pList*) & cli->head, next_fn, visit_auto, (void*) & ac, cli->mutex);

    if (ac.count == 0)
    {
        // No match. Do nothing
        return;
    }

    if (ac.count == 1)
    {
        // Single match. autocomplete this
        CliCommand *cmd = ac.last;
        ASSERT(strlen(cmd->cmd) >= (size_t) cli->cursor);
        const char *s = & cmd->cmd[cli->cursor];
        for (; *s; s++)
        {
            cli_process(cli, *s);
        }
        cli_process(cli, ' ');
        return;
    }

    // Print the partial matches
    ac.print = true;
    cli_print(cli, "%s", cli->eol);
    list_visit((pList*) & cli->head, next_fn, visit_auto, (void*) & ac, cli->mutex);
    cli_print(cli, "%s", cli->prompt);
    // restore the buffer so far ..
    cli_print(cli, "%s", cli->buff);
}

    /*
     *
     */

void cli_process(CLI *cli, char c)
{
    if (((size_t)(cli->cursor + 1)) >= cli->size)
    {
        //  line is full : ERROR
        cli_clear(cli);
        cli_print(cli, "%s%s", cli->eol, cli->prompt);
        return;
    }

    if (c == '\t')
    {
        cli_autocomplete(cli);
        return;
    }

    // Echo the char
    cli_print(cli, "%c", c);

    // Just ignore carriage return
    if (c == '\r')
    {
        return;
    }

    // handle backspace
    if (c == '\b')
    {
        if (cli->cursor > 0)
        {
            // delete the last char
            cli->cursor -= 1;
            cli->buff[cli->cursor] = '\0';
            // overwrite the deleted char
            cli_print(cli, " \b");
        }
        return;
    }

    if (c == '\n')
    {
        // Execute the line
        cli_execute(cli);
        cli_clear(cli);
        cli_print(cli, "%s", cli->prompt);
        return;
    }

    // Buffer the char and return
    cli->buff[cli->cursor] = c;
    cli->cursor += 1;
    cli->buff[cli->cursor] = '\0';
}

    /*
     *
     */

void cli_close(CLI *cli)
{
    ASSERT(cli);
    ASSERT(cli->buff);

    // Free the text input buffer
    free(cli->buff);
    cli->buff = 0;

    // Unlink all the actions
    while (cli->head)
    {
        list_remove((pList*) & cli->head, (pList) cli->head, next_fn, cli->mutex); 
    }
}

//  FIN
