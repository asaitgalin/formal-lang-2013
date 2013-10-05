#ifndef _VARIABLE_WORKER_
#define _VARIABLE_WORKER_

struct varNode_tag;

struct varNode_tag {
    char *name;
    int value;
    struct varNode_tag *next;
};

typedef struct varNode_tag varNode_t;

typedef struct {
    varNode_t *front;
    varNode_t *back;
} variableList_t;

int varList_init(variableList_t *list);
int varList_pushBack(variableList_t *list, const char *name, int value);
varNode_t *varList_find(variableList_t *list, const char *name);
void varList_clear(variableList_t *list);

#endif

