#ifndef IO_BACKEND_H
#define IO_BACKEND_H

char *string_script(char **value);
char *file_script(char *file);
char *stdin_script(void);
char *io_backend(int argc, char **argv);
#endif /* ! IO_BACKEND_H */
