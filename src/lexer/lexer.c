#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../parser/parser.h"
#include "process_lexer.h"

#define INITIAL_CMD_LEN 256
#define INITIAL_OUTPUT_LEN 1024
#define BUFFER_SIZE 128

void split_redirection(const char *input, struct tokenVect *tokens)
{
    char *number = NULL;
    char *redirection = NULL;
    char *word = NULL;

    const char *ptr = input;

    // Extract number if it exists
    while (isdigit(*ptr))
    {
        ptr++;
    }
    if (ptr != input)
    {
        number = my_strndup(input, ptr - input);
        input = ptr;
    }

    // Extract redirection operator
    const char *redirections[] = { ">>", ">&", "<&", ">|", "<>", ">", "<" };
    for (size_t i = 0; i < sizeof(redirections) / sizeof(redirections[0]); ++i)
    {
        size_t len = strlen(redirections[i]);
        if (strncmp(input, redirections[i], len) == 0)
        {
            redirection = my_strndup(input, len);
            input += len;
            break;
        }
    }

    // Extract word
    if (*input != '\0')
    {
        word = my_strdup(input);
    }

    struct token *tok;
    if (number != NULL)
    {
        tok = create_token(number);
        tok->type = TOKEN_IONUMBER;
        append_token(tokens, tok);
    }
    tok = create_token(redirection);
    append_token(tokens, tok);
    if (word != NULL)
    {
        tok = create_token(word);
        append_token(tokens, tok);
    }
    free(number);
    free(redirection);
    free(word);
}

// create a token and reset a word
void finalize_word(struct lexing_param *lexing)
{
    struct tokenVect *tokens = lexing->tokens;
    char *word = lexing->word;
    bool was_squoted = lexing->was_squoted;
    bool was_dquoted = lexing->was_dquoted;
    bool was_backslash = lexing->was_backslash;

    if (was_squoted || (was_backslash && lexing->size_word == 1) || was_dquoted)
    {
        struct token *tok = create_token(word);
        tok->type = TOKEN_WORD;
        if (was_dquoted && strstr(word, "$") != NULL)
            tok->type = TOKEN_VAR;
        else if (lexing->is_assignment_word)
            tok->type = TOKEN_ASSIGNMENT_WORD;
        append_token(tokens, tok);
        memset(word, 0, BUFFER_SIZE * sizeof(char));
        lexing->size_word = 0;
    }
    else
    {
        if (is_containing_redirection(word))
            split_redirection(word, tokens);
        else
        {
            struct token *tok = create_token(word);
            if (is_assignment_word(word))
                tok->type = TOKEN_ASSIGNMENT_WORD;
            else if (strstr(word, "$") != NULL)
                tok->type = TOKEN_VAR;
            append_token(tokens, tok);
        }
        memset(word, 0, BUFFER_SIZE * sizeof(char));
        lexing->size_word = 0;
    }
    lexing->was_squoted = false;
    lexing->was_dquoted = false;
    lexing->was_backslash = false;
    lexing->is_assignment_word = false;
}

// add ; or \n  token
void process_end_of_statement(struct tokenVect *tokens, char c)
{
    if (c == '\n' && tokens->len == 0)
        return;

    char temp[2] = { c, 0 };
    struct token *tok = create_token(temp);
    append_token(tokens, tok);
}

void init_lexing(struct lexing_param *lexing)
{
    lexing->tokens = create_list_token();
    lexing->word = calloc(BUFFER_SIZE, sizeof(char));
    lexing->size_word = 0;
    lexing->word_capacity = BUFFER_SIZE;
    lexing->in_squote = false;
    lexing->in_dquote = false;
    lexing->was_squoted = false;
    lexing->was_dquoted = false;
    lexing->in_comment = false;
    lexing->in_backslash = false;
    lexing->was_backslash = false;
    lexing->is_assignment_word = false;
}

void process_input(struct lexing_param *lexing, char previous, char input,
                   char next)
{
    if (input == '\n' && lexing->in_comment)
    {
        lexing->in_comment = false;
        process_end_of_statement(lexing->tokens, '\n');
        return;
    } // else
    else if (lexing->in_comment)
        return;

    if (input == '\\' && !lexing->in_squote && !lexing->in_backslash)
    {
        lexing->in_backslash = true;
        lexing->was_backslash = true;
    }
    else if (lexing->in_backslash)
    {
        process_backslash(lexing, input, next);
    }
    else if (is_group_delimiter(input) && !lexing->in_squote
             && !lexing->in_dquote)
    {
        if (lexing->size_word != 0)
            finalize_word(lexing);
        process_add_char(lexing, input);
        finalize_word(lexing);
    }
    else if (previous != '|' && previous != '>' && input == '|' && next != '|'
             && !lexing->in_squote && !lexing->in_dquote)
    {
        if (lexing->size_word != 0) // if previous token wasnt null
            finalize_word(lexing);
        process_add_char(lexing, input);
        finalize_word(lexing);
    }
    else
        continue_process_input(lexing, input, next);
}

