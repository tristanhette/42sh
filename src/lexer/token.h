#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

#include "utils_lexer.h"

enum TokenType
{
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_UNTIL,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_CASE,
    TOKEN_ESAC,
    TOKEN_SEMICOLON,
    TOKEN_D_SEMICOLON,
    TOKEN_NEWLINE,
    TOKEN_QUOTE,
    TOKEN_WORD,
    TOKEN_VAR,
    TOKEN_ASSIGNMENT_WORD,
    TOKEN_NEG,
    TOKEN_PIPE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_IONUMBER,
    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_DOUBLE_GREATER,
    TOKEN_GREATER_AMPERSAND,
    TOKEN_LESS_AMPERSAND,
    TOKEN_GREATER_PIPE,
    TOKEN_LESS_GREATER,
    TOKEN_CONCAT,
    TOKEN_OPEN_BRACKETS,
    TOKEN_CLOSE_BRACKETS,
    TOKEN_OPEN_PARENTHESIS,
    TOKEN_CLOSE_PARENTHESIS,
    TOKEN_EOF
};

struct token
{
    enum TokenType type;
    char *value;
};

struct tokenVect
{
    struct token **data;
    size_t len;
    size_t pos;
    size_t capacity;
};

enum TokenType determine_token_type(const char *value);
enum TokenType determine_token_type2(const char *value);
struct token *create_token(char *word);
struct tokenVect *create_list_token(void);
void append_token(struct tokenVect *tokens, struct token *token);
struct token *token_peek(struct tokenVect *tokens);
struct token *token_pop(struct tokenVect *tokens);
void free_token(struct token *token);
void free_tokenVect(struct tokenVect *tokens);
void print_tokenVect(struct tokenVect *tokens);

#endif /* !TOKEN_H */
