#define _DEFAULT_SOURCE
#include "builtins_next2.h"

#include <err.h>
#include <fnmatch.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../parser/parser.h"
#include "utils.h"

int export_builtin(struct ast *node)
{
    char *param = concat_args(node->command);
    char *value = strchr(param, '=');
    if (value)
    {
        *value = 0;
        value++;
        setenv(param, value, 1);
    }
    else
    {
        if (!getenv(param))
        {
            fprintf(stderr, "export: `%s` not a valide identifier.\n", param);
            free(param);
            return 1;
        }
        setenv(node->command->name, param, 1);
    }
    free(param);
    return 0;
}

int continue_builtin(struct ast *node)
{
    if (node->command->next)
        commandLineInfo.n_continue = atoi(node->command->next->name) + 1;
    else
        commandLineInfo.n_continue = 1;
    return 0;
}

int break_builtin(struct ast *node)
{
    if (node->command->next)
        commandLineInfo.n_break = atoi(node->command->next->name);
    else
        commandLineInfo.n_break = 1;
    return 0;
}

int alias_builtin(struct ast *node)
{
    struct Command *com = node->command;
    if (com->next->next)
    {
        struct Command *to_eval = expand_command(com->next->next->next);
        char *string = concat_args(to_eval);
        if (*string == 0)
        {
            free(string);
            string = to_eval->name;
        }
        struct tokenVect *tokens = lexer(string);
        struct ast *new_node = parse(tokens);

        char *name = node->command->next->name;
        name[strlen(name) - 1] = 0;

        add_fonction(name, new_node, tokens);
        destroy_command(to_eval);
        return 0;
    }
    else
    {
        fprintf(stderr, "alias: not found");
        return 1;
    }
}

int fonction_builtin(struct ast *node)
{
    // struct ast *node_fonction = get_fonction(node->command->name);
    // int res = evaluate_node(node_fonction);
    add_fonction(node->command->name, node->children[0], NULL);
    return 0;
}

int while_builtin(struct ast *condition, struct ast *execution)
{
    int a = 0;
    while (evaluate_node(condition) == 0)
    {
        if (commandLineInfo.n_continue > 1)
        {
            commandLineInfo.n_continue -= 1;
            continue;
        }
        else if (commandLineInfo.n_continue == 1)
            commandLineInfo.n_continue = 0;
        if (commandLineInfo.n_break > 0)
        {
            commandLineInfo.n_break -= 1;
            break;
        }
        a = evaluate_node(execution);
    }
    if ((a == 568 && commandLineInfo.n_break == 0)
        || (a == 567 && commandLineInfo.n_continue == 0))
        a = 0;
    return a;
}

int until_builtin(struct ast *condition, struct ast *execution)
{
    int a = 0;
    while (evaluate_node(condition) != 0)
    {
        if (commandLineInfo.n_continue > 1)
        {
            commandLineInfo.n_continue -= 1;
            continue;
        }
        else if (commandLineInfo.n_continue == 1)
            commandLineInfo.n_continue = 0;
        if (commandLineInfo.n_break > 0)
        {
            commandLineInfo.n_break -= 1;
            break;
        }
        a = evaluate_node(execution);
    }
    if ((a == 568 && commandLineInfo.n_break == 0)
        || (a == 567 && commandLineInfo.n_continue == 0))
        a = 0;
    return a;
}

int for_builtin(struct ast *parent)
{
    if (parent->nb_children == 2)
        return exec_for(parent->children[0]->command->name,
                        commandLineInfo.argv, parent->children[1],
                        parent->nb_children);

    struct Command *cmd = expand_command(parent->children[1]->command);

    int return_value =
        exec_for(parent->children[0]->command->name, concat_com_in_list(cmd),
                 parent->children[2], parent->nb_children);

    destroy_command(cmd);
    return return_value;
}

int exec_for(char *child1, char **child2, struct ast *child3, int nbchild)
{
    int a = 0;
    for (int i = 0; child2[i] != NULL; i++)
    {
        if (commandLineInfo.n_continue > 1)
        {
            commandLineInfo.n_continue -= 1;
            continue;
        }
        else if (commandLineInfo.n_continue == 1)
            commandLineInfo.n_continue = 0;
        if (commandLineInfo.n_break > 0)
        {
            commandLineInfo.n_break -= 1;
            break;
        }
        setenv(child1, child2[i], 1);
        a = evaluate_node(child3);
        // add_var(child1, child2[i]);
    }
    if (nbchild > 2)
        free_com_in_list(child2);
    if ((a == 568 && commandLineInfo.n_break == 0)
        || (a == 567 && commandLineInfo.n_continue == 0))
        a = 0;
    return a;
}

int case_builtin(struct ast *tree, int nbchild)
{
    if (nbchild < 1)
        return 2;

    struct Command *tree_cmd = expand_command(tree->children[0]->command);
    for (int i = 1; i < nbchild; i++)
    {
        struct Command *expanded = expand_command(tree->children[i]->command);
        struct Command *cmd = expanded;
        do
        {
            char *word = cmd->name;
            if (fnmatch(word, tree_cmd->name, 0) == 0 || strcmp("*", word) == 0)
            {
                destroy_command(tree_cmd);
                destroy_command(expanded);
                int return_value = 0;
                for (size_t k = 0; k < tree->children[i]->nb_children; k++)
                    return_value =
                        evaluate_node(tree->children[i]->children[k]);
                return return_value;
            }
            cmd = cmd->next;
        } while (cmd);

        destroy_command(expanded);
    }

    destroy_command(tree_cmd);
    return 0;
}
