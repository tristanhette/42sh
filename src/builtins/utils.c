#include "utils.h"

#include <stdlib.h>
#include <string.h>

char escape_char(int b, char *string)
{
    if (string[b] == '\\')
    {
        switch (string[++b])
        {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        default:
            return '\\';
        }
    }
    return '\\';
}

char *detect_escapes(char *string)
{
    int i = 0;
    int n = strlen(string);
    char *buf = calloc(sizeof(char), n + 1);
    int i_buf = 0;
    while (string[i] != 0)
    {
        if (string[i] == '\\')
        {
            buf[i_buf] = escape_char(i, string);
            if (buf[i_buf] == '\t' || buf[i_buf] == '\n')
                i += 1;
            else if (buf[i_buf] == '\\')
                i += 1;
        }
        else
        {
            buf[i_buf] = string[i];
        }
        i_buf++;

        i++;
    }
    // free(string);
    return buf;
}

char check_option(char *opt)
{
    int i = 1;
    if (opt[0] != '-')
        return 0;
    char l = 0;
    while (opt[i] != 0)
    {
        l = opt[i];
        if (l == 'e' || l == 'n' || l == 'E')
        {
            i++;
        }
        else
            return 0;
    }
    char buf = l;
    return buf;
}

int my_atoi(char *s)
{
    int len = strlen(s);
    int a = atoi(s);
    if (a != 0 || (len == 1 && s[0] == '0'))
        return a;
    return -1;
}
