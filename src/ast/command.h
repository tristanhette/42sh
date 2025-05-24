#ifndef COMMAND_H
#define COMMAND_H

enum CommandType
{
    CMD_WORD,
    CMD_VAR,
    CMD_CONCAT,
    CMD_FREE
};

struct Command
{
    char *name;
    char *backup;
    struct Command *next;
    enum CommandType type;
};

void concat_command(struct Command *com1, struct Command *com2);
struct Command *createCommand(char *command);
struct Command *stack_push(struct Command *s, enum CommandType type, char *e);
char *concat_args(struct Command *stack);
struct Command *extend_command(struct Command *command);
char **concat_com_in_list(struct Command *command);
void free_com_in_list(char **com);
struct Command *get_command_copy(struct Command *command);
void destroy_command(struct Command *command);
void free_command(struct Command *command);

#endif /* !COMMAND_H */
