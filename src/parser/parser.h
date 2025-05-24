#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "grammar.h"

struct ast *parse(struct tokenVect *tokens);

#endif /* ! PARSER_H */
