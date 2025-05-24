#ifndef GRAMMAR_CASE_H
#define GRAMMAR_CASE_H

#include "grammar.h"

int parse_rule_case(struct tokenVect *tokens, struct ast *parent);
int parse_case_clause(struct tokenVect *tokens, struct ast *parent);
int parse_case_item(struct tokenVect *tokens, struct ast *parent);

#endif /* ! GRAMMAR_CASE_H */
