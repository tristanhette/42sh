#include "utils_shell.h"

#include <ctype.h>

static int get_type(struct tokenVect *tokens)
{
    return token_peek(tokens)->type;
}

int is_first_shell_command(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_OPEN_BRACKETS || type == TOKEN_OPEN_PARENTHESIS
        || is_first_for(tokens) || is_first_if(tokens) || is_first_until(tokens)
        || is_first_while(tokens) || is_first_case(tokens);
}

int is_first_funcdec(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    if (type != TOKEN_WORD)
        return 0;
    if (tokens->pos + 1 == tokens->capacity - 1)
        return 0;
    enum TokenType next = tokens->data[tokens->pos + 1]->type;
    return next == TOKEN_OPEN_PARENTHESIS;
}

int is_first_if(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_IF;
}

int is_first_for(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_FOR;
}

int is_first_while(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_WHILE;
}

int is_first_until(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_UNTIL;
}

int is_first_else_clause(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_ELSE || type == TOKEN_ELIF;
}

int is_first_compound_list(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_NEWLINE || is_first_and_or(tokens);
}

int is_redirection_token(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_GREATER || type == TOKEN_LESS
        || type == TOKEN_DOUBLE_GREATER || type == TOKEN_GREATER_AMPERSAND
        || type == TOKEN_LESS_AMPERSAND || type == TOKEN_GREATER_PIPE
        || type == TOKEN_LESS_GREATER;
}

int is_valid_word(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type != TOKEN_NEWLINE && type != TOKEN_SEMICOLON && type != TOKEN_EOF
        && type != TOKEN_AND && type != TOKEN_OR && type != TOKEN_PIPE
        && type != TOKEN_IONUMBER && type != TOKEN_OPEN_PARENTHESIS
        && type != TOKEN_CLOSE_PARENTHESIS && type != TOKEN_D_SEMICOLON
        && !is_redirection_token(tokens);
}
