#include "command.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../lexer/utils_lexer.h"

void concat_command(struct Command *com1, struct Command *com2)
{
    if (com1->type == CMD_CONCAT)
        return;
    free(com1->next->name); // Free concat
    free(com1->next);

    // Used to concat the both two last com
    size_t len1 = strlen(com1->name);
    size_t len2 = strlen(com2->name);

    char *new_value = calloc(len1 + len2 + 1, sizeof(char));
    strcpy(new_value, com1->name);
    strcpy(new_value + len1, com2->name);

    free(com1->name);
    free(com2->name);

    com1->name = new_value;
    com1->next = com2->next;

    free(com2);
}

struct Command *createCommand(char *command)
{
    struct Command *newCom = malloc(sizeof(struct Command));
    if (newCom == NULL)
    {
        perror("erreur malloc: createCom");
        exit(EXIT_FAILURE);
    }
    newCom->name = command;
    newCom->next = NULL;
    return newCom;
}

struct Command *stack_push(struct Command *s, enum CommandType type, char *e)
{
    struct Command *res = calloc(1, sizeof(struct Command));
    if (res == NULL)
        return s;
    res->name = e;
    res->next = NULL;
    res->type = type;
    if (s == NULL)
    {
        return res;
    }
    struct Command *current = s;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = res;
    return s;
}

char *concat_args(struct Command *stack)
{
    size_t total_length = 0;
    struct Command *current = stack;
    current = current->next;
    while (current != NULL)
    {
        total_length += strlen(current->name) + 1; // Add 1 for the space
        current = current->next;
    }
    if (total_length != 0)
    {
        char *result = malloc(total_length);
        if (result == NULL)
        {
            perror("erreur malloc: concatenate_stack_names_with_spaces");
            exit(EXIT_FAILURE);
        }
        current = stack;
        current = current->next;
        size_t index = 0;
        while (current != NULL)
        {
            size_t name_length = strlen(current->name);
            strcpy(result + index, current->name);
            index += name_length;
            result[index++] = ' '; // Add space between names
            current = current->next;
        }

        if (total_length > 0)
            result[total_length - 1] = '\0';

        return result;
    }

    return calloc(1, 1);
}

static char **split_name(char *input)
{
    int numSubstrings = 1;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == ' ')
        {
            numSubstrings++;
        }
    }

    char **substrings =
        calloc(numSubstrings + 1, sizeof(char *)); // +1 for the NULL element
    if (substrings == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    const char *delimiter = " ";
    char *token = strtok((char *)input, delimiter);
    int i = 0;
    while (token != NULL)
    {
        substrings[i] = my_strdup(token);
        if (substrings[i] == NULL)
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        i++;
        token = strtok(NULL, delimiter);
    }

    return substrings;
}

struct Command *extend_command(struct Command *command)
{
    if (!command)
        return NULL;

    char **new_str = split_name(command->name);
    if (!new_str[1])
    {
        free(new_str[0]);
        free(new_str);
        return extend_command(command->next);
    }

    size_t i = 0;
    free(command->name);
    command->name = new_str[i++];
    struct Command *prev = command;
    while (new_str[i] != NULL)
    {
        struct Command *new_command = malloc(sizeof(struct Command));
        new_command->name = new_str[i];
        new_command->type = CMD_WORD;
        new_command->next = prev->next;
        prev->next = new_command;
        prev = prev->next;

        i++;
    }

    free(new_str);
    prev->next = extend_command(prev->next);
    return command;
}

// fonction qui sert a concat les commandes et leur parametre dans un char **
char **concat_com_in_list(struct Command *command)
{
    if (command == NULL)
        return NULL;
    int commandCount = 0;
    struct Command *current = command;
    while (current != NULL)
    {
        commandCount++;
        current = current->next;
    }
    char **commandList = malloc((commandCount + 1) * sizeof(char *));
    if (commandList == NULL)
    {
        perror("erreur malloc: createCommandNameList");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    current = command;
    while (current != NULL)
    {
        char **names =
            split_name(current->name); // Split spaces for FOR builtins
        size_t index = 0;
        while (names[index])
        {
            commandList[i] = names[index];
            if (i + 1 == commandCount + 1)
                commandList =
                    realloc(commandList, (++commandCount + 1) * sizeof(char *));
            if (commandList[i] == NULL)
            {
                perror("erreur strdup: createCommandNameList");
                exit(EXIT_FAILURE);
            }
            index++;
            i++;
        }
        current = current->next;
        free(names);
    }
    commandList[i] = NULL;
    return commandList;
}

void free_com_in_list(char **com)
{
    int i = 0;
    while (com[i] != NULL)
    {
        free(com[i]);
        i++;
    }
    free(com);
}

// Get a deep copy of the given command
struct Command *get_command_copy(struct Command *command)
{
    if (!command)
        return NULL;

    struct Command *com_copy = malloc(sizeof(struct Command));
    com_copy->type = command->type;
    com_copy->name = calloc(strlen(command->name) + 1, sizeof(char));
    strcpy(com_copy->name, command->name);

    com_copy->next = get_command_copy(command->next); // Recursive Copy
    return com_copy;
}

// Free the given command block
void destroy_command(struct Command *command)
{
    if (!command)
        return;

    free(command->name);
    destroy_command(command->next); // Recursive free
    free(command);
}

void free_command(struct Command *cmd)
{
    while (cmd != NULL)
    {
        struct Command *temp = cmd;
        cmd = cmd->next;
        free(temp);
    }
}
