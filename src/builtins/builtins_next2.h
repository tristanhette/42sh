#ifndef BUILTINS_NEXT2_H
#define BUILTINS_NEXT2_H

#include "../ast/ast.h"
#include "../ast/command.h"

int export_builtin(struct ast *node);
int continue_builtin(struct ast *node);
int break_builtin(struct ast *node);
int alias_builtin(struct ast *node);
int fonction_builtin(struct ast *node);
int for_builtin(struct ast *parent);
int until_builtin(struct ast *condition, struct ast *execution);
int while_builtin(struct ast *condition, struct ast *execution);
int exec_for(char *child1, char **child2, struct ast *child3, int nbchild);
int case_builtin(struct ast *tree, int nbchild);

#endif /* !BUILTINS_NEXT2_H */
