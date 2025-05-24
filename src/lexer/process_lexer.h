#ifndef PROCESS_LEXER_H
#define PROCESS_LEXER_H

#include "lexer.h"

void process_backslash(struct lexing_param *lexing, char input, char next);
void process_squote(struct lexing_param *lexing, char next);
void process_dquote(struct lexing_param *lexing, char next);
void process_add_char(struct lexing_param *lexing, char input);
void process_remove_braced_var(struct tokenVect *tokens);
void process_concat_double_semi_col(struct tokenVect *tokens);
void process_cut_case_pipe(struct tokenVect *tokens);
void replace_with_cmd_output(char **inputPtr);

#endif /* PROCESS_LEXER_H */
