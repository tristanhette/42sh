#define _POSIX_C_SOURCE 1

#include "io_backend.h"

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *string_script(char **value)
{
    char *buf = calloc(sizeof(char), 256);
    int size = 256;
    int i = 0;
    int i_buf = 0;
    while (value[2][i] != 0)
    {
        if (i == size)
        {
            size += 256;
            buf = realloc(buf, sizeof(char) * size);
        }
        buf[i_buf] = value[2][i];
        i_buf++;
        i++;
    }
    buf[i] = 0;
    return buf;
}

char *file_script(char *file)
{
    int f = open(file, O_RDONLY);
    if (f == -1)
        errx(2, "Unknown script %s\n", file);

    size_t length = 1;
    char *buf = calloc(1, length);
    char cursor[256] = { 0 };
    int bytes = 0;
    while ((bytes = read(f, cursor, 256)) > 0)
    {
        length += bytes;
        buf = realloc(buf, sizeof(char) * (length + 1));
        strncat(buf, cursor, bytes);
    }

    buf[length - 1] = 0;
    return buf;
}

char *stdin_script(void)
{
    FILE *f = fdopen(0, "r");
    char *buf = calloc(sizeof(char), 1024);
    fread(buf, sizeof(char), 1024, f);
    return buf;
}

char *io_backend(int argc, char **argv)
{
    if (argc == 3)
    {
        if (strcmp(argv[1], "-c") == 0)
            return string_script(argv);
        else if (strcmp(argv[1], "<") == 0)
        {
            return file_script(argv[2]);
        }
        else
            goto error;
    }
    else if (argc == 2)
    {
        return file_script(argv[1]);
    }
    else if (argc == 1)
    {
        return stdin_script();
    }
    else
    {
        goto error;
    }
error:
    errx(2, "Usage: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]");
}
