#include "ast.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtins/builtins.h"

// fonction pour creer un noeud
struct ast *create_node(enum ast_type type, char *data)
{
    struct ast *newNode = malloc(sizeof(struct ast));
    struct Command *newCom = createCommand(data);
    newNode->type = type;
    newNode->nb_children = 0;
    newNode->children = NULL;
    newNode->command = newCom;
    return newNode;
}

// fonction pour ajouter un noeud fils
struct ast *addNode(struct ast *parent, enum ast_type type, char *cmd)
{
    if (type == PARAM || type == PARAM_CONCAT || type == PARAM_VAR)
    {
        if (parent->nb_children > 0)
        {
            struct Command *lastChildCommand =
                parent->children[parent->nb_children - 1]->command;
            if (lastChildCommand != NULL)
            {
                lastChildCommand =
                    stack_push(lastChildCommand,
                               type == PARAM_VAR          ? CMD_VAR
                                   : type == PARAM_CONCAT ? CMD_CONCAT
                                                          : CMD_WORD,
                               cmd);
            }
        }
    }
    else
    {
        struct ast *childNode = create_node(type, cmd);
        if (type == AST_EXP)
            childNode->command->type = CMD_VAR;
        parent->nb_children++;
        parent->children = realloc(parent->children,
                                   sizeof(struct ast *) * parent->nb_children);
        parent->children[parent->nb_children - 1] = childNode;
    }
    return parent;
}
// fonction pour ajouter un noeud en root
struct ast *addRootNode(struct ast *oldRoot, enum ast_type type, char *cmd)
{
    struct ast *newRoot = create_node(type, cmd);
    newRoot->nb_children++;
    newRoot->children =
        realloc(newRoot->children, sizeof(struct ast *) * newRoot->nb_children);
    newRoot->children[0] = oldRoot;
    return newRoot;
}

void free_ast_node(struct ast *node)
{
    if (node != NULL)
    {
        free_command(node->command);

        for (size_t i = 0; i < node->nb_children; i++)
        {
            free_ast_node(node->children[i]);
        }
        free(node->children);
        free(node);
    }
}

void free_ast(struct ast *root)
{
    free_ast_node(root);
}

struct ast *child_to_root(struct ast *root, struct ast *child)
{
    root->nb_children++;
    root->children =
        realloc(root->children, sizeof(struct ast *) * root->nb_children);
    root->children[root->nb_children - 1] = child;
    return root;
}

struct ast *add_parent(struct ast *parent, enum ast_type tipe, char *cmd)
{
    struct ast *node_pipe = create_node(tipe, cmd);
    struct ast *tmp = parent->children[parent->nb_children - 1];
    add_child(node_pipe, tmp);
    parent->children[parent->nb_children - 1] = node_pipe;
    return parent;
}

void add_child(struct ast *parent, struct ast *child)
{
    parent->nb_children++;
    parent->children =
        realloc(parent->children, sizeof(struct ast *) * parent->nb_children);
    parent->children[parent->nb_children - 1] = child;
}

void delete_child(struct ast *parent, size_t index)
{
    if (index >= parent->nb_children)
        return;

    free_ast_node(parent->children[index]);
    while (index + 1 < parent->nb_children)
    {
        parent->children[index] = parent->children[index + 1];
        ++index;
    }

    parent->nb_children--;
    parent->children =
        realloc(parent->children, parent->nb_children * sizeof(struct ast *));
}
