#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

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

    strncpy(Emp_Table->name, name, sizeof(Emp_Table->name)-1);
    Emp_Table->name[sizeof(Emp_Table->name)-1] = '\0';
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

void execute_insert(char *table, int *values, int count) {
    if (strcmp(table, Employee->name) != 0) {
        printf("Unknown table '%s'\n", table);
        free(values);
        return;
    }
    else if (count != Employee->col_count) {
        printf("Expected %zu values, got %d.\n", Employee->col_count, count);
        free(values);
        return;
    }
    else
    {
        for (int i = 0; i < count; i++) {
        Column *col = Employee->columns[i];
        if (col->count >= col->capacity) {
            col->capacity *= 2;
            col->data = realloc(col->data, sizeof(int) * col->capacity);
            if (!col->data) {
                perror("Realloc failed");
                exit(EXIT_FAILURE);
            }
        }
        col->data[col->count++] = values[i];
    }
    Employee->row_count++;
    printf("Inserted row into %s.\n", table);
    free(values);
}
}

void column_aggregate(Column *col, const char *agg) {
    if (col->count == 0) {
        printf("Column '%s' is empty.\n", col->name);
        return;
    }

    if (strcmp(agg, "SUM") == 0) {
        long long sum = 0;
        for (size_t i = 0; i < col->count; i++) sum += col->data[i];
        printf("SUM(%s) = %lld\n", col->name, sum);
    }
    else if (strcmp(agg, "AVG") == 0) {
        long long sum = 0;
        for (size_t i = 0; i < col->count; i++) sum += col->data[i];
        printf("AVG(%s) = %.2f\n", col->name, (double)sum / col->count);
    }
    else if (strcmp(agg, "MIN") == 0) {
        int min = col->data[0];
        for (size_t i = 1; i < col->count; i++) if (col->data[i] < min) min = col->data[i];
        printf("MIN(%s) = %d\n", col->name, min);
    }
    else if (strcmp(agg, "MAX") == 0) {
        int max = col->data[0];
        for (size_t i = 1; i < col->count; i++) if (col->data[i] > max) max = col->data[i];
        printf("MAX(%s) = %d\n", col->name, max);
    }
    else if (strcmp(agg, "COUNT") == 0) {
        printf("COUNT(%s) = %zu\n", col->name, col->count);
    }
}

void execute_select(char *agg, char *column, char *table) {
    if (strcasecmp(table, Employee->name) != 0) {
        printf("Unknown table '%s'\n", table);
        return;
    }

    for (size_t i = 0; i < Employee->col_count; i++) {
        if (strcasecmp(Employee->columns[i]->name, column) == 0) {
            column_aggregate(Employee->columns[i], agg);
            return;
        }
    }
    printf("Unknown column '%s'\n", column);
}

int yyparse();

int main() {
    Employee = table_create("Employee");
    table_get_column(Employee, "salary");
    table_get_column(Employee, "age");
    table_get_column(Employee, "experience");

    printf("Mini Columnar SQL Engine (Flex+Bison)\n");
    printf("Example commands:\n");
    printf("  INSERT INTO Employee VALUES (50000, 30, 5);\n");
    printf("  SELECT SUM(salary) FROM Employee;\n");
    printf("Type Ctrl+D to quit.\n");

    yyparse(); 

    printf("Shutting down...\n");
    table_free(Employee);
    printf("Memory freed. Exiting.\n");

    return 0;
}