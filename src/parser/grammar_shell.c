#include "grammar.h"
#include "grammar_case.h"
#include "parser.h"
#include "utils.h"
#include "utils_case.h"
#include "utils_shell.h"

int parse_shell_command(struct tokenVect *tokens, struct ast *parent)
{
    // Return the created node
    if (is_first_if(tokens))
    {
        if (!parse_rule_if(tokens, parent))
            goto error;
    }
    else if (is_first_while(tokens))
    {
        if (!parse_rule_while(tokens, parent))
            goto error;
    }
    else if (is_first_until(tokens))
    {
        if (!parse_rule_until(tokens, parent))
            goto error;
    }
    else if (is_first_for(tokens))
    {
        if (!parse_rule_for(tokens, parent))
            goto error;
    }
    else if (is_first_case(tokens))
    {
        if (!parse_rule_case(tokens, parent))
            goto error;
    }
    else if (token_peek(tokens)->type == TOKEN_OPEN_BRACKETS)
    {
        token_pop(tokens);
        addNode(parent, AST_CMD, "shell"); // Maybe it can work
        struct ast *new_parent = parent->children[parent->nb_children - 1];
        if (!is_first_compound_list(tokens)
            || !parse_compound_list(tokens, new_parent))
            goto error;

        if (token_peek(tokens)->type != TOKEN_CLOSE_BRACKETS)
            goto error;
        token_pop(tokens);
    }
    else if (token_peek(tokens)->type == TOKEN_OPEN_PARENTHESIS)
    {
        addNode(parent, AST_SUB, token_pop(tokens)->value);
        struct ast *new_parent = parent->children[parent->nb_children - 1];
        if (!is_first_compound_list(tokens)
            || !parse_compound_list(tokens, new_parent))
            goto error;

        if (token_peek(tokens)->type != TOKEN_CLOSE_PARENTHESIS)
            goto error;
        token_pop(tokens);
    }
    else
        goto error;

    return 1;

error:
    return 0;
}

int parse_funcdec(struct tokenVect *tokens, struct ast *parent)
{
    addNode(parent, AST_FONCTION, token_pop(tokens)->value);
    struct ast *new_parent = parent->children[parent->nb_children - 1];

    if (token_peek(tokens)->type != TOKEN_OPEN_PARENTHESIS
        || token_pop(tokens) == NULL)
        goto error;
    if (token_peek(tokens)->type != TOKEN_CLOSE_PARENTHESIS
        || token_pop(tokens) == NULL)
        goto error;

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (!is_first_shell_command(tokens)
        || !parse_shell_command(tokens, new_parent))
        goto error;

    return 1;

error:
    return 0;
}

int parse_rule_if(struct tokenVect *tokens, struct ast *parent)
{
    // Create an IF node
    parent = addNode(parent, AST_IF, token_pop(tokens)->value);
    struct ast *new_parent = parent->children[parent->nb_children - 1];

    // Add if condition
    new_parent = addNode(new_parent, AST_CMD, "shell");
    struct ast *if_cond = new_parent->children[new_parent->nb_children - 1];
    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, if_cond))
        goto error;

    if (token_peek(tokens)->type != TOKEN_THEN) // Then
        goto error;
    token_pop(tokens); // No need to save it

    // Add this to Second left child
    new_parent = addNode(new_parent, AST_CMD, "shell");
    struct ast *then_cond = new_parent->children[new_parent->nb_children - 1];
    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, then_cond))
        goto error;

    if (is_first_else_clause(tokens)) // [ ELSE | ELIF ]
        if (!parse_else_clause(tokens, new_parent))
            goto error;

    if (token_peek(tokens)->type != TOKEN_FI) // FI
        goto error;
    token_pop(tokens);

    // Return the new if node
    return 1;

error:
    return 0;
}

int parse_else_clause(struct tokenVect *tokens, struct ast *parent)
{
    enum TokenType type = token_pop(tokens)->type;

    // Add Else / Elif node
    parent = addNode(parent, AST_CMD, "shell");
    struct ast *else_cond = parent->children[parent->nb_children - 1];
    if (!parse_compound_list(tokens, else_cond))
        goto error;

    if (type == TOKEN_ELIF)
    {
        if (token_peek(tokens)->type != TOKEN_THEN)
            goto error;
        token_pop(tokens);

        // Add Elif Then node
        parent = addNode(parent, AST_CMD, "shell");
        struct ast *elif_cond = parent->children[parent->nb_children - 1];
        if (!is_first_compound_list(tokens)
            || !parse_compound_list(tokens, elif_cond))
            goto error;

        if (token_peek(tokens)->type == TOKEN_ELSE
            || token_peek(tokens)->type == TOKEN_ELIF) // [ ELSE | ELIF ]
        {
            if (!is_first_else_clause(tokens)
                || !parse_else_clause(tokens, parent))
                goto error;
        }
        else
            goto error;
    }

    return 1;

error:
    return 0;
}

int parse_rule_while(struct tokenVect *tokens, struct ast *parent)
{
    parent = addNode(parent, AST_WHILE, token_pop(tokens)->value);
    struct ast *parent_while = parent->children[parent->nb_children - 1];
    parent_while = addNode(parent_while, AST_CMD, "shell");
    struct ast *while_cond =
        parent_while->children[parent_while->nb_children - 1];

    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, while_cond))
        goto error;

    if (token_peek(tokens)->type != TOKEN_DO)
        goto error;
    token_pop(tokens);

    parent_while = addNode(parent_while, AST_CMD, "shell");
    struct ast *while_exec =
        parent_while->children[parent_while->nb_children - 1];

    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, while_exec))
        goto error;

    if (token_peek(tokens)->type != TOKEN_DONE)
        goto error;
    token_pop(tokens);

    return 1;

