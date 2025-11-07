#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COLUMNS 10

typedef struct {
    char name[64];    
    int *data;        
    size_t count;     
    size_t capacity;  
} Column;

typedef struct {
    char name[64];
    Column *columns[MAX_COLUMNS];
    size_t col_count;
    size_t row_count;
} Table;

Table *Employee;


Column* column_create(const char *name, size_t capacity) 
{
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

Table *table_create(const char *name)
{
    Table *Emp_Table = malloc(sizeof(Table));
    if(!Emp_Table)
    {
        perror("Failed to allocate table");
    }

    strncpy(Emp_Table->name, name, sizeof(Emp_Table)-1);
    Emp_Table->name[sizeof(Emp_Table)-1] = '\0';
    Emp_Table->col_count = 0;
    Emp_Table->row_count = 0;
    return Emp_Table;
}

void table_get_column(Table *Emp_table, const char *name)
{
    if (Emp_table->col_count >= MAX_COLUMNS)
    {
        fprintf(stderr, "\n Error: Reached maximum number of column ");
        return;
    }
    Emp_table->columns[Emp_table->col_count++] = column_create(name, 16);
}

void table_free(Table *Emp_Table)
{
    if(!Emp_Table)
        return;
    for(size_t i = 0; i < Emp_Table->col_count; i++)
    {
        if(Emp_Table->columns[i])
            column_free(Emp_Table->columns[i]);
    }
    free(Emp_Table);
}