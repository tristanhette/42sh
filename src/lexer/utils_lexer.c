#include "utils_lexer.h"

#include "lexer.h"

size_t my_strnlen(const char *s, size_t n)
{
    size_t len = 0;
    while (len < n && s[len] != '\0')
    {
        len++;
    }
    return len;
}

char *my_strndup(const char *s, size_t n)
{
    size_t len = my_strnlen(s, n); // Find the length of s, but not exceeding n
    char *new_str = malloc(len + 1); // Allocate memory for the new string

    if (new_str == NULL)
    {
        return NULL; // Return NULL if memory allocation fails
    }

    memcpy(new_str, s, len); // Copy the content from s to new_str
    new_str[len] = '\0'; // Null-terminate the new string

    return new_str;
}

char *my_strdup(const char *s)
{
    char *dup = malloc(strlen(s) + 1);
    if (dup == NULL)
    {
        return NULL; // Memory allocation failed
    }

    strcpy(dup, s);
    return dup;
}

bool is_delimiter(char c) // delimiter for now
{
    return c == ' ' || c == ';' || c == '\n';
}

bool is_end_of_statement(char c) // end of command or comment
{
    return c == ';' || c == '\n';
}

bool is_group_delimiter(char c)
{
    return c == '{' || c == '}' || c == '(' || c == ')';
}

bool is_containing_redirection(const char *input)
{
    // Define the redirection operators
    const char *redirections[] = { ">>", ">&", "<&", ">|", "<>", ">", "<" };
    size_t numRedirections = sizeof(redirections) / sizeof(redirections[0]);

    // Check if the input string contains any of the redirection operators
    for (size_t i = 0; i < numRedirections; i++)
    {
        if (strstr(input, redirections[i]) != NULL)
        {
            return true; // Redirection operator found
        }
    }

    return false; // No redirection operator found
}

bool is_assignment_word(char *input)
{
    if (strstr(input, "=") != NULL)
        return true;
    return false;
}

bool is_end_in_dquote(const char *str, size_t len)
{
    bool inComment = false;
    bool in_squote = false;
    bool in_dquote = false;
    bool in_backslash = false;

    for (size_t i = 0; i != len; i++)
    {
        char p = str[i];
        if (in_squote)
        {
            if (p == '\'')
            {
                in_squote = false; // End of single quote
            }
        }
        if (in_dquote)
        {
            if (p == '\"')
            {
                in_dquote = false; // End of single quote
            }
        }
        else if (inComment)
        {
            if (p == '\n')
            {
                inComment = false; // End of comment
            }
        }
        else if (in_backslash)
            in_backslash = false;
        else
        {
            if (p == '#')
                inComment = true; // Start of a comment
            else if (p == '\'')
                in_squote = true; // Start of single quote
            else if (p == '\"')
                in_dquote = true;
            else if (p == '\\')
                in_backslash = true;
        }
    }

    // Return true if the end position is within a comment or a single quote
    return in_dquote;
}

bool is_end_in_comment_or_quote(const char *str, size_t len)
{
    bool inComment = false;
    bool in_squote = false;
    bool in_dquote = false;
    bool in_backslash = false;

    for (size_t i = 0; i != len; i++)
    {
        char p = str[i];
        if (in_squote)
        {
            if (p == '\'')
            {
                in_squote = false; // End of single quote
            }
        }
        if (in_dquote)
        {
            if (p == '\"')
            {
                in_dquote = false; // End of single quote
            }
        }
        else if (inComment)
        {
            if (p == '\n')
            {
                inComment = false; // End of comment
            }
        }
        else if (in_backslash)
            in_backslash = false;
        else
        {
            if (p == '#')
                inComment = true; // Start of a comment
            else if (p == '\'')
                in_squote = true; // Start of single quote
            else if (p == '\"')
                in_dquote = true;
            else if (p == '\\')
                in_backslash = true;
        }
    }

    // Return true if the end position is within a comment or a single quote
    return inComment || in_squote;
}
