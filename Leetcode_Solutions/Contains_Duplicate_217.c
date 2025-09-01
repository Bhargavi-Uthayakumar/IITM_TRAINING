#define MAX_LEVEL 16
#define P 0.5

typedef struct SkiplistNode {
    int val;
    struct SkiplistNode* forward[MAX_LEVEL];
} SkiplistNode;

typedef struct {
    SkiplistNode* header;
    int level;
} Skiplist;

SkiplistNode* createNode(int level, int val) {
    SkiplistNode* node = (SkiplistNode*)malloc(sizeof(SkiplistNode));
    node->val = val;
    for (int i = 0; i < MAX_LEVEL; i++) node->forward[i] = NULL;
    return node;
}

Skiplist* skiplistCreate() {
    Skiplist* list = (Skiplist*)malloc(sizeof(Skiplist));
    list->header = createNode(MAX_LEVEL, INT_MIN);
    list->level = 1;
    return list;
}

int randomLevel() {
    int lvl = 1;
    while (((double)rand() / RAND_MAX) < P && lvl < MAX_LEVEL) {
        lvl++;
    }
    return lvl;
}

bool skiplistSearch(Skiplist* obj, int target) {
    SkiplistNode* curr = obj->header;
    for (int i = obj->level - 1; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->val < target) {
            curr = curr->forward[i];
        }
    }
    curr = curr->forward[0];
    return curr && curr->val == target;
}

void skiplistAdd(Skiplist* obj, int num) {
    SkiplistNode* update[MAX_LEVEL];
    SkiplistNode* curr = obj->header;

    for (int i = obj->level - 1; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->val < num) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    int lvl = randomLevel();
    if (lvl > obj->level) {
        for (int i = obj->level; i < lvl; i++) {
            update[i] = obj->header;
        }
        obj->level = lvl;
    }

    SkiplistNode* newNode = createNode(lvl, num);
    for (int i = 0; i < lvl; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

void skiplistFree(Skiplist* obj) {
    SkiplistNode* curr = obj->header;
    while (curr) {
        SkiplistNode* next = curr->forward[0];
        free(curr);
        curr = next;
    }
    free(obj);
}

bool containsDuplicate(int* nums, int numsSize) {
    Skiplist* list = skiplistCreate();

    for (int i = 0; i < numsSize; i++) {
        if (skiplistSearch(list, nums[i])) {
            skiplistFree(list);
            return true;
        }
        skiplistAdd(list, nums[i]);
    }

    skiplistFree(list);
    return false;
}