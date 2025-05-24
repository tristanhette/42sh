#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "exec_main.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "variables/variables.h"

struct CommandLineInfo commandLineInfo;

int main(int argc, char **argv)
{
    initialize_globals(argc, argv);
    // set directories environment variables
    char buf[1024] = { 0 };
    getcwd(buf, 1024 * sizeof(char));
    setenv("HOME", buf, 1);
    setenv("PWD", buf, 1);
    setenv("OLDPWD", buf, 1);
    //
    int res = exec_main(argc, argv);
    free_fonctions_table(commandLineInfo.fonction_table);
    return res;
}
