#define _DEFAULT_SOURCE
#include "builtins.h"

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int true_builtin(void)
{
    return 0;
}

int false_builtin(void)
{
    return 1;
}

int echo_builtin(struct Command *cmd)
{
    char *conName = concat_args(cmd);
    int a = 0;
    if (cmd->next == NULL)
    {
        free(conName);
        return exec_echo(0, "");
    }
    if (cmd->next->next != NULL)
    {
        struct Command *cpy = cmd;

        char option;
        while ((option = check_option(cmd->next->name)) != 0)
        {
            cpy = cmd;
            cmd = cmd->next;
        }
        option = check_option(cmd->name);
        char *str;
        if (option != 0)
        {
            str = concat_args(cpy->next);
            a = exec_echo(option, str);
        }
        else
        {
            str = concat_args(cpy);
            a = exec_echo(option, str);
        }

        if (strcmp(str, "") != 0)
            free(str);
        else
            a = exec_echo(0, conName);
    }
    else
        a = exec_echo(0, conName);
    free(conName);
    return a;
}

int exec_echo(char option, char *string)
{
    if (option == 0)
    {
        puts(string);
    }
    else if (option == 'n')
    {
        printf("%s", string);
    }
    else if (option == 'e')
    {
        char *str = detect_escapes(string);
        int i = 0;
        while (str[i] != 0)
        {
            putchar(str[i]);
            i++;
        }
        putchar('\n');
        free(str);
    }
    else if (option == 'E')
    {
        int i = 0;
        while (string[i] != 0)
        {
            if (string[i] == '\n')
            {
                putchar('\\');
                putchar('n');
            }
            else if (string[i] == '\t')
            {
                putchar('\\');
                putchar('t');
            }
            else
            {
                putchar(string[i]);
            }
            i++;
        }
        putchar('\n');
    }
    else
        puts(string);
    return fflush(stdout);
}

int pipe_builtin(struct ast *node1, struct ast *node2)
{
    int fd[2];
    pid_t pid;
    if (pipe(fd) == -1)
        return 1;

    pid = fork();
    if (pid == -1)
        errx(1, "Pipe: could not create fork.");

    if (pid == 0)
    {
        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        exit(evaluate_node(node1));
    }
    else
    {
        close(fd[1]);

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        return evaluate_node(node2);
    }
}

int negation_builtin(struct ast *node)
{
    return !evaluate_node(node);
}

int and_builtin(struct ast *node1, struct ast *node2)
{
    return !evaluate_node(node1) && !evaluate_node(node2);
}

int or_builtin(struct ast *node1, struct ast *node2)
{
    return !evaluate_node(node1) || !evaluate_node(node2);
}
