#include "ast_exec.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void print_ast(struct ast *node, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    if (node->command != NULL)
    {
        char *conName = concat_args(node->command);
        printf("  %d: %s %s\n", node->type, node->command->name, conName);
        free(conName);
    }
    for (size_t i = 0; i < node->nb_children; i++)
    {
        print_ast(node->children[i], depth + 1);
    }
}

int eval_if(struct ast *node)
{
    int a = 0;
    for (size_t i = 0; i < node->nb_children; i++)
    {
        int testChild = evaluate_node(node->children[i]);
        if (testChild == 0 && i + 1 != node->nb_children)
        {
            i++;
            int a = evaluate_node(node->children[i]);
            return a;
        }
        if (testChild != 0)
            i++;
        a = testChild;
    }
    if (node->nb_children == 2)
        a = !a;
    return a;
}

int eval_shell(struct ast *node)
{
    int a = 0;
    for (size_t i = 0; i < node->nb_children; i++)
    {
        a = evaluate_node(node->children[i]);
        if (commandLineInfo.n_continue != 0 || commandLineInfo.n_break != 0)
            break;
    }
    return a;
}

int eval_zombie(struct Command *cmd)
{
    char **com = concat_com_in_list(cmd);
    while (**com != 0
           && (**com == '\t'
               || **com
                   == ' ')) // Shifts the pointer while there are spaces or tabs
        (*com)++;
    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
        if (execvp(com[0], com) == -1)
        {
            exit(127);
        }
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent process
        int status;
        waitpid(pid, &status, 0);
        // check if the command was successful
        int exit_status = WEXITSTATUS(status);
        if (WIFEXITED(status)
            && (exit_status == 0 || exit_status == 1)) // Case when true/false
        {
            free_com_in_list(com);
            return exit_status;
        }
        else
        {
            // Command failed, return failure
            free_com_in_list(com);
            fprintf(stderr, "%s: command not found\n", cmd->name);
            return 127;
        }
    }
    else
    {
        free_com_in_list(com);
        perror("Erreur fork process");
        exit(EXIT_FAILURE);
    }
}

static int eval_function(struct Command *cmd, struct ast *fonction_node)
{
    int return_value = 0;
    struct CommandLineInfo backup = commandLineInfo;
    char **argv = concat_com_in_list(cmd);

    commandLineInfo.argv = argv;
    commandLineInfo.argc = 1;

    while (argv[commandLineInfo.argc] != NULL)
        commandLineInfo.argc++;

    return_value = evaluate_node(fonction_node);

    free_com_in_list(argv);
    commandLineInfo = backup;

    return return_value;
}

int evaluate_node(struct ast *node)
{
    // check peut etre les variables a expand
    if (node == NULL || node->command == NULL)
        return true;

    // Skip la root car c'est un if
    if (node->type == AST_IF)
        return eval_if(node);
    else if (strcmp(node->command->name, "shell") == 0)
        return eval_shell(node);

    int return_value = 0;
    // execution de la commande du noeud
    struct Command *cmd = expand_command(node->command);
    struct ast *fonction_node = get_fonction(cmd->name);
    if (node->type == AST_FONCTION)
        return_value = fonction_builtin(node);
    else if (fonction_node != NULL)
        return_value = eval_function(cmd, fonction_node);
    else if (strcmp(cmd->name, "echo") == 0)
        return_value = echo_builtin(cmd);
    else if (strcmp(cmd->name, "cd") == 0)
        return_value = cd_builtin(cmd);
    else if (strcmp(cmd->name, "true") == 0)
        return_value = true_builtin();
    else if (strcmp(cmd->name, "false") == 0)
        return_value = false_builtin();
    else if (strcmp(cmd->name, "exit") == 0)
        return_value = exit_builtin(cmd);
    else if (strcmp(cmd->name, "unset") == 0)
        return_value = unset_builtin(cmd);
    else if (strcmp(cmd->name, ".") == 0)
        return_value = dot_builtin(cmd);
    else
        return_value = evaluate_node2(node, cmd, return_value);

    destroy_command(cmd);
    return return_value;
}

int evaluate_node2(struct ast *node, struct Command *cmd, int return_value)
{
    if (node->type == AST_VAR)
        return_value = var_builtin(cmd);
    else if (strcmp(cmd->name, "continue") == 0)
        return_value = continue_builtin(node);
    else if (strcmp(cmd->name, "alias") == 0)
        return_value = alias_builtin(node);
    else if (strcmp(cmd->name, "break") == 0)
        return_value = break_builtin(node);
    else if (node->type == AST_PIPE)
        return_value = pipe_builtin(node->children[0], node->children[1]);
    else if (node->type == AST_REDIR)
        return_value = redir_builtin(node);
    else if (node->type == AST_OR)
        return_value = !or_builtin(node->children[0], node->children[1]);
    else if (node->type == AST_AND)
        return_value = !and_builtin(node->children[0], node->children[1]);
    else if (node->type == AST_NEG)
        return_value = !eval_shell(node);
    else if (node->type == AST_WHILE)
        return_value = while_builtin(node->children[0], node->children[1]);
    else if (node->type == AST_UNTIL)
        return_value = until_builtin(node->children[0], node->children[1]);
    else if (node->type == AST_FOR)
        return_value = for_builtin(node);
    else if (node->type == AST_SUB)
        return_value = subshell_builtin(node);
    else if (strcmp(cmd->name, "export") == 0)
        return_value = export_builtin(node);
    else if (node->type == AST_CASE)
        return_value = case_builtin(node, node->nb_children);
    else
        return_value = eval_zombie(cmd);
    return return_value;
}

int evaluate_ast(struct ast *root)
{
    if (root != NULL)
    {
        return evaluate_node(root);
    }
    return 0;
}
