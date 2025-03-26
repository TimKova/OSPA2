#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

CommandList parse_commands(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open commands.txt");
        exit(1);
    }

    CommandList cmdlist;
    cmdlist.count = 0;
    cmdlist.list = NULL;

    char line[256];
    int commandIndex = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "threads", 7) == 0) {
            sscanf(line, "threads,%d", &cmdlist.count);
            cmdlist.list = calloc(cmdlist.count, sizeof(Command));
        } else {
            sscanf(line, "%[^,],%[^,],%u", cmdlist.list[commandIndex].type,
                   cmdlist.list[commandIndex].name, &cmdlist.list[commandIndex].salary);
            commandIndex++;
        }
    }

    fclose(fp);
    return cmdlist;
}

void free_commands(CommandList cmds) {
    free(cmds.list);
}
