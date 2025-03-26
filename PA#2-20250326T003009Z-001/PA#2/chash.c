#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commands.h"
#include "threads.h"

int main() {
    CommandList commands = parse_commands("commands.txt");

    run_threads(commands);

    free_commands(commands);
    return 0;
}
