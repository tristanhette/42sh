#define _DEFAULT_SOURCE
#include "variables.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../fonctions/fonction.h"

void initialize_globals(int argc, char *argv[])
{
    commandLineInfo.argc = argc;
    commandLineInfo.argv = argv;
    commandLineInfo.n_break = 0;
    commandLineInfo.n_continue = 0;
    commandLineInfo.fonction_table = init_fonction_table();
}

static int generate_random_number(int min, int max)
{
    srand(time(NULL));
    return min + rand() % (max - min + 1);
}

static char *uid_to_string(uid_t uid)
{
    char *uid_str;
    int uid_str_size =
        snprintf(NULL, 0, "%d", uid) + 1; // +1 for the null terminator
    uid_str = (char *)malloc(uid_str_size);
    if (uid_str == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    snprintf(uid_str, uid_str_size, "%d", uid);
    return uid_str;
}

static char *pid_to_string(void)
{
    pid_t pid = getpid();
    char *pid_str;
    int pid_str_size =
        snprintf(NULL, 0, "%d", pid) + 1; // +1 for the null terminator
    pid_str = (char *)malloc(pid_str_size);
    if (pid_str == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    snprintf(pid_str, pid_str_size, "%d", pid);
    return pid_str;
}

static char *int_to_string(int num)
{
    char buffer[20]; // Ajustez la taille selon vos besoins.
    snprintf(buffer, sizeof(buffer), "%d", num);
    char *result = malloc(strlen(buffer) + 1);
    if (result != NULL)
    {
        strcpy(result, buffer);
    }
    return result;
}

char *copy_string(const char *source)
{
    if (source == NULL)
        return NULL;

    char *destination = calloc(strlen(source) + 1, sizeof(char));

    if (destination != NULL)
        strcpy(destination, source);

    return destination;
}

char *concat_argv(int argc, char *argv[])
{
    if (argc <= 1)
        return calloc(1, sizeof(char)); // Returns Emptry allocated string

    size_t length = strlen(argv[1]);
    char *result = calloc(length + 1, sizeof(char));
    strcpy(result, argv[1]);

    for (int i = 2; i < argc; i++)
    {
        size_t new_size = 1 + strlen(argv[i]); // Add 1 for the space
        result = realloc(result, (length + new_size + 1) * sizeof(char));
        strcpy(result + length, " ");
        strcpy(result + length + 1, argv[i]);
        length += new_size;
    }

    return result;
}

char *handle_star(int argc, char *argv[])
{
    if (argc > 1)
    {
        size_t total_length = 0;
        for (int i = 1; i < argc; i++)
        {
            total_length +=
                strlen(argv[i]) + 1; // +1 pour l'espace entre les arguments
        }
        char *all_arguments = (char *)malloc(total_length);
        if (all_arguments == NULL)
        {
            perror("Erreur d'allocation mémoire");
            exit(EXIT_FAILURE);
        }
        all_arguments[0] =
            '\0'; // Assurez-vous que la chaîne est initialement vide
        for (int i = 1; i < argc; i++)
        {
            strcat(all_arguments, argv[i]);
            if (i < argc - 1)
            {
                strcat(all_arguments, " ");
            }
        }

        return all_arguments;
    }
    else
    {
        char *empty_string = (char *)malloc(1);
        empty_string[0] = '\0';
        return empty_string;
    }
}

// Expand the command and returns a new command with variable expanded
struct Command *expand_command(struct Command *command)
{
    struct Command *expanded_com = get_command_copy(command);

    struct Command *com = expanded_com;
    struct Command *prev = NULL;
    int to_concat = 0;
    while (com != NULL)
    {
        if (com->type == CMD_CONCAT)
            to_concat = 1;
        else
        {
            if (com->type == CMD_VAR)
            {
                char *temp = com->name;
                com->name = switch_var(temp, 0);
                extend_command(com);
                free(temp);
            }
            if (to_concat)
            {
                concat_command(prev, com);
                to_concat = 0;
                com = prev->next;
                continue;
            }
            prev = com;
        }

        com = com->next;
    }

    return expanded_com;
}

char *switch_var(char *input, int last_exit_code)
{
    if (strcmp(input, "$@") == 0)
        return concat_argv(commandLineInfo.argc, commandLineInfo.argv);
    else if (strcmp(input, "$*") == 0)
        return handle_star(commandLineInfo.argc, commandLineInfo.argv);
    else if (strcmp(input, "$?") == 0)
        return int_to_string(last_exit_code);
    else if (strcmp(input, "$$") == 0)
        return pid_to_string();
    else if (strcmp(input, "$#") == 0)
        return int_to_string(commandLineInfo.argc - 1);
    else if (strcmp(input, "$RANDOM") == 0)
    {
        int random_number = generate_random_number(1, 100000);
        char *str = int_to_string(random_number);
        return str;
    }
    else if (strcmp(input, "$UID") == 0)
    {
        uid_t uid = getuid();
        return uid_to_string(uid);
    }
    else
    {
        char *endptr;
        long number = strtol(input + 1, &endptr, 10);
        if ((*endptr != '\0' && *endptr != '\n') || *(input + 1) == 0)
            return expand_parameters(input); // getenv
        if (number + 1 > commandLineInfo.argc || number < 0)
            return calloc(1, 1);
        return copy_string(commandLineInfo.argv[number]);
    }
}

char *str_concat(char *str1, const char *str2)
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    str1 = realloc(str1, len1 + len2 + 1);

    strcpy(str1 + len1, str2);
    str1[len1 + len2] = 0;
    return str1; // Return bcs easier but not needed bcs pointer
}

char *expand_parameters2(const char *input, size_t length, char *result,
                         size_t i)
{
    size_t start;
    size_t end;
    int hasBraces = input[i + 1] == '{';
    if (hasBraces)
    {
        start = i + 2; // Start of parameter name after ${
        i = start;
        while (input[i] != '}' && i < length)
            i++; // Find the closing brace
        end = i;
        if (i >= length)
        {
            // Handle error: unmatched '{'
            free(result);
            return NULL;
        }
    }
    else
    {
        start = i + 1; // Start of parameter name after $
        i = start;
        while (isalnum(input[i]) || input[i] == '_')
            i++; // Find the end of the parameter name
        end = i;
        i--; // Adjust to point to the last character of parameter name
    }
    char *param = strndup(input + start, end - start);
    if (!param)
    {
        free(result);
        return NULL;
    }
    char *value = getenv(param);
    free(param);
    free(result);
    if (!value)
        return calloc(1, 1);
    return copy_string(value);
}

char *expand_parameters(const char *input)
{
    if (input == NULL)
        return NULL;
    size_t length = strlen(input);
    char *result = calloc(length + 1, sizeof(char));
    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i)
    {
        if (input[i] == '$' && (length - 1) - i > 0)
            return expand_parameters2(input, length, result, i);
        else
            result[result_index++] = input[i];
    }

    result[result_index] = '\0'; // Null-terminate the string
    char *to_return = copy_string(result);
    free(result);
    return to_return;
}

int var_builtin(struct Command *command)
{
    if (command)
    {
        char *param = concat_args(command);
        setenv(command->name, param, 1);
        // add_var(node->command->name, param);//care leaks
        free(param);
        return 0;
    }
    return 0;
}
