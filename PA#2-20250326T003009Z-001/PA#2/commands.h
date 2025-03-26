#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef struct {
    char type[10];
    char name[50];
    uint32_t salary;
} Command;

typedef struct {
    Command* list;
    int count;
} CommandList;

CommandList parse_commands(const char* filename);
void free_commands(CommandList cmds);

#endif
