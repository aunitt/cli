
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "list.h"
#include "cli.h"

static pList* next(pList item)
{
    CliCommand *cmd = (CliCommand *) item;
    return (pList*) & cmd->next;
}

static int match_cmd(pList w, void *arg)
{
    CliCommand *cmd = (CliCommand *) w;
    const char* match = (const char*) arg;

    return strcmp(cmd->cmd, match) == 0;
}

    /*
     *
     */

static void cli_reply(CLI *cli, const char *text)
{
    ASSERT(cli);
    ASSERT(cli->output);
    cli->output(text);
}

void cli_init(CLI *cli, int size)
{
    ASSERT(size);
    cli->buff = (char*) malloc(size+1);
    cli->buff[0] = '\0';
    cli->size = size;
    cli->cursor = 0;
    cli->head = 0;

    cli_reply(cli, cli->prompt);
}

void cli_register(CLI *cli, CliCommand *cmd)
{
    list_append((pList*) & cli->head, (pList) cmd, next, 0);
}

    /*
     *
     */

static void cli_parse(CLI *cli)
{
    char *save = 0;

    // Extract the first word in the buffer
    const char* cmd = strtok_r(cli->buff, " ", & save);
    ALOG_DEBUG("'%s'", cmd);

    // Look up the command
    CliCommand *exec = (CliCommand*) list_find((pList*) & cli->head, next, match_cmd, (void*) cmd, 0);
    ALOG_DEBUG("%p", exec);

    if (!exec)
    {
        // Command not found
        cli_reply(cli, "'");
        cli_reply(cli, cmd);
        cli_reply(cli, "' not found");
        cli_reply(cli, cli->eol);
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

static int visit_help(pList w, void *arg)
{
    CliCommand *cmd = (CliCommand *) w;
    CLI *cli = (CLI*) arg;

    cli_reply(cli, cmd->help);
    cli_reply(cli, cli->eol);
 
    return 0;
}

void cli_help(CLI *cli, CliCommand* cmd)
{
    // Call visit_help() on all elements of the list
    list_visit((pList*) & cli->head, next, visit_help, (void*) cli, 0);
}

    /*
     *
     */

void cli_process(CLI *cli, char c)
{
    if ((cli->cursor + 1) >= cli->size)
    {
        //  TODO : line is full : ERROR
        return;
    }

    // Just ignore carriage return
    if (c == '\r')
    {
        return;
    }

    // Echo the char
    char reply[2] = { c, '\0' };
    cli_reply(cli, reply);

    // TODO : handle backspace

    if (c == '\n')
    {
        // Execute the line
        cli_parse(cli);
        cli_clear(cli);
        cli_reply(cli, cli->prompt);
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
