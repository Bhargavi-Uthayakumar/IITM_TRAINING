#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define TABLE_SIZE 8

typedef struct Node {
    char *key;
    int value;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    pthread_mutex_t lock;
} Bucket;

typedef struct {
    Bucket buckets[TABLE_SIZE];
} HashMap;

unsigned int hash(const char *key) {
    unsigned int h = 0;
    while (*key) {
        h = h * 31 + (unsigned char)(*key++);
    }
    return h % TABLE_SIZE;
}

void hashmap_init(HashMap *map) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        map->buckets[i].head = NULL;
        pthread_mutex_init(&map->buckets[i].lock, NULL);
    }
}

void hashmap_insert(HashMap *map, const char *key, int value) {
    unsigned int index = hash(key);
    Bucket *bucket = &map->buckets[index];

    pthread_mutex_lock(&bucket->lock);

    Node *node = bucket->head;
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;  
            pthread_mutex_unlock(&bucket->lock);
            return;
        }
        node = node->next;
    }

    Node *new_node = malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = bucket->head;
    bucket->head = new_node;

    pthread_mutex_unlock(&bucket->lock);
}

int hashmap_lookup(HashMap *map, const char *key, int *out_value) {
    unsigned int index = hash(key);
    Bucket *bucket = &map->buckets[index];

    pthread_mutex_lock(&bucket->lock);

    Node *node = bucket->head;
    while (node) {
        if (strcmp(node->key, key) == 0) {
            *out_value = node->value;
            pthread_mutex_unlock(&bucket->lock);
            return 1; 
        }
        node = node->next;
    }

    pthread_mutex_unlock(&bucket->lock);
    return 0; 
}

void hashmap_destroy(HashMap *map) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Bucket *bucket = &map->buckets[i];
        pthread_mutex_lock(&bucket->lock);
        Node *node = bucket->head;
        while (node) {
            Node *tmp = node;
            node = node->next;
            free(tmp->key);
            free(tmp);
        }
        pthread_mutex_unlock(&bucket->lock);
        pthread_mutex_destroy(&bucket->lock);
    }
}

typedef struct {
    HashMap *map;
    int thread_id;
} ThreadArg;

void *writer_thread(void *arg) {
    ThreadArg *targ = arg;
    char key[64];
    for (int i = 0; i < 15; i++) {
        sprintf(key, "key_%d_%d", targ->thread_id, i);
        hashmap_insert(targ->map, key, i);
        printf("[Writer %d] Inserted %s = %d\n", targ->thread_id, key, i);
        usleep(10000);
    }
    return NULL;
}

void *reader_thread(void *arg) {
    ThreadArg *targ = arg;
    char key[64];
    for (int i = 0; i < 20; i++) {
        int rnd_tid = rand() % 2 + 1;
        int rnd_idx = rand() % 15;
        sprintf(key, "key_%d_%d", rnd_tid, rnd_idx);
        int val;
        if (hashmap_lookup(targ->map, key, &val)) {
            printf("[Reader %d] Found %s = %d\n", targ->thread_id, key, val);
        } else {
            printf("[Reader %d] %s not found\n", targ->thread_id, key);
        }
        usleep(15000);
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    HashMap map;
    hashmap_init(&map);

    pthread_t threads[4];
    ThreadArg args[4];

    args[0].map = &map; args[0].thread_id = 1;
    args[1].map = &map; args[1].thread_id = 2;
    pthread_create(&threads[0], NULL, writer_thread, &args[0]);
    pthread_create(&threads[1], NULL, writer_thread, &args[1]);

    args[2].map = &map; args[2].thread_id = 3;
    args[3].map = &map; args[3].thread_id = 4;
    pthread_create(&threads[2], NULL, reader_thread, &args[2]);
    pthread_create(&threads[3], NULL, reader_thread, &args[3]);

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    hashmap_destroy(&map);
    return 0;
}
