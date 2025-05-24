#ifndef UTILS_H
#define UTILS_H

#include "grammar.h"
#include "utils_shell.h"

int is_first_and_or(struct tokenVect *tokens);
int is_first_pipeline(struct tokenVect *tokens);
int is_first_command(struct tokenVect *tokens);
int is_first_simple_command(struct tokenVect *tokens);
int is_first_prefix(struct tokenVect *tokens);
int is_first_redirection(struct tokenVect *tokens);
int is_first_element(struct tokenVect *tokens);
int is_first_compound_list(struct tokenVect *tokens);

#endif /* ! UTILS_H */
