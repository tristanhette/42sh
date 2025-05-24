#include "utils_case.h"

int is_first_case(struct tokenVect *tokens)
{
    return token_peek(tokens)->type == TOKEN_CASE;
}

int is_first_case_clause(struct tokenVect *tokens)
{
    return is_first_case_item(tokens);
}

int is_first_case_item(struct tokenVect *tokens)
{
    return token_peek(tokens)->type == TOKEN_OPEN_PARENTHESIS
        || token_peek(tokens)->type == TOKEN_WORD
        || token_peek(tokens)->type == TOKEN_VAR;
}
