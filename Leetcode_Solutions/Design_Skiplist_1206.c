
#define MAX_LEVEL 16
#define P_FACTOR 0.5

typedef struct SkipListNode{
    int key;
    struct SkipListNode **farword;
}SkipListNode;

typedef struct {
    int level;
    SkipListNode *header;
} Skiplist;

SkipListNode* createNode(int key, int level){
    SkipListNode* newNode = (SkipListNode *)malloc(sizeof(SkipListNode));
    if(newNode==NULL) return NULL;
    newNode->key = key;
    newNode->farword = (SkipListNode**)malloc(sizeof(SkipListNode*)*(level+1));
    for(int i=0; i<level+1; i++)
    {
        newNode->farword[i] = NULL;
    }
    
    return newNode;
}

int generateRandomLevel(){
    int level = 0;

    while((rand()/(double)RAND_MAX) < P_FACTOR && level < MAX_LEVEL)
    {
        level++;
    }

    return level;
}
Skiplist* skiplistCreate() {
    srand(time(NULL));
    Skiplist *list = (Skiplist *)malloc(sizeof(Skiplist));
    if(list==NULL) return NULL;

    list->level = 0;
    list->header = createNode(-1, MAX_LEVEL);

    return list;
}

bool skiplistSearch(Skiplist* obj, int target) {
    SkipListNode *curr = obj->header;
    for(int i = obj->level; i>=0; i--)
    {
        while(curr->farword[i] && curr->farword[i]->key < target)
        {
            curr = curr->farword[i];
        }
    }
    curr = curr->farword[0];
    if(curr && curr->key==target)
    {
        return true;
    }
    return false;
}

void skiplistAdd(Skiplist* obj, int num) {
    SkipListNode *updates[MAX_LEVEL+1];
    SkipListNode *curr = obj->header;

    for(int i=obj->level; i>=0; i--)
    {
        while(curr->farword[i] && curr->farword[i]->key < num)
        {
            curr = curr->farword[i];
        }
        updates[i] = curr;
    }

    curr = curr->farword[0];
    // if(curr && curr->key==num)
    // {
    //     return;
    // }

    int newLevel = generateRandomLevel();

    if(newLevel > obj->level)
    {
        for(int i=obj->level+1; i<=newLevel; i++)
        {
            updates[i] = obj->header;
        }
        obj->level = newLevel;
    }

    SkipListNode *newNode = createNode(num, newLevel);
    
    for(int i=0; i<=newLevel; i++)
    {
        newNode->farword[i] = updates[i]->farword[i];
        updates[i]->farword[i] = newNode;
    }
    
}

bool skiplistErase(Skiplist* obj, int num) {
    SkipListNode *updates[MAX_LEVEL+1];
    SkipListNode *curr = obj->header;

    for(int i=obj->level; i>=0; i--)
    {
        while(curr->farword[i] && curr->farword[i]->key < num)
        {
            curr = curr->farword[i];
        }
        updates[i] = curr;
    }

    curr = curr->farword[0];

    if(curr==NULL || curr->key!=num)
    {
        return false;
    }

    if(curr && curr->key == num)
    {
        for(int i=0; i<obj->level+1; i++)
        {
            if (updates[i]->farword[i] == curr) {
                updates[i]->farword[i] = curr->farword[i];
            }
        }
    }
    free(curr);
    while(obj->level>0 && obj->header->farword[obj->level]==NULL)
    {
        obj->level--;
    }
    return true;
}

void skiplistFree(Skiplist* obj) {
    SkipListNode *curr = obj->header;
    SkipListNode *prev = NULL;

    while(curr)
    {
        SkipListNode *next = curr->farword[0];
        free(curr->farword);
        free(curr);
        curr = next;
    }
    free(obj);
}

/**
 * Your Skiplist struct will be instantiated and called as such:
 * Skiplist* obj = skiplistCreate();
 * bool param_1 = skiplistSearch(obj, target);
 
 * skiplistAdd(obj, num);
 
 * bool param_3 = skiplistErase(obj, num);
 
 * skiplistFree(obj);
*/
