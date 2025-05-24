#ifndef UTILS_SHELL_H
#define UTILS_SHELL_H

#include "grammar.h"
#include "utils.h"
#include "utils_case.h"

int is_first_shell_command(struct tokenVect *tokens);
int is_first_funcdec(struct tokenVect *tokens);
int is_first_if(struct tokenVect *tokens);
int is_first_for(struct tokenVect *tokens);
int is_first_while(struct tokenVect *tokens);
int is_first_until(struct tokenVect *tokens);
int is_first_else_clause(struct tokenVect *tokens);
int is_redirection_token(struct tokenVect *tokens);
int is_valid_word(struct tokenVect *tokens);

#endif /* ! UTILS_SHELL_H */
