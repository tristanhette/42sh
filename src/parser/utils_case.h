#ifndef UTILS_CASE_H
#define UTILS_CASE_H

#include "grammar.h"
#include "utils.h"
#include "utils_shell.h"

int is_first_case(struct tokenVect *tokens);
int is_first_case_clause(struct tokenVect *tokens);
int is_first_case_item(struct tokenVect *tokens);

#endif /* ! UTILS_CASE_H */
