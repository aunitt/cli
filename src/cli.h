
#if !defined(__CLI_H__)

#define __CLI_H__

#include <stdio.h>

struct CLI;
struct Mutex;

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
    CliCommand *head;
    FILE *output;
    const char* prompt;
    const char* eol;
    struct Mutex *mutex; // can be null
    void *ctx; // context
}   CLI;

void cli_init(CLI *cli, int size, void *ctx);
void cli_close(CLI *cli);

void cli_register(CLI *cli, CliCommand *cmd);
void cli_process(CLI *cli, char c);

void cli_print(CLI *cli, const char *fmt, ...) __attribute__((format(printf,2,3)));

// Default 'help' command handler
void cli_help(CLI *cli, CliCommand* cmd);

#endif  //  __CLI_H__

//  FIN
