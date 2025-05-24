#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdlib.h>

#include "../ast/ast.h"
#include "../fonctions/fonction.h"

struct CommandLineInfo
{
    int argc;
    char **argv;
    int n_break;
    int n_continue;
    struct fonctions *fonction_table;
};

extern struct CommandLineInfo commandLineInfo;

struct variable
{
    char *name;
    char *value;
};

struct variables
{
    struct variable **data;
    size_t len;
    size_t capacity;
};

void initialize_globals(int argc, char *argv[]);
struct Command *expand_command(struct Command *command);
char *switch_var(char *input, int last_exit_code);
char *expand_parameters2(const char *input, size_t length, char *result,
                         size_t i);
char *expand_parameters(const char *input);
char *copy_string(const char *source);
int var_builtin(struct Command *command);

#endif /* !VARIABLES_H */
