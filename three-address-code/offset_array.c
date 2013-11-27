#include <stdlib.h>

#include "offset_array.h"

int offsetArray_init(size_t initialCapacity, offsetArray_t *array) {
    if (!array || initialCapacity == 0) {
        return -1;
    }
    array->values = (int *)calloc(initialCapacity, sizeof(int));
    if (!array->values) {
        return -1;
    }
    array->capacity = initialCapacity;
    array->size = 0;
    return 0;
}

int offsetArray_realloc(offsetArray_t *array) {
    size_t newCapacity = array->capacity * 2; 
    int *newValues = (int *)realloc(array->values, newCapacity * sizeof(int));
    if (!newValues) {
        return -1;
    }
    array->values = newValues;
    array->capacity = newCapacity;
    return 0;
}

int offsetArray_put(offsetArray_t *array, int value) {
    if (!array) {
        return -1;
    }
    if (array->size + 1 == array->capacity) {
        if (offsetArray_realloc(array) != 0) {
            return -1;
        }
    }
    array->values[array->size++] = value;
    return 0;
}

int offsetArray_get(offsetArray_t *array, size_t index, int *value) {
    if (index >= array->size || index < 0) {
        return -1;
    }
    *value = array->values[index];
    return 0;
}

void offsetArray_clear(offsetArray_t *array) {
    if (array) {
        free(array->values);
    }
}

