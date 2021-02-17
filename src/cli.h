
#if !defined(__CLI_H__)

#define __CLI_H__

#include "mutex.h"
#include "list.h"

struct CLI;

typedef struct CliCommand {
    const char *cmd;
    void (*handler)(CLI *cli, struct CliCommand *cmd);
    const char *help;
    struct CliCommand *next;
}   CliCommand;

typedef struct CLI {
    char *buff;
    int size;
    int cursor;
    char *strtok_save;
    pList head;
    void (*output)(const char *s);
    const char* prompt;
    const char* eol;
    Mutex *mutex; // can be null
    void *ctx; // context
}   CLI;

void cli_init(CLI *cli, int size, void *ctx);
void cli_close(CLI *cli);

void cli_register(CLI *cli, CliCommand *cmd);
void cli_process(CLI *cli, char c);

// Default 'help' command : 
void cli_help(CLI *cli, CliCommand* cmd);

#endif  //  __CLI_H__

//  FIN
