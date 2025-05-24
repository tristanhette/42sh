#include "grammar_case.h"

int parse_rule_case(struct tokenVect *tokens, struct ast *parent)
{
    token_pop(tokens);

    if (token_peek(tokens)->type != TOKEN_WORD
        && token_peek(tokens)->type != TOKEN_VAR)
        goto error;

    addNode(parent, AST_CASE, "case");
    struct ast *new_parent = parent->children[parent->nb_children - 1];
    if (token_peek(tokens)->type == TOKEN_VAR)
        addNode(new_parent, AST_EXP, token_pop(tokens)->value);
    else
        addNode(new_parent, AST_CMD, token_pop(tokens)->value);

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (token_peek(tokens)->type != TOKEN_IN)
        goto error;
    token_pop(tokens);

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (is_first_case_clause(tokens))
    {
        if (!parse_case_clause(tokens, new_parent))
            goto error;
    }

    if (token_peek(tokens)->type != TOKEN_ESAC)
        goto error;
    token_pop(tokens);

    return 1;

error:
    return 0;
}

int parse_case_clause(struct tokenVect *tokens, struct ast *parent)
{
    if (!parse_case_item(tokens, parent))
        goto error;

    while (token_peek(tokens)->type == TOKEN_D_SEMICOLON)
    {
        token_pop(tokens);
        while (token_peek(tokens)->type == TOKEN_NEWLINE)
            token_pop(tokens);

        if (is_first_case_item(tokens))
        {
            if (!parse_case_item(tokens, parent))
                goto error;
        }
        else
            break;
    }

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    return 1;

error:
    return 0;
}

int parse_case_item(struct tokenVect *tokens, struct ast *parent)
{
    if (token_peek(tokens)->type == TOKEN_OPEN_PARENTHESIS)
        token_pop(tokens);

    if (token_peek(tokens)->type != TOKEN_WORD
        && token_peek(tokens)->type != TOKEN_VAR)
        goto error;

    if (token_peek(tokens)->type == TOKEN_VAR)
        addNode(parent, AST_EXP, token_pop(tokens)->value);
    else
        addNode(parent, AST_CMD, token_pop(tokens)->value);

    while (token_peek(tokens)->type == TOKEN_CONCAT)
    {
        struct token *t = token_pop(tokens);
        if (token_peek(tokens)->type == TOKEN_CLOSE_PARENTHESIS)
            break;
        addNode(parent, PARAM_CONCAT, t->value);
        if (token_peek(tokens)->type == TOKEN_VAR)
            addNode(parent, PARAM_VAR, token_pop(tokens)->value);
        else
            addNode(parent, PARAM, token_pop(tokens)->value);
    }

    while (token_peek(tokens)->type == TOKEN_PIPE)
    {
        token_pop(tokens);
        if (token_peek(tokens)->type != TOKEN_WORD
            && token_peek(tokens)->type == TOKEN_VAR)
            goto error;
        addNode(parent,
                token_peek(tokens)->type == TOKEN_VAR ? PARAM_VAR : PARAM,
                token_pop(tokens)->value);
    }

    if (token_peek(tokens)->type != TOKEN_CLOSE_PARENTHESIS)
        goto error;
    token_pop(tokens);

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (is_first_compound_list(tokens))
    {
        if (!parse_compound_list(tokens,
                                 parent->children[parent->nb_children - 1]))
            goto error;
    }

    return 1;

error:
    return 0;
}
