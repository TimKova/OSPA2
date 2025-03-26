#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hash_table.h"
#include <pthread.h>

hashRecord* head = NULL;
extern int lock_acquire_count;
extern int lock_release_count;
extern pthread_rwlock_t rwlock;

long long timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000LL + tv.tv_usec;
}

uint32_t jenkins_one_at_a_time_hash(const char* key) {
    size_t i = 0;
    uint32_t hash = 0;
    while (key[i] != '\0') {
        hash += key[i++];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

void insert_record(const char* name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    hashRecord* new_node = malloc(sizeof(hashRecord));
    new_node->hash = hash;
    strncpy(new_node->name, name, 49);
    new_node->name[49] = '\0';
    new_node->salary = salary;
    new_node->next = NULL;

    if (!head || hash < head->hash) {
        new_node->next = head;
        head = new_node;
        return;
    }

    hashRecord* curr = head;
    hashRecord* prev = NULL;

    while (curr && curr->hash < hash) {
        prev = curr;
        curr = curr->next;
    }

    if (curr && curr->hash == hash) {
        strncpy(curr->name, name, 49);
        curr->name[49] = '\0';
        curr->salary = salary;
        free(new_node);
        return;
    }

    new_node->next = curr;
    if (prev) prev->next = new_node;
}

void delete_record(const char* name) {
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    hashRecord* curr = head;
    hashRecord* prev = NULL;

    while (curr) {
        if (curr->hash == hash) {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

hashRecord* search_record(const char* name) {
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    hashRecord* curr = head;
    while (curr) {
        if (curr->hash == hash) return curr;
        curr = curr->next;
    }
    return NULL;
}

void print_table(FILE* out) {
    hashRecord* curr = head;
    while (curr) {
        fprintf(out, "%u,%s,%u\n", curr->hash, curr->name, curr->salary);
        curr = curr->next;
    }
}

void print_final_summary(FILE* out) {
    fprintf(out, "\nNumber of lock acquisitions:  %d\n", lock_acquire_count);
    fprintf(out, "Number of lock releases:  %d\n", lock_release_count);

    pthread_rwlock_rdlock(&rwlock);
    fprintf(out, "%lld: READ LOCK ACQUIRED\n", timestamp());
    print_table(out);
    fprintf(out, "%lld: READ LOCK RELEASED\n", timestamp());
    pthread_rwlock_unlock(&rwlock);
}
