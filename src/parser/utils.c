#include "utils.h"

#include <ctype.h>

static int get_type(struct tokenVect *tokens)
{
    return token_peek(tokens)->type;
}

int is_first_and_or(struct tokenVect *tokens)
{
    return is_first_pipeline(tokens);
}

int is_first_pipeline(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_NEG || is_first_command(tokens);
}

int is_first_command(struct tokenVect *tokens)
{
    return is_first_simple_command(tokens) || is_first_shell_command(tokens)
        || is_first_funcdec(tokens);
}

int is_first_simple_command(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    if (type != TOKEN_WORD && type != TOKEN_VAR && !is_first_prefix(tokens))
        return 0;

    if (tokens->pos + 1 == tokens->capacity - 1)
        return 1;

    enum TokenType next = tokens->data[tokens->pos + 1]->type;
    return next != TOKEN_OPEN_PARENTHESIS;
}

int is_first_prefix(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_ASSIGNMENT_WORD || is_first_redirection(tokens);
}

int is_first_redirection(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_IONUMBER || type == TOKEN_GREATER || type == TOKEN_LESS
        || type == TOKEN_DOUBLE_GREATER || type == TOKEN_GREATER_AMPERSAND
        || type == TOKEN_LESS_AMPERSAND || type == TOKEN_GREATER_PIPE
        || type == TOKEN_LESS_GREATER;
}

int is_first_element(struct tokenVect *tokens)
{
    enum TokenType type = get_type(tokens);
    return type == TOKEN_WORD || type == TOKEN_VAR
        || is_first_redirection(tokens);
}
