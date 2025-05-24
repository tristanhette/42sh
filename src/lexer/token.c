#include "token.h"

#include <string.h>

enum TokenType determine_token_type(const char *value)
{
    if (strcmp(value, "if") == 0)
        return TOKEN_IF;
    if (strcmp(value, "then") == 0)
        return TOKEN_THEN;
    if (strcmp(value, "elif") == 0)
        return TOKEN_ELIF;
    if (strcmp(value, "else") == 0)
        return TOKEN_ELSE;
    if (strcmp(value, "fi") == 0)
        return TOKEN_FI;
    if (strcmp(value, ";") == 0)
        return TOKEN_SEMICOLON;
    if (strcmp(value, "\n") == 0)
        return TOKEN_NEWLINE;
    if (strcmp(value, "\'") == 0)
        return TOKEN_QUOTE;
    if (strcmp(value, "EOF") == 0)
        return TOKEN_EOF;
    if (strcmp(value, "while") == 0)
        return TOKEN_WHILE;
    if (strcmp(value, "until") == 0)
        return TOKEN_UNTIL;
    if (strcmp(value, "do") == 0)
        return TOKEN_DO;
    if (strcmp(value, "done") == 0)
        return TOKEN_DONE;
    if (strcmp(value, "for") == 0)
        return TOKEN_FOR;
    if (strcmp(value, "in") == 0)
        return TOKEN_IN;
    if (strcmp(value, "!") == 0)
        return TOKEN_NEG;
    if (strcmp(value, "|") == 0)
        return TOKEN_PIPE;

    // Assuming any other string is a word
    return determine_token_type2(value);
}

enum TokenType determine_token_type2(const char *value)
{
    if (strcmp(value, "&&") == 0)
        return TOKEN_AND;
    if (strcmp(value, "||") == 0)
        return TOKEN_OR;
    if (strcmp(value, ">") == 0)
        return TOKEN_GREATER;
    if (strcmp(value, "<") == 0)
        return TOKEN_LESS;
    if (strcmp(value, ">>") == 0)
        return TOKEN_DOUBLE_GREATER;
    if (strcmp(value, ">&") == 0)
        return TOKEN_GREATER_AMPERSAND;
    if (strcmp(value, "<&") == 0)
        return TOKEN_LESS_AMPERSAND;
    if (strcmp(value, ">|") == 0)
        return TOKEN_GREATER_PIPE;
    if (strcmp(value, "<>") == 0)
        return TOKEN_LESS_GREATER;
    if (strcmp(value, "EOF") == 0)
        return TOKEN_EOF;
    if (strcmp(value, "{") == 0)
        return TOKEN_OPEN_BRACKETS;
    if (strcmp(value, "}") == 0)
        return TOKEN_CLOSE_BRACKETS;
    if (strcmp(value, "(") == 0)
        return TOKEN_OPEN_PARENTHESIS;
    if (strcmp(value, ")") == 0)
        return TOKEN_CLOSE_PARENTHESIS;
    if (strcmp(value, "case") == 0)
        return TOKEN_CASE;
    if (strcmp(value, "esac") == 0)
        return TOKEN_ESAC;
    if (strcmp(value, ";;") == 0)
        return TOKEN_D_SEMICOLON;

    // Assuming any other string is a word
    return TOKEN_WORD;
}

struct token *create_token(char *word)
{
    struct token *token = malloc(sizeof(struct token));
    token->type = determine_token_type(word);
    token->value = my_strdup(word);
    return token;
}

struct tokenVect *create_list_token(void)
{
    struct tokenVect *tokens = calloc(1, sizeof(struct tokenVect));
    tokens->data = calloc(16, sizeof(struct token *));
    tokens->len = 0;
    tokens->pos = 0;
    tokens->capacity = 16;
    return tokens;
}

void append_token(struct tokenVect *tokens, struct token *token)
{
    if (tokens->len >= tokens->capacity)
    {
        tokens->capacity *= 2;
        tokens->data =
            realloc(tokens->data, tokens->capacity * sizeof(struct token *));
    }
    tokens->data[tokens->len] = token;
    tokens->len++;
}

struct token *token_peek(struct tokenVect *tokens)
{
    if (tokens->pos > tokens->len - 1) // case out of bound
        return NULL;
    return tokens->data[tokens->pos];
}

struct token *token_pop(struct tokenVect *tokens)
{
    if (tokens->pos > tokens->len - 1) // case out of bound
        return NULL;
    tokens->pos++;
    return tokens->data[tokens->pos - 1];
}

void free_token(struct token *token)
{
    if (token != NULL)
    {
        free(token->value); // Free the dynamically allocated string
        free(token); // Then free the struct itself
    }
}

void free_tokenVect(struct tokenVect *tokens)
{
    if (tokens != NULL)
    {
        for (size_t i = 0; i < tokens->len; i++)
        {
            free_token(tokens->data[i]); // Free each struct token
        }
        free(tokens->data); // Free the array of struct token pointers
        free(tokens); // Finally, free the struct tokenVect itself
    }
}

void print_tokenVect(struct tokenVect *tokens)
{
    printf("Tokens:\n");
    for (size_t i = 0; i < tokens->len; i++)
    {
        printf("Type: %d, Value: %s\n", tokens->data[i]->type,
               tokens->data[i]->value);
    }
}
