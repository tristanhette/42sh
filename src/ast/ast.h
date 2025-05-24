#ifndef AST_H
#define AST_H

struct ast;
struct Command;

#include <stdbool.h>
#include <stdlib.h>

#include "../builtins/builtins.h"
#include "ast_exec.h"
#include "command.h"

enum ast_type
{
    AST_CMD,
    AST_IF,
    AST_LIST,
    AST_PIPE,
    AST_REDIR,
    AST_NEG,
    AST_OR,
    AST_FOR,
    AST_AND,
    AST_WHILE,
    AST_UNTIL,
    AST_VAR,
    AST_SUB,
    AST_EXP,
    AST_EXIT,
    AST_UNSET,
    AST_DOT,
    AST_FONCTION,
    AST_CASE,
    PARAM,
    PARAM_VAR,
    PARAM_CONCAT
};

struct ast
{
    enum ast_type type; // type of the node
    size_t nb_children; // size of children
    struct ast **children; // array of children
    struct Command *command; // data contained in the node
};

struct ast *create_node(enum ast_type type, char *data);
struct ast *addNode(struct ast *parent, enum ast_type type, char *cmd);
struct ast *addRootNode(struct ast *oldRoot, enum ast_type type, char *cmd);
void free_command(struct Command *cmd);
void free_ast_node(struct ast *node);
void free_ast(struct ast *root);
struct ast *child_to_root(struct ast *root, struct ast *child);
struct ast *add_parent(struct ast *parent, enum ast_type tipe, char *cmd);
void add_child(struct ast *parent, struct ast *child);
void delete_child(struct ast *parent, size_t index);

#endif /* ! AST_H */
