struct node {
    int key;
    int val;
    struct node *next;
    struct node *prev;
};

typedef struct {
    int cap;
    int count;    
} LRUCache;

struct node *head;
struct node *tail;

struct node* rem[100000];

LRUCache* lRUCacheCreate(int capacity) 
{
    LRUCache *cache = malloc(sizeof(LRUCache));
    cache->cap = capacity;
    cache->count = 0;
    head = malloc(sizeof(struct node));
    tail = malloc(sizeof(struct node));

    head->next = tail;
    tail->prev = head;

    for (int i = 0; i < 10001; i++)
        rem[i] = NULL;
    
    return cache;   
}
void del(struct node *curr) {

    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
}

void add(struct node *curr) {

    curr->next = head->next;
    curr->prev = head;
    
    head->next->prev = curr;
    head->next = curr;

}

int lRUCacheGet(LRUCache* obj, int key) {
    if(obj->count == 0)
            return -1;

        if(rem[key] == NULL)
            return -1;

        int val;
        struct node *curr = rem[key];
        val = curr->val;
        del(curr);
        add(curr);
        return val;
}

void lRUCachePut(LRUCache* obj, int key, int value) {
      printf("key=%d\n",key);
        if(rem[key] != NULL) {

        struct node *curr = rem[key];
        curr->val = value;
        del(curr);
        add(curr);
        }
    
        else {

            if(obj->count == obj->cap) {
                obj->count--;
                rem[tail->prev->key] = NULL;
                del(tail->prev);
            }

            obj->count++;
            struct node *curr = malloc(sizeof(struct node));
            curr->val = value;
            curr->key = key;
            printf("curr=%x\n",curr);
            printf("add key=%d val =%d \n",key,value);
            rem[key] = curr;
            add(curr);
        }
  
}

void lRUCacheFree(LRUCache* obj) {
    for(int i = 0; i <= 10000; i++)
        if(rem[i])
            free(rem[i]);
    
    free(head);
    free(tail);
    free(obj);
}

/**
 * Your LRUCache struct will be instantiated and called as such:
 * LRUCache* obj = lRUCacheCreate(capacity);
 * int param_1 = lRUCacheGet(obj, key);
 
 * lRUCachePut(obj, key, value);
 
 * lRUCacheFree(obj);
*/