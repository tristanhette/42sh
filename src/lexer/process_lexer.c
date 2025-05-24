#include "process_lexer.h"

void process_backslash(struct lexing_param *lexing, char input, char next)
{
    if (input == '\n')
    {
        lexing->in_backslash = false;
        return;
    }

    if (lexing->size_word != 0)
    {
        finalize_word(lexing);
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }

    char temp[2] = { input, 0 };
    struct token *tok = create_token(temp);
    tok->type = TOKEN_WORD;
    append_token(lexing->tokens, tok);

    if ((lexing->in_dquote || !is_delimiter(next)) && next != 0)
    {
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }

    lexing->in_backslash = false;
}

void process_squote(struct lexing_param *lexing, char next)
{
    lexing->word[lexing->size_word] = 0;
    if (is_assignment_word(lexing->word) && !lexing->in_squote)
        lexing->is_assignment_word = true;

    if (lexing->size_word != 0 && !lexing->in_squote)
    {
        finalize_word(lexing);
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }

    lexing->in_squote = !lexing->in_squote;
    lexing->was_squoted = true;
    if (!lexing->in_squote)
        finalize_word(lexing);
    if (!is_delimiter(next) && next != 0 && !lexing->in_squote)
    {
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }
}

void process_dquote(struct lexing_param *lexing, char next)
{
    lexing->word[lexing->size_word] = 0;
    if (is_assignment_word(lexing->word) && !lexing->in_dquote)
        lexing->is_assignment_word = true;

    if (lexing->size_word != 0 && !lexing->in_dquote)
    {
        finalize_word(lexing);
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }

    lexing->in_dquote = !lexing->in_dquote;
    lexing->was_dquoted = true;
    if (!lexing->in_dquote)
        finalize_word(lexing);
    if (!is_delimiter(next) && next != 0 && !lexing->in_dquote)
    {
        struct token *tok = create_token("CONCAT");
        tok->type = TOKEN_CONCAT;
        append_token(lexing->tokens, tok);
    }
}

void process_add_char(struct lexing_param *lexing, char input)
{
    lexing->word[lexing->size_word] = input;
    lexing->size_word++;
    if (lexing->size_word == lexing->word_capacity - 1)
    {
        lexing->word_capacity *= 2;
        lexing->word =
            realloc(lexing->word, sizeof(char) * lexing->word_capacity);
    }
}

void process_remove_braced_var(struct tokenVect *tokens)
{
    for (size_t i = 0; i < tokens->len - 1; i++)
    {
        if (tokens->data[i]->type == TOKEN_VAR
            && tokens->data[i + 1]->type == TOKEN_OPEN_BRACKETS)
        {
            free_token(tokens->data[i + 1]);
            for (size_t j = i + 1; j < tokens->len - 1; j++)
            {
                tokens->data[j] = tokens->data[j + 1];
            }
            tokens->len--;

            free_token(tokens->data[i + 2]);
            for (size_t j = i + 2; j < tokens->len - 1; j++)
            {
                tokens->data[j] = tokens->data[j + 1];
            }
            tokens->len--;

            size_t len = strlen(tokens->data[i + 1]->value);
            tokens->data[i]->value = realloc(tokens->data[i]->value, len + 2);
            memmove(tokens->data[i]->value + 1, tokens->data[i + 1]->value,
                    len);
            tokens->data[i]->value[len + 1] = 0;

            free_token(tokens->data[i + 1]);
            for (size_t j = i + 1; j < tokens->len - 1; j++)
            {
                tokens->data[j] = tokens->data[j + 1];
            }
            tokens->len--;
        }
    }
}

void process_concat_double_semi_col(struct tokenVect *tokens)
{
    for (size_t i = 0; i < tokens->len - 1; i++)
    {
        if (tokens->data[i]->type == TOKEN_SEMICOLON
            && tokens->data[i + 1]->type == TOKEN_SEMICOLON)
        {
            free_token(tokens->data[i + 1]);
            for (size_t j = i + 1; j < tokens->len - 1; j++)
            {
                tokens->data[j] = tokens->data[j + 1];
            }
            tokens->data[i]->type = TOKEN_D_SEMICOLON;
            tokens->data[i]->value = realloc(tokens->data[i]->value, 3);
            tokens->data[i]->value[1] = ';';
            tokens->data[i]->value[2] = 0;
            tokens->len--;
        }
    }
}

// Dynamically replaces `$(cmd)` and ``cmd`` with the command's output
void replace_with_cmd_output(char **inputPtr)
{
    char *input = *inputPtr;
    size_t inputSize = strlen(input) + 1; // +1 for null terminator

    char *pos;
    while ((pos = strstr(input, "$(")))
    {
        input = *inputPtr;
        inputSize = strlen(input) + 1; // +1 for null terminator
        bool res = replace(inputPtr, &input, inputSize, pos);
        if (!res)
            break;
    }
    input = *inputPtr;
    while ((pos = strstr(input, "`")))
    {
        input = *inputPtr;
        inputSize = strlen(input) + 1; // +1 for null terminator
        bool res = replace(inputPtr, &input, inputSize, pos);
        if (!res)
            break;
    }
}
