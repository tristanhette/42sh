#ifndef GRAMMAR_SHELL_H
#define GRAMMAR_SHELL_H

#include "grammar.h"
#include "grammar_case.h"

int parse_shell_command(struct tokenVect *tokens, struct ast *parent);
int parse_funcdec(struct tokenVect *tokens, struct ast *parent);
int parse_rule_if(struct tokenVect *tokens, struct ast *parent);
int parse_else_clause(struct tokenVect *tokens, struct ast *parent);
int parse_rule_while(struct tokenVect *tokens, struct ast *parent);
int parse_rule_until(struct tokenVect *tokens, struct ast *parent);
int parse_rule_for(struct tokenVect *tokens, struct ast *parent);

#endif /* ! GRAMMAR_SHELL_H */
