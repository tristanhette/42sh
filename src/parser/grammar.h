#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "grammar_shell.h"
#include "utils.h"
#include "utils_shell.h"

int parse_input(struct tokenVect *tokens, struct ast *parent);
int parse_list(struct tokenVect *tokens, struct ast *parent);
int parse_and_or(struct tokenVect *tokens, struct ast *parent);
int parse_pipeline(struct tokenVect *tokens, struct ast *parent);
int parse_command(struct tokenVect *tokens, struct ast *parent);
int parse_simple_command(struct tokenVect *tokens, struct ast *parent);
int parse_prefix(struct tokenVect *tokens, struct ast *parent);
int parse_redirection(struct tokenVect *tokens, struct ast *parent);
int parse_element(struct tokenVect *tokens, struct ast *parent);
int parse_compound_list(struct tokenVect *tokens, struct ast *parent);

#endif /* ! GRAMMAR_H */