void continue_process_input(struct lexing_param *lexing, char input, char next)
{
    if (input == '|' && lexing->size_word != 0
        && lexing->word[lexing->size_word - 1] == '|')
    {
        if (lexing->size_word != 1)
        {
            lexing->word[lexing->size_word - 1] = 0;
            finalize_word(lexing);
            process_add_char(lexing, input);
        }
        process_add_char(lexing, input);
        finalize_word(lexing);
    }
    else if (input == '#' && !lexing->in_squote && !lexing->in_dquote
             && lexing->size_word == 0) // start comment
    {
        lexing->in_comment = true;
        if (lexing->size_word != 0)
            finalize_word(lexing);
    }
    else if (input == '$' && !lexing->in_squote && lexing->size_word != 0)
    {
        finalize_word(lexing);
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
        process_add_char(lexing, input);
    }
    else if (input == ' ' && lexing->in_dquote && lexing->size_word != 0
             && lexing->word[0] == '$')
    {
        finalize_word(lexing);
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
        process_add_char(lexing, input);
    }
    else if (is_delimiter(input) && !lexing->in_squote && !lexing->in_dquote)
    {
        if (lexing->size_word != 0) // if previous token wasnt null
            finalize_word(lexing);
        if (is_end_of_statement(input))
            process_end_of_statement(lexing->tokens, input);
    }
    else if (input == '\'' && !lexing->in_dquote)
    {
        process_squote(lexing, next);
    }
    else if (input == '\"' && !lexing->in_squote)
    {
        process_dquote(lexing, next);
    }
    else // add char to current string
    {
        process_add_char(lexing, input);
    }
}

struct tokenVect *lexer(char *input)
{
    struct lexing_param *lexing = calloc(1, sizeof(struct lexing_param));
    init_lexing(lexing);

    size_t length = strlen(input);
    char *duplicate = (char *)malloc(length + 1);
    if (duplicate == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL; // Return NULL to indicate failure
    }
    strcpy(duplicate, input);
    replace_with_cmd_output(&duplicate);
    input = duplicate;

    if (input[0] != 0)
    {
        process_input(lexing, '\0', input[0], input[1]);

        int i = 1;
        while (input[i] != 0)
        {
            process_input(lexing, input[i - 1], input[i], input[i + 1]);
            i++;
        }
    }
    if (lexing->in_squote || lexing->in_dquote
        || lexing->in_backslash) // if quote not terminated
    {
        free(duplicate);
        free_tokenVect(lexing->tokens);
        free(lexing->word);
        free(lexing);
        return NULL;
    }
    if (lexing->word[0] != 0) // if current string wasnt finished
    {
        finalize_word(lexing);
    }
    struct token *tok = create_token("EOF"); // add last token
    append_token(lexing->tokens, tok);

    free(lexing->word);
    struct tokenVect *tokens = lexing->tokens;
    process_remove_braced_var(tokens);
    process_concat_double_semi_col(tokens);
    free(lexing);
    free(duplicate);
    return tokens;
}

// Dynamically executes the command and returns the output
char *execute_command(char *cmd)
{
    int pipefd[2];
    pid_t pid;
    char *output = malloc(4096); // Allocate memory for the output

    if (pipe(pipefd) == -1)
    {
        perror("pipe failed");
        free(output);
        return NULL;
    }

    pid = fork();

    if (pid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        free(output);
        errx(2, "fork failed");
    }
    else if (pid == 0)
    {
        // Child process
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]); // Close write end after dup2

        struct tokenVect *tokens = lexer(cmd);
        struct ast *ast = parse(tokens);
        int return_code = evaluate_ast(ast);
        if (return_code > 1)
            errx(return_code, "substitute fail");
        free_tokenVect(tokens);
        free_ast(ast);
        // Call the function that prints to stdout

        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        close(pipefd[1]); // Close unused write end
        ssize_t len =
            read(pipefd[0], output, 4096); // Read the output from the pipe
        output[len] = 0;
        close(pipefd[0]); // Close read end

        int status;
        wait(&status); // Wait for child to terminate

        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            if (exit_status > 1)
                errx(exit_status, "substitute fail in child");
        }
    }

    return output;
}

void newline_to_space(char *str)
{
    if (str == NULL)
        return; // Check for NULL pointer

    // Iterate through each character in the string
    while (*str != '\0')
    { // Continue until we hit the end of the string
        if (*str == '\n')
        { // Check if the current character is a newline
            *str = ' '; // Replace newline with space
        }
        str++; // Move to the next character
    }
}

bool replace(char **inputPtr, char **input, size_t inputSize, char *pos)
{
    char *end = (pos[0] == '$') ? strchr(pos + 2, ')') : strchr(pos + 1, '`');
    if (!end)
        return false; // No closing bracket or backtick found

    size_t posOffset = pos - *input;
    size_t endOffset = end - *input;

    // Calculate command size and allocate memory
    size_t cmdSize = endOffset - posOffset - (pos[0] == '$' ? 2 : 1);
    char *cmd = malloc(cmdSize + 1); // +1 for null terminator
    strncpy(cmd, *input + posOffset + (pos[0] == '$' ? 2 : 1), cmdSize);
    cmd[cmdSize] = '\0'; // Null-terminate the command string

    char *output;
    // Execute the command and get the output
    if (is_end_in_comment_or_quote(*input, posOffset))
    {
        *input += endOffset;
        free(cmd);
        return true;
    }
    else
    {
        output = execute_command(cmd);
        size_t outputSize = strlen(output);
        if (outputSize > 1 && output[outputSize - 1] == '\n')
        {
            output[outputSize - 1] = 0;
        }
        if (!is_end_in_dquote(*input, posOffset))
            newline_to_space(output);
    }

    // Calculate new *input size
    size_t outputSize = strlen(output);
    // Need magic in this world
    size_t newSize = inputSize - (endOffset - posOffset + 1) + outputSize + 100;
    if (outputSize > inputSize)
        newSize++;

    // Reallocate *input to fit the new size
    char *newinput = calloc(newSize, sizeof(char));
    if (newinput == NULL)
        errx(2, "calloc fail");
    memmove(newinput, *input, posOffset);
    memmove(newinput + posOffset, output, outputSize);
    memmove(newinput + posOffset + outputSize, *input + endOffset + 1,
            inputSize - endOffset - 1);

    free(*input);
    *input = newinput;
    *inputPtr = *input; // Update the caller's pointer

    // Clean up
    free(cmd);
    free(output);

    *input += endOffset;
    return true;
}
