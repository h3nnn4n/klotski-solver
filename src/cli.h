#ifndef SRC_CLI_H_
#define SRC_CLI_H_

#include <stdbool.h>

typedef enum {
    CLI_HELP,
    CLI_GUI,
    CLI_BUILD,
    CLI_REBUILD,
    CLI_DELETE,
    CLI_VERIFY,
    CLI_LIST,
} cli_action_t;

typedef struct {
    cli_action_t action;
    const char  *pdb_filter;
} cli_args_t;

int        cli_run(int argc, char **argv);
cli_args_t cli_parse(int argc, char **argv);

#endif
