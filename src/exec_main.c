#define _DEFAULT_SOURCE
#include <string.h>

#include "ast/ast.h"
#include "fonctions/fonction.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "variables/variables.h"

int exec_main(int argc, char **argv)
{
    bool pretty_print = false;
    if (strcmp(argv[argc - 1], "--pretty-print") == 0)
    {
        pretty_print = true;
        argc--;
    }

    int return_code = 0;
    char *input = io_backend(argc, argv);
    if (*input == 0)
        goto success;

    // Ask for tokens
    struct tokenVect *tokens = lexer(input);

    if (tokens == NULL)
        errx(2, "Lexer: Error during lexing.");

    if (pretty_print)
    {
        print_tokenVect(tokens);
        puts("\n");
    }

    // Ask for ast
    struct ast *ast = parse(tokens);
    if (pretty_print)
    {
        print_ast(ast, 15);
        puts("\n");
    }
    // initialize_globals(argc, argv);
    //  Evaluate Ast
    return_code = evaluate_ast(ast);

    // free_fonctions_table(commandLineInfo.fonction_table);
    free_tokenVect(tokens);
    free_ast(ast);
success:
    free(input);

    return return_code;
}
