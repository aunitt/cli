
#if !defined(__CLI_H__)

#define __CLI_H__

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
    CliCommand *head;
    void (*output)(const char *s);
    const char* prompt;
}   CLI;

void cli_init(CLI *cli, int size);
void cli_close(CLI *cli);

void cli_register(CLI *cli, CliCommand *cmd);
void cli_process(CLI *cli, char c);

#endif  //  __CLI_H__

//  FIN
