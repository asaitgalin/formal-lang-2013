#include <stdlib.h>
#include <string.h>

#include "variable_list.h"

int varList_init(variableList_t *list) {
    if (!list) {
        return -1;
    }
    list->front = NULL;
    list->back = NULL;
    return 0;    
}

int varList_pushBack(variableList_t *list, const char *name, int value) {
    if (!name || !list) {
        return -1;
    }
    varNode_t *node = (varNode_t *)calloc(1, sizeof(varNode_t));
    if (!node) {
        return -1;
    }
    int len = strlen(name);
    node->name = (char *)calloc(len + 1, sizeof(char));
    if (!node->name) {
        free(node);
        return -1;
    } 
    strncpy(node->name, name, len);
    node->value = value;
    node->next = NULL;
    if (!list->front) {
        list->front = node;
    }
    if (!list->back) {
        list->back = node;
    } else {
        list->back->next = node;
        list->back = node;
    }
    return 0;
}

varNode_t *varList_find(variableList_t *list, const char *name) {
    if (!name || !list) {
        return NULL;
    }
    varNode_t *first = list->front;
    while (first) {
        if (!strcmp(first->name, name)) {
            return first;
        }
        first = first->next;
    }
    return NULL;
}

void varList_clear(variableList_t *list) {
    varNode_t *node = NULL;
    while (list->front) {
        node = list->front;
        list->front = node->next;
        free(node->name);
        free(node);
    }
    list->front = NULL;
    list->back = NULL;
}

