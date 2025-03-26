#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "commands.h"
#include "threads.h"
#include "hash_table.h"

int lock_acquire_count = 0;
int lock_release_count = 0;
int inserts_remaining = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t insert_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inserts_done = PTHREAD_COND_INITIALIZER;
FILE* out_fp;

void* thread_worker(void* arg) {
    Command* cmd = (Command*)arg;

    if (strcmp(cmd->type, "insert") == 0) {
        pthread_rwlock_wrlock(&rwlock);
        fprintf(out_fp, "%lld: WRITE LOCK ACQUIRED\n", timestamp());
        lock_acquire_count++;

        insert_record(cmd->name, cmd->salary);
        fprintf(out_fp, "%lld: INSERT,%u,%s,%u\n", timestamp(), jenkins_one_at_a_time_hash(cmd->name), cmd->name, cmd->salary);

        lock_release_count++;
        fprintf(out_fp, "%lld: WRITE LOCK RELEASED\n", timestamp());
        pthread_rwlock_unlock(&rwlock);

        pthread_mutex_lock(&insert_mutex);
        inserts_remaining--;
        if (inserts_remaining == 0) {
            pthread_cond_broadcast(&inserts_done);
        }
        pthread_mutex_unlock(&insert_mutex);

    } else if (strcmp(cmd->type, "delete") == 0) {
        pthread_mutex_lock(&insert_mutex);
        if (inserts_remaining > 0) {
            fprintf(out_fp, "%lld: WAITING ON INSERTS\n", timestamp());
            pthread_cond_wait(&inserts_done, &insert_mutex);
            fprintf(out_fp, "%lld: DELETE AWAKENED\n", timestamp());
        }
        pthread_mutex_unlock(&insert_mutex);

        pthread_rwlock_wrlock(&rwlock);
        fprintf(out_fp, "%lld: WRITE LOCK ACQUIRED\n", timestamp());
        lock_acquire_count++;

        delete_record(cmd->name);
        fprintf(out_fp, "%lld: DELETE,%s\n", timestamp(), cmd->name);

        lock_release_count++;
        fprintf(out_fp, "%lld: WRITE LOCK RELEASED\n", timestamp());
        pthread_rwlock_unlock(&rwlock);

    } else if (strcmp(cmd->type, "search") == 0) {
        pthread_rwlock_rdlock(&rwlock);
        fprintf(out_fp, "%lld: READ LOCK ACQUIRED\n", timestamp());
        lock_acquire_count++;

        hashRecord* found = search_record(cmd->name);
        if (found) {
            fprintf(out_fp, "%lld: SEARCH,%u,%s,%u\n", timestamp(), found->hash, found->name, found->salary);
        } else {
            fprintf(out_fp, "%lld: SEARCH: NOT FOUND NOT FOUND\n", timestamp());
        }

        lock_release_count++;
        fprintf(out_fp, "%lld: READ LOCK RELEASED\n", timestamp());
        pthread_rwlock_unlock(&rwlock);
    }

    return NULL;
}

void run_threads(CommandList cmdlist) {
    out_fp = fopen("output.txt", "w");
    pthread_t threads[cmdlist.count];

    for (int i = 0; i < cmdlist.count; i++) {
        if (strcmp(cmdlist.list[i].type, "insert") == 0) {
            inserts_remaining++;
        }
    }

    for (int i = 0; i < cmdlist.count; i++) {
        pthread_create(&threads[i], NULL, thread_worker, &cmdlist.list[i]);
    }

    for (int i = 0; i < cmdlist.count; i++) {
        pthread_join(threads[i], NULL);
    }

    fprintf(out_fp, "Finished all threads.\n");
    print_final_summary(out_fp);
    fclose(out_fp);
}
