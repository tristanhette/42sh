#ifndef UTILS_LEXER_H
#define UTILS_LEXER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t my_strnlen(const char *s, size_t n);
char *my_strndup(const char *s, size_t n);
char *my_strdup(const char *s);
bool is_delimiter(char c);
bool is_end_of_statement(char c);
bool is_group_delimiter(char c);
bool is_containing_redirection(const char *input);
bool is_assignment_word(char *input);
bool is_end_in_dquote(const char *str, size_t len);
bool is_end_in_comment_or_quote(const char *str, size_t len);

#endif /* !UTILS_LEXER_H */
