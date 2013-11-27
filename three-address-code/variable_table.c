#include <stdlib.h>
#include <string.h>

#include "variable_table.h"

const float fillCoefficient = 0.75; // 3/4

unsigned int elfHash(const char* str, int len, size_t tableSize) {
    unsigned int hash = 0;
    unsigned int x = 0;
    for(int i = 0; i < len; ++str, ++i) {
        hash = (hash << 4) + (*str);
        if((x = hash & 0xF0000000L) != 0) {
            hash ^= (x >> 24);
        }
        hash &= ~x;
    }
    return hash % tableSize;
}

int hashTable_init(size_t initialCapacity, hashTable_t *table) {
    if (!table || initialCapacity == 0) {
        return -1;
    }
    table->size = 0;
    table->capacity = initialCapacity;
    table->items = (variableList_t *)calloc(table->capacity, sizeof(variableList_t));
    if (!table->items) {
        return -1;
    }
    for (int i = 0; i < table->capacity; ++i) {
        varList_init(&table->items[i]);
    }
    return 0;
}

int hashTable_rebuild(hashTable_t *table) {
    size_t newCapacity = table->capacity * 2;
    size_t newSize = table->size;
    variableList_t *newItems = (variableList_t *)calloc(newCapacity, sizeof(variableList_t));
    if (!newItems) {
        return -1;
    }
    for (int i = 0; i < table->capacity; ++i) {
        varNode_t *node = table->items[i].front;
        while (node) {
            unsigned int hash = elfHash(node->name, strlen(node->name), newCapacity);
            if (varList_pushBack(&newItems[hash], node->name, node->value) != 0) {
                free(newItems);
                return -1;
            }
            node = node->next;
        }
    }
    hashTable_clear(table);
    table->size = newSize;
    table->capacity = newCapacity;
    table->items = newItems;
    return 0;
}

void hashTable_clear(hashTable_t *table) {
    if (table) {
        for (int i = 0; i < table->capacity; ++i) {
            varList_clear(&table->items[i]);
        }
        free(table->items);
    }
}

int hashTable_checkRebuild(hashTable_t *table) {
    if (((float)table->size / (float)table->capacity) > fillCoefficient) {
        return hashTable_rebuild(table);
    }
    return 0;
}

int hashTable_setValue(hashTable_t *table, const char *name, int value) {
    if (!name) {
        return -1;
    }
    unsigned int hash = elfHash(name, strlen(name), table->capacity);
    varNode_t *node = varList_find(&table->items[hash], name);

    if (!node) {
        if (varList_pushBack(&table->items[hash], name, value) == 0) {
            ++table->size;
            return hashTable_checkRebuild(table);
        }
        return -1;
    }
    node->value = value;
    return 0;
}

int hashTable_getValue(hashTable_t *table, const char *name, int *value) {
    if (!name || !value) {
        return -1;
    }
    unsigned int hash = elfHash(name, strlen(name), table->capacity);
    varNode_t *node = varList_find(&table->items[hash], name);

    if (!node) {
        if (varList_pushBack(&table->items[hash], name, 0) == 0) {
            ++table->size;
            *value = 0;
            return hashTable_checkRebuild(table);
        }
        return -1;
    }
    *value = node->value;
    return 0;
}