error:
    return 0;
}

int parse_rule_until(struct tokenVect *tokens, struct ast *parent)
{
    parent = addNode(parent, AST_UNTIL, token_pop(tokens)->value);
    struct ast *parent_until = parent->children[parent->nb_children - 1];
    parent_until = addNode(parent_until, AST_CMD, "shell");
    struct ast *until_cond =
        parent_until->children[parent_until->nb_children - 1];

    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, until_cond))
        goto error;

    if (token_peek(tokens)->type != TOKEN_DO)
        goto error;
    token_pop(tokens);

    parent_until = addNode(parent_until, AST_CMD, "shell");
    struct ast *until_exec =
        parent_until->children[parent_until->nb_children - 1];

    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, until_exec))
        goto error;

    if (token_peek(tokens)->type != TOKEN_DONE)
        goto error;
    token_pop(tokens);

    return 1;

error:
    return 0;
}

int parse_rule_for_in(struct tokenVect *tokens, struct ast *parent)
{
    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (token_peek(tokens)->type != TOKEN_IN || token_pop(tokens) == NULL)
        goto error;

    if (is_valid_word(tokens))
    {
        if (token_peek(tokens)->type == TOKEN_VAR)
            addNode(parent, AST_EXP, token_pop(tokens)->value);
        else
            addNode(parent, AST_CMD, token_pop(tokens)->value);

        while (is_valid_word(tokens))
        {
            if (token_peek(tokens)->type == TOKEN_VAR)
                addNode(parent, PARAM_VAR, token_pop(tokens)->value);
            else
                addNode(parent, PARAM, token_pop(tokens)->value);
        }
    }
    else if (token_peek(tokens)->type == TOKEN_NEWLINE
             || token_peek(tokens)->type == TOKEN_SEMICOLON)
        addNode(parent, AST_CMD, "");
    else
        goto error;

    if ((token_peek(tokens)->type != TOKEN_NEWLINE
         && token_peek(tokens)->type != TOKEN_SEMICOLON)
        || token_pop(tokens) == NULL)
        goto error;
    return 1;

error:
    return 0;
}

int parse_rule_for(struct tokenVect *tokens, struct ast *parent)
{
    parent = addNode(parent, AST_FOR, token_pop(tokens)->value);
    struct ast *parent_for = parent->children[parent->nb_children - 1];

    if (token_peek(tokens)->type != TOKEN_WORD)
        goto error;

    if (token_peek(tokens)->type == TOKEN_VAR)
        addNode(parent_for, AST_EXP, token_pop(tokens)->value);
    else
        addNode(parent_for, AST_CMD, token_pop(tokens)->value);

    if (token_peek(tokens)->type == TOKEN_SEMICOLON)
        token_pop(tokens);
    else if (token_peek(tokens)->type == TOKEN_NEWLINE
             || token_peek(tokens)->type == TOKEN_IN)
    {
        if (!parse_rule_for_in(tokens, parent_for))
            goto error;
    }
    else
        goto error;

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens);

    if (token_peek(tokens)->type != TOKEN_DO || token_pop(tokens) == NULL)
        goto error;

    parent_for = addNode(parent_for, AST_CMD, "shell");
    struct ast *for_content = parent_for->children[parent_for->nb_children - 1];

    if (!is_first_compound_list(tokens)
        || !parse_compound_list(tokens, for_content))
        goto error;

    if (token_peek(tokens)->type != TOKEN_DONE || token_pop(tokens) == NULL)
        goto error;

    return 1;

error:
    return 0;
}

int parse_compound_list(struct tokenVect *tokens, struct ast *parent)
{
    while (token_peek(tokens)->type == TOKEN_NEWLINE) // SKIP ?
        token_pop(tokens);

    if (!is_first_and_or(tokens) || !parse_and_or(tokens, parent))
        goto error;

    while (token_peek(tokens)->type == TOKEN_SEMICOLON
           || token_peek(tokens)->type == TOKEN_NEWLINE)
    {
        token_pop(tokens);
        while (token_peek(tokens)->type == TOKEN_NEWLINE)
            token_pop(tokens);

        if (token_peek(tokens)->type == TOKEN_THEN
            || token_peek(tokens)->type == TOKEN_ELSE
            || token_peek(tokens)->type == TOKEN_ELIF
            || token_peek(tokens)->type == TOKEN_FI
            || token_peek(tokens)->type == TOKEN_DO
            || token_peek(tokens)->type == TOKEN_DONE
            || token_peek(tokens)->type == TOKEN_CLOSE_BRACKETS
            || token_peek(tokens)->type == TOKEN_CLOSE_PARENTHESIS
            || token_peek(tokens)->type == TOKEN_D_SEMICOLON)
            break;

        if (!is_first_and_or(tokens) || !parse_and_or(tokens, parent))
            goto error;
    }

    if (token_peek(tokens)->type == TOKEN_SEMICOLON)
        token_pop(tokens);

    while (token_peek(tokens)->type == TOKEN_NEWLINE)
        token_pop(tokens); // Skip

    return 1;

error:
    return 0;
}
