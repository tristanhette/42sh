#ifndef FONCTION_H
#define FONCTION_H

#include "../ast/ast.h"

struct fonction
{
    struct ast *node;
    struct tokenVect *tokens;
    char *name;
};

struct fonctions
{
    struct fonction **data;
    int len;
    int capacity;
};

struct fonctions *init_fonction_table(void);
void add_fonction(char *name, struct ast *node, struct tokenVect *tokens);
struct ast *get_fonction(char *name);
void remove_fonction(char *name);
void free_fonctions_table(struct fonctions *table);

#endif /* ! FONCTION_H */
