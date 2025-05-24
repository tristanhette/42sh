#ifndef BUILTINS_NEXT_H
#define BUILTINS_NEXT_H

#include "../ast/ast.h"
#include "../ast/command.h"

int eval_exit(int status);
int exit_builtin(struct Command *cmd);
int cd_builtin(struct Command *cmd);
int exec_cd(char *path);
int cd_relative_paths(char *path, char *pwd);
int subshell_builtin(struct ast *node);
int unset_var(struct Command *com);
int unset_func(struct Command *com);
int unset_builtin(struct Command *com);
int dot_builtin(struct Command *com);

#endif /* !BUILTINS_NEXT_H */
