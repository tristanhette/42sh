#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdio.h>

#include "token.h"
#include "utils_lexer.h"

struct lexing_param
{
    struct tokenVect *tokens;
    char *word;
    size_t size_word;
    size_t word_capacity;
    bool in_squote;
    bool in_dquote;
    bool was_squoted;
    bool was_dquoted;
    bool in_comment;
    bool in_backslash;
    bool was_backslash;
    bool is_assignment_word;
};

void init_lexing(struct lexing_param *lexing);
void split_redirection(const char *input, struct tokenVect *tokens);
void finalize_word(struct lexing_param *lexing);
void process_end_of_statement(struct tokenVect *tokens, char c);
void process_input(struct lexing_param *lexing, char previous, char input,
                   char next);
void continue_process_input(struct lexing_param *lexing, char input, char next);
struct tokenVect *lexer(char *input);
char *check_command_substitution(char *input);
char *execute_command(char *cmd);
bool replace(char **inputPtr, char **input, size_t inputSize, char *pos);

#endif /* !LEXER_H */
