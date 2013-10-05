#ifndef _VARIABLE_TABLE_H_
#define _VARIABLE_TABLE_H_

#include "variable_list.h"

typedef struct {
    size_t size;
    size_t capacity;
    variableList_t *items;
} hashTable_t;

int hashTable_init(size_t initialSize, hashTable_t *table);
int hashTable_setValue(hashTable_t *table, const char *name, int value);
int hashTable_getValue(hashTable_t *table, const char *name, int *value);
void hashTable_clear(hashTable_t *table);

#endif
