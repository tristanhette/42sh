#include "grammar.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

// Should return the AST
int parse_input(struct tokenVect *tokens, struct ast *parent)
{
    if (token_peek(tokens)->type == TOKEN_NEWLINE
        || token_peek(tokens)->type == TOKEN_EOF)
        return 1;

    // print_tokenVect(tokens);

    if (!parse_list(tokens, parent)) // Beginning of the grammar parsing
        goto error;

    if (token_peek(tokens)->type != TOKEN_EOF) // If remaining words not parsed
        goto error;

    // print_ast(parent, 9);
    return 1;

error:
    // print_ast(parent, 1);
    return 0;
}

// Returns True if accepted or False
int parse_list(struct tokenVect *tokens, struct ast *parent)
{
    if (!parse_and_or(tokens, parent))
        goto error;

    // Parse every element of the input
    while (token_peek(tokens)->type == TOKEN_NEWLINE
           || token_peek(tokens)->type == TOKEN_SEMICOLON)
    {
        token_pop(tokens);
        while (token_peek(tokens)->type == TOKEN_NEWLINE)
            token_pop(tokens);

        if (token_peek(tokens)->type == TOKEN_EOF)
            return 1;

        if (!is_first_and_or(tokens) || !parse_and_or(tokens, parent))
            goto error;
    }

    return 1;

error:
    return 0;
}

int parse_and_or(struct tokenVect *tokens, struct ast *parent)
{
    if (!is_first_pipeline(tokens) || !parse_pipeline(tokens, parent))
        goto error;

    enum TokenType type = token_peek(tokens)->type;

    while (type == TOKEN_AND || type == TOKEN_OR)
    {
        // Add AND node or OR node depending on type variable
        if (type == TOKEN_AND)
        {
            parent = add_parent(parent, AST_CMD, "shell");
            parent = add_parent(parent, AST_AND, token_pop(tokens)->value);
        }
        else
        {
            parent = add_parent(parent, AST_CMD, "shell");
            parent = add_parent(parent, AST_OR, token_pop(tokens)->value);
        }

        struct ast *new_parent = parent->children[parent->nb_children - 1];
        new_parent = addNode(new_parent, AST_CMD, "shell");
        struct ast *parent_and_or =
            new_parent->children[new_parent->nb_children - 1];

        while (token_peek(tokens)->type == TOKEN_NEWLINE)
            token_pop(tokens);

        if (!is_first_pipeline(tokens)
            || !parse_pipeline(tokens, parent_and_or))
            goto error;

        type = token_peek(tokens)->type;
    }

    return 1;

error:
    return 0;
}

int parse_pipeline(struct tokenVect *tokens, struct ast *parent)
{
    // Return the node
    struct ast *neg_parent = parent;
    if (token_peek(tokens)->type == TOKEN_NEG)
    {
        parent = addNode(parent, AST_NEG, token_pop(tokens)->value);
        struct ast *new_parent = parent->children[parent->nb_children - 1];
        new_parent = addNode(new_parent, AST_CMD, "shell");
        neg_parent = new_parent->children[new_parent->nb_children - 1];
    }

    if (!is_first_command(tokens) || !parse_command(tokens, neg_parent))
        goto error;

    while (token_peek(tokens)->type == TOKEN_PIPE)
    {
        // Create a pipe node

        parent = add_parent(parent, AST_CMD, "shell");
        parent = add_parent(parent, AST_PIPE, token_pop(tokens)->value);
        struct ast *new_parent = parent->children[parent->nb_children - 1];
        new_parent = addNode(new_parent, AST_CMD, "shell");
        struct ast *pipe_parent =
            new_parent->children[new_parent->nb_children - 1];

        while (token_peek(tokens)->type == TOKEN_NEWLINE)
            token_pop(tokens);

        if (!is_first_command(tokens) || !parse_command(tokens, pipe_parent))
            goto error;
    }

    return 1;

error:
    return 0;
}

// Return a CMD node
int parse_command(struct tokenVect *tokens, struct ast *parent)
{
    if (is_first_simple_command(tokens))
    {
        if (!parse_simple_command(tokens, parent))
            goto error;
    }
    else
    {
        if (is_first_shell_command(tokens))
        {
            if (!parse_shell_command(tokens, parent))
                goto error;
        }
        else if (is_first_funcdec(tokens))
        {
            if (!parse_funcdec(tokens, parent))
                goto error;
        }
        else
            goto error;

        while (is_first_redirection(tokens))
        {
            if (!parse_redirection(tokens, parent))
                goto error;
        }
    }
    return 1;

error:
    return 0;
}

