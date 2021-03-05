
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "debug.h"
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

    ovprintf(cli->output, fmt, va);

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

static void not_found(CLI *cli, const char *cmd)
{
    cli_print(cli, "'%s' not found%s", cmd, cli->eol);
}

    /*
     *
     */

static void cli_execute(CLI *cli)
{
    // Extract the first word in the buffer
    const char* cmd = strtok_r(cli->buff, " ", & cli->strtok_save);

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
    const char *s = strtok_r(0, " ", & cli->strtok_save);

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

void cli_process(CLI *cli, char c)
{
    if (((size_t)(cli->cursor + 1)) >= cli->size)
    {
        //  line is full : ERROR
        cli_clear(cli);
        cli_print(cli, "%s%s", cli->eol, cli->prompt);
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
    free(cli->buff);
    cli->buff = 0;
}

//  FIN
