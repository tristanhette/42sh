#ifndef REDIRS_H
#define REDIRS_H

#include "../ast/ast.h"

int redirect(int oldfd, int newfd, struct ast *tree);
int redir_builtin(struct ast *tree);
int exec_redir2(struct ast *tree, int oldfd, char *symbol, char *file);
int exec_redir(struct ast *tree, int oldfd, char *symbol, char *file);
#endif /* !REDIRS_H */
