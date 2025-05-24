#include "parser.h"

#include <err.h>
#include <stdio.h>

struct ast *parse(struct tokenVect *tokens)
{
    // Create ast and give to parse_input
    struct ast *ast = create_node(AST_CMD, "shell");

    if (!parse_input(tokens, ast))
        errx(2, "Grammar: Syntax incorrect at token: %s.",
             token_peek(tokens)->value);

    return ast;
}
