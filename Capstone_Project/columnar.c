#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[64];    
    int *data;        
    size_t count;     
    size_t capacity;  
} Column;

Column* column_create(const char *name, size_t capacity) {
    Column *col = (Column*)malloc(sizeof(Column));
    if (!col) {
        perror("Failed to allocate Column struct");
        return NULL;
    }

    strncpy(col->name, name, sizeof(col->name) - 1);
    col->name[sizeof(col->name) - 1] = '\0';

    col->data = (int*)malloc(sizeof(int) * capacity);
    if (!col->data) {
        perror("Failed to allocate column data");
        free(col);
        return NULL;
    }

    col->count = 0;
    col->capacity = capacity;
    return col;
}

void column_free(Column *col) {
    if (!col) return;
    free(col->data);
    free(col);
}