int parse_simple_command(struct tokenVect *tokens, struct ast *parent)
{
    int prefix_found = 0;
    size_t backup_index = parent->nb_children;

    if (is_first_prefix(tokens))
    {
        prefix_found = 1;
        while (parse_prefix(tokens, parent))
            continue; // ??

        if (token_peek(tokens)->type != TOKEN_WORD
            && token_peek(tokens)->type != TOKEN_VAR)
            goto success;
    }

    if (!prefix_found && token_peek(tokens)->type != TOKEN_WORD
        && token_peek(tokens)->type != TOKEN_VAR)
        goto error;

    while (backup_index < parent->nb_children)
    {
        if (parent->children[backup_index]->type == AST_VAR)
            delete_child(parent, backup_index);
        ++backup_index;
    }

    if (parent->type == AST_IF)
        addNode(parent, AST_LIST, token_pop(tokens)->value);
    else
    {
        if (token_peek(tokens)->type == TOKEN_VAR)
            addNode(parent, AST_EXP, token_pop(tokens)->value);
        else
            addNode(parent, AST_CMD, token_pop(tokens)->value);
    }

    while (parse_element(tokens, parent))
        continue;

success:
    return 1;

error:
    return 0;
}

static int get_index_equal(char *name)
{
    int index = -1;
    while (name[++index] != '=')
        ;

    name[index] = 0;
    return index;
}

int parse_prefix(struct tokenVect *tokens, struct ast *parent)
{
    // Create ASSIGNMENT WORD if any
    if (token_peek(tokens)->type == TOKEN_ASSIGNMENT_WORD)
    {
        char *var_name = token_pop(tokens)->value;
        int index = get_index_equal(var_name);
        char *var_value = var_name + index + 1;

        addNode(parent, AST_VAR, var_name);
        if (token_peek(tokens)->type == TOKEN_VAR)
            addNode(parent, PARAM_VAR, var_value);
        else
            addNode(parent, PARAM, var_value);
        while (token_peek(tokens)->type == TOKEN_CONCAT)
        {
            addNode(parent, PARAM_CONCAT, token_pop(tokens)->value);
            if (token_peek(tokens)->type == TOKEN_VAR)
                addNode(parent, PARAM_VAR, token_pop(tokens)->value);
            else
                addNode(parent, PARAM, token_pop(tokens)->value);
        }
    }
    else if (is_first_redirection(tokens))
    {
        if (!parse_redirection(tokens, parent))
            goto error;
    }
    else
        goto error;

    return 1;

error:
    return 0;
}

int parse_redirection(struct tokenVect *tokens, struct ast *parent)
{
    char *io_number = NULL;
    if (token_peek(tokens)->type == TOKEN_IONUMBER)
        io_number = token_pop(tokens)->value;

    if (!is_redirection_token(tokens))
        goto error;

    if (parent->children)
    {
        parent = add_parent(parent, AST_CMD, "shell");
        parent = add_parent(parent, AST_REDIR, token_pop(tokens)->value);
    }
    else
        addNode(parent, AST_REDIR, token_pop(tokens)->value);

    struct ast *new_parent = parent->children[parent->nb_children - 1];

    if (io_number)
        addNode(new_parent, AST_CMD, io_number);

    if (token_peek(tokens)->type != TOKEN_WORD
        && token_peek(tokens)->type != TOKEN_VAR)
        goto error;
    addNode(new_parent, AST_CMD, token_pop(tokens)->value);

    return 1;

error:
    return 0;
}

int parse_element(struct tokenVect *tokens, struct ast *parent)
{
    enum TokenType type = token_peek(tokens)->type;
    if (is_valid_word(tokens))
    {
        struct ast *child = parent->children[parent->nb_children - 1];
        if (child->type != AST_REDIR)
            child = parent;
        while (child->type == AST_REDIR)
            child = child->children[0];
        addNode(child,
                type == TOKEN_VAR          ? PARAM_VAR
                    : type == TOKEN_CONCAT ? PARAM_CONCAT
                                           : PARAM,
                token_pop(tokens)->value);
    }
    else if (is_first_redirection(tokens))
    {
        if (!parse_redirection(tokens, parent))
            goto error;
    }
    else
        goto error;

    return 1;

error:
    return 0;
}
