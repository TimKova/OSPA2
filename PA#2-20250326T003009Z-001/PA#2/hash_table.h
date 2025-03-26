#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdio.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

extern hashRecord* head;

uint32_t jenkins_one_at_a_time_hash(const char* key);
void insert_record(const char* name, uint32_t salary);
void delete_record(const char* name);
hashRecord* search_record(const char* name);
void print_table(FILE* out);
void print_final_summary(FILE* out);
long long timestamp();

#endif
