#include "fonction.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token.h"
#include "../variables/variables.h"

struct fonctions *init_fonction_table(void)
{
    struct fonctions *table = malloc(sizeof(struct fonctions));
    if (table != NULL)
    {
        table->data = NULL;
        table->len = 0;
        table->capacity = 0;
    }
    return table;
}

void add_fonction(char *name, struct ast *node, struct tokenVect *tokens)
{
    if (get_fonction(name))
        remove_fonction(name); // Replace if already existing
    struct fonctions *fonction_table = commandLineInfo.fonction_table;
    if (fonction_table->len == fonction_table->capacity)
    {
        fonction_table->capacity += 10;
        fonction_table->data =
            realloc(fonction_table->data,
                    fonction_table->capacity * sizeof(struct fonction *));
    }
    fonction_table->data[fonction_table->len] = malloc(sizeof(struct fonction));
    fonction_table->data[fonction_table->len]->name = copy_string(name);
    fonction_table->data[fonction_table->len]->node = node;
    fonction_table->data[fonction_table->len]->tokens = tokens;
    fonction_table->len++;
}

struct ast *get_fonction(char *name)
{
    struct fonctions *fonction_table = commandLineInfo.fonction_table;
    for (int i = 0; i < fonction_table->len; i++)
    {
        if (strcmp(fonction_table->data[i]->name, name) == 0)
            return fonction_table->data[i]->node;
    }
    return NULL;
}

void remove_fonction(char *name)
{
    struct fonctions *fonction_table = commandLineInfo.fonction_table;
    for (int i = 0; i < fonction_table->len; i++)
    {
        if (strcmp(fonction_table->data[i]->name, name) == 0)
        {
            if (fonction_table->data[i]->tokens)
            {
                free_tokenVect(fonction_table->data[i]->tokens);
                free_ast(fonction_table->data[i]->node);
            }
            free(fonction_table->data[i]->name);
            free(fonction_table->data[i]);
            for (int j = i; j < fonction_table->len - 1; j++)
            {
                fonction_table->data[j] = fonction_table->data[j + 1];
            }
            fonction_table->len--;
            // si on veut reduire la capaciter
            if (fonction_table->len > 0
                && fonction_table->len < fonction_table->capacity / 2)
            {
                fonction_table->capacity /= 2;
                fonction_table->data = realloc(fonction_table->data,
                                               fonction_table->capacity
                                                   * sizeof(struct fonction *));
            }
            return;
        }
    }
}

void free_fonctions_table(struct fonctions *table)
{
    if (table == NULL)
        return;
    for (int i = 0; i < table->len; i++)
    {
        if (table->data[i]->tokens)
        {
            free_tokenVect(table->data[i]->tokens);
            free_ast(table->data[i]->node);
        }
        free(table->data[i]->name);
        free(table->data[i]);
    }
    free(table->data);
    free(table);
}
