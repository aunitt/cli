
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
    LOG_DEBUG("'%s'", cmd);

    // Look up the command
    CliCommand *exec = (CliCommand*) list_find((pList*) & cli->head, next, match_cmd, (void*) cmd, 0);
    LOG_DEBUG("%p", exec);

    if (!exec)
    {
        // Command not found
        cli_reply(cli, "'");
        cli_reply(cli, cmd);
        cli_reply(cli, "' not found\n");
        cli_reply(cli, cli->prompt);
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

void cli_process(CLI *cli, char c)
{
    //LOG_DEBUG("%c", c);

    if ((cli->cursor + 1) >= cli->size)
    {
        //  TODO : line is full : ERROR
        return;
    }

    // Echo the char
    char reply[2] = { c, '\0' };
    cli_reply(cli, reply);

    // TODO : handle backspace

    if ((c == '\r') || (c == '\n'))
    {
        // Execute the line
        cli_parse(cli);
        cli_clear(cli);
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
