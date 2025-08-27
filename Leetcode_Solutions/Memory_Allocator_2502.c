
typedef struct {
    int *memory; 
    int n;       
} Allocator;

Allocator* allocatorCreate(int n) {
    Allocator* obj = (Allocator*)malloc(sizeof(Allocator));
    obj->n = n;
    obj->memory = (int*)malloc(sizeof(int) * n);
    
    memset(obj->memory, -1, sizeof(int) * n); 
    return obj;
}


int allocatorAllocate(Allocator* obj, int size, int mID) {
    int consecutiveFree = 0;
    int startIndex = -1;

    for (int i = 0; i < obj->n; ++i) {
        if (obj->memory[i] == -1) { 
            consecutiveFree++;
            if (consecutiveFree == size) { 
                startIndex = i - size + 1;
                
                for (int j = startIndex; j <= i; ++j) {
                    obj->memory[j] = mID;
                }
                return startIndex;
            }
        } else { 
            consecutiveFree = 0;
        }
    }
    return -1; 
}

int allocatorFreeMemory(Allocator* obj, int mID) {
    int freedUnits = 0;
    for (int i = 0; i < obj->n; ++i) {
        if (obj->memory[i] == mID) {
            obj->memory[i] = -1; 
            freedUnits++;
        }
    }
    return freedUnits;
}

void allocatorFree(Allocator* obj) {
    free(obj->memory);
    free(obj);
}