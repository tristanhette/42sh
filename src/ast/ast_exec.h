#ifndef AST_EXEC_H
#define AST_EXEC_H

#include <stdbool.h>
#include <stdlib.h>

#include "../builtins/builtins.h"
#include "../builtins/builtins_next.h"
#include "../builtins/builtins_next2.h"
#include "../redirs/redirs.h"
#include "../variables/variables.h"
#include "ast.h"
#include "command.h"

void print_ast(struct ast *node, int depth);
int eval_if(struct ast *node);
int eval_zombie(struct Command *cmd);
int evaluate_node(struct ast *node);
int evaluate_node2(struct ast *node, struct Command *cmd, int return_value);
int evaluate_ast(struct ast *root);

#endif /* ! AST_EXEC_H */
