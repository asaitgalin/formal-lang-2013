#include <stdio.h>
#include <stdlib.h>

#include "variable_table.h"

int main() {
    hashTable_t table;
    hashTable_init(7, &table);
    hashTable_setValue(&table, "var1", 5);
    hashTable_setValue(&table, "var1", 6);

    hashTable_setValue(&table, "var2", 1);
    hashTable_setValue(&table, "var3", 2);
    
    hashTable_setValue(&table, "var4", 3);
    hashTable_setValue(&table, "var5", 4);
    hashTable_setValue(&table, "var6", 5);

    hashTable_setValue(&table, "var7", 6);
    hashTable_setValue(&table, "var8", 7);
    hashTable_setValue(&table, "var9", 8);

    int value;
    hashTable_getValue(&table, "var1", &value);
    printf("Our value in var1: %d\n", value);
    
    int value2;
    hashTable_getValue(&table, "var5", &value2);
    printf("Out value in var5: %d\n", value2);

    hashTable_clear(&table);
    return 0;
}
