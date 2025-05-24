#ifndef BUILTINS_H
#define BUILTINS_H

#include "../ast/ast.h"

int true_builtin(void);
int false_builtin(void);
int echo_builtin(struct Command *cmd);
int exec_echo(char option, char *string);
int pipe_builtin(struct ast *node1, struct ast *node2);
int negation_builtin(struct ast *node);
int and_builtin(struct ast *node1, struct ast *node2);
int or_builtin(struct ast *node1, struct ast *node2);

#endif /* !BUILTINS_H */
