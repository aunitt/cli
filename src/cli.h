
#if !defined(__CLI_H__)

#define __CLI_H__

typedef struct CliCommand {
    const char *cmd;
    void (*handler)(struct CliCommand *cmd, int argc, char **argv);
    const char *help;
    struct CliCommand *next;
}   CliCommand;

void cli_register(CliCommand *cmd);
void cli_process(char c);

#endif  //  __CLI_H__

//  FIN
