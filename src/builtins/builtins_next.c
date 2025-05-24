#define _DEFAULT_SOURCE
#include "builtins_next.h"

#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../exec_main.h"
#include "utils.h"

int exit_builtin(struct Command *cmd)
{
    if (cmd->next == NULL)
    {
        exit(0);
    }
    if (cmd->next->next == NULL)
    {
        int status;
        if ((status = my_atoi(cmd->next->name)) != -1)
        {
            if (status > 255)
                exit(255);
            exit(status);
        }
        else
            errx(2, "exit: %s: numeric argument required", cmd->next->name);
    }
    else
    {
        errx(1, "exit: too many arguments");
    }
}
int cd_builtin(struct Command *cmd)
{
    if (cmd->next == NULL)
    {
        return exec_cd(NULL);
    }
    else if (cmd->next->next != NULL)
    {
        errx(1, "cd: too many arguments");
    }
    else
    {
        int len = strlen(cmd->next->name);
        if (cmd->next->name[len - 1] == '/' && len > 1)
        {
            cmd->next->name[len - 1] = 0;
        }
        return exec_cd(cmd->next->name);
    }
}

int exec_cd(char *path)
{
    if (path == NULL)
    {
        char *pwd = getenv("HOME");
        if (pwd != NULL)
        {
            int i = chdir(pwd);
            setenv("OLDPWD", getenv("PWD"), 1);
            setenv("PWD", pwd, 1);
            return i;
        }
    }
    else if (strcmp(path, "..") == 0)
    {
        char *new_path = getenv("PWD");
        int i = strlen(new_path);
        setenv("OLDPWD", new_path, 1);
        while (new_path[i] != '/')
            i--;
        new_path[i] = 0;
        int a = chdir("..");
        setenv("PWD", new_path, 1);
        return a;
    }
    else if (strcmp(path, "-") == 0)
    {
        char *pwd = getenv("PWD");
        char *oldpwd = getenv("OLDPWD");
        chdir(oldpwd);
        int a = setenv("PWD", oldpwd, 1);
        a = (a == 0) && (setenv("OLDPWD", pwd, 1) == 0);
        return 0;
    }
    else if (strcmp(path, ".") != 0)
    {
        char *pwd = getenv("PWD");
        if (path[0] == '/')
        {
            int a = chdir(path);
            if (a == -1)
                errx(1, "cd: %s: No such file or directory", path);
            setenv("OLDPWD", pwd, 1);
            setenv("PWD", path, 1);
            return a;
        }
        else
        {
            return cd_relative_paths(path, pwd);
        }
    }
    return 0;
}

int cd_relative_paths(char *path, char *pwd)
{
    char *dirname = strtok(path, "/");
    while (dirname != NULL)
    {
        DIR *dir = opendir(pwd);
        struct dirent *entry = readdir(dir);
        int a = 1;
        for (; entry; entry = readdir(dir))
        {
            char buf[256] = { 0 };
            sprintf(buf, "%s", entry->d_name);
            if (strcmp(buf, dirname) == 0)
            {
                setenv("OLDPWD", pwd, 1);
                char new_path[1024] = { 0 };
                a = chdir(dirname);
                getcwd(new_path, sizeof(new_path));
                setenv("PWD", new_path, 1);
                pwd = new_path;
            }
            if (a == 0)
                break;
        }
        if (a == 1)
            errx(1, "cd: %s: No such file or directory", path);
        dirname = strtok(NULL, "/");
    }
    return 0;
}

int subshell_builtin(struct ast *node)
{
    pid_t pid = fork();
    if (pid == 0) // Child
    {
        int return_value = 0;
        for (size_t i = 0; i < node->nb_children; i++)
            return_value = evaluate_node(node->children[i]);
        exit(return_value);
    }
    else // Parent
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        errx(2, "Subshell did not finished properly.");
    }
}

int unset_var(struct Command *com)
{
    com = com->next;
    while (com)
    {
        unsetenv(com->name);
        com = com->next;
    }
    return 0;
}

int unset_func(struct Command *com)
{
    com = com->next;
    while (com)
    {
        remove_fonction(com->name);
        com = com->next;
    }
    return 0;
}

int unset_builtin(struct Command *com)
{
    if (com->next != NULL)
    {
        char *param = com->next->name;
        if (param[0] == '-')
        {
            if (param[1] == 'v')
                return unset_var(com->next);
            else if (param[1] == 'f')
                return unset_func(com->next);
            else
                fprintf(stderr, "unset: invalid option");
        }
        else
            unset_var(com);
    }
    return 0;
}

int dot_builtin(struct Command *com)
{
    int argc = 0;
    char **argv = concat_com_in_list(com);
    while (com)
    {
        argc += 1;
        com = com->next;
    }
    int res = exec_main(argc, argv);
    free_com_in_list(argv);
    return res;
}
