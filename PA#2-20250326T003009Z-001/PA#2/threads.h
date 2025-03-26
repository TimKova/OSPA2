#ifndef THREADS_H
#define THREADS_H

#include "commands.h"

void run_threads(CommandList cmdlist);
void* thread_worker(void* arg);

#endif
