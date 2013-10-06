#ifndef _OFFSET_ARRAY_H_
#define _OFFSET_ARRAY_H_

typedef struct {
    int *values;
    size_t capacity;
    size_t size;
} offsetArray_t;

int offsetArray_init(size_t initialCapacity, offsetArray_t *array);
int offsetArray_put(offsetArray_t *array, int value);
int offsetArray_get(offsetArray_t *array, size_t index, int *value);
void offsetArray_clear(offsetArray_t *array);

#endif
