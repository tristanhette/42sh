#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtins/builtins.h"

int redirect_duplicate(int oldfd, int newfd, struct ast *tree)
{
    if (newfd == -1)
        errx(2, "Redirect: New Field Descriptor not correct.");

    int savedfd = dup(oldfd);

    dup2(newfd, oldfd);
    int return_value = evaluate_node(tree);

    dup2(savedfd, oldfd);
    close(savedfd);
    close(newfd);

    return return_value;
}

int redirect(int oldfd, int newfd, struct ast *tree)
{
    if (newfd == -1)
        errx(2, "Redirect: New Field Descriptor not correct.");

    int savedfd = dup(oldfd);

    dup2(newfd, oldfd);
    int return_value = evaluate_node(tree);
    close(newfd);

    dup2(savedfd, oldfd);
    close(savedfd);

    return return_value;
}

int find_fd(char *file)
{
    int i = 0;
    while (file[i] != 0)
        i++;
    int fd = atoi(file);

    if (fd != 0 || (i == 1 && file[0] == '0'))
        return fd;
    return -1;
}

int redir_builtin(struct ast *tree)
{
    struct ast *cmp = tree->children[0];
    char *arg_symbol = tree->command->name;
    if (tree->nb_children == 3)
    {
        int io_number = atoi(tree->children[1]->command->name);
        char *arg_file = tree->children[2]->command->name;
        return exec_redir(cmp, io_number, arg_symbol, arg_file);
    }
    else if (tree->nb_children == 2)
    {
        char *arg_file = tree->children[1]->command->name;
        return exec_redir(cmp, -1, arg_symbol, arg_file);
    }
    else
        return 0;
}

int exec_redir2(struct ast *tree, int oldfd, char *symbol, char *file)
{
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if (strcmp(symbol, ">|") == 0)
    {
        if ((fd = find_fd(file)) == -1)
            fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
        return redirect(oldfd == -1 ? STDOUT_FILENO : oldfd, fd, tree);
    }
    else if (strcmp(symbol, "<") == 0)
    {
        if ((fd = find_fd(file)) == -1)
            fd = open(file, O_RDONLY);
        return redirect(oldfd == -1 ? STDIN_FILENO : oldfd, fd, tree);
    }
    else if (strcmp(symbol, ">>") == 0)
    {
        if ((fd = find_fd(file)) == -1)
        {
            if ((fd = open(file, O_WRONLY | O_APPEND)) == -1)
                fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
        }
        return redirect(oldfd == -1 ? STDOUT_FILENO : oldfd, fd, tree);
    }
    else if (strcmp(symbol, ">&") == 0)
    {
        if (strcmp(file, "-"))
        {
            close(oldfd);
            return evaluate_node(tree);
        }
        else
        {
            if ((fd = find_fd(file)) == -1)
                fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
            return redirect_duplicate(oldfd == -1 ? STDOUT_FILENO : oldfd, fd,
                                      tree);
        }
    }
    return 1;
}

int exec_redir(struct ast *tree, int oldfd, char *symbol, char *file)
{
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if (strcmp(symbol, ">") == 0)
    {
        if ((fd = find_fd(file)) == -1)
            fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, mode);
        return redirect(oldfd == -1 ? STDOUT_FILENO : oldfd, fd, tree);
    }
    else if (strcmp(symbol, "<&") == 0)
    {
        if (strcmp(file, "-") == 0)
        {
            close(oldfd);
            return evaluate_node(tree);
        }
        else
        {
            if ((fd = find_fd(file)) == -1)
                fd = open(file, O_RDONLY);
            return redirect_duplicate(oldfd == -1 ? STDIN_FILENO : oldfd, fd,
                                      tree);
        }
    }
    else if (strcmp(symbol, "<>") == 0)
    {
        if ((fd = find_fd(file)) == -1)
            fd = open(file, O_CREAT | O_RDWR, mode);
        return redirect(oldfd == -1 ? STDOUT_FILENO : oldfd, fd,
                        tree); // OUT by default. Need to check this
    }
    else
        return exec_redir2(tree, oldfd, symbol, file);
}
