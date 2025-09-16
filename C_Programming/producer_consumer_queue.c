#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define PRODUCERS  3
#define CONSUMERS  2
#define ITEMS_PER_PRODUCER 8

typedef struct {
    int *buffer;
    int capacity;
    int in, out, count;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} BoundedQueue;

void queue_init(BoundedQueue *q, int capacity) {
    q->buffer = malloc(sizeof(int) * capacity);
    q->capacity = capacity;
    q->in = q->out = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void queue_destroy(BoundedQueue *q) {
    free(q->buffer);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
}

void enqueue(BoundedQueue *q, int item) {
    pthread_mutex_lock(&q->lock);

    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->buffer[q->in] = item;
    q->in = (q->in + 1) % q->capacity;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int dequeue(BoundedQueue *q) {
    pthread_mutex_lock(&q->lock);

    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    int item = q->buffer[q->out];
    q->out = (q->out + 1) % q->capacity;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);

    return item;
}

BoundedQueue queue;
int total_items = PRODUCERS * ITEMS_PER_PRODUCER;
int consumed_items = 0;

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = id * 100 + i;
        enqueue(&queue, item);
        printf("Producer %d produced %d\n", id, item);
        usleep((rand() % 200) * 1000); 
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&queue.lock);
        if (consumed_items >= total_items) {
            pthread_mutex_unlock(&queue.lock);
            break;
        }
        pthread_mutex_unlock(&queue.lock);

        int item = dequeue(&queue);

        pthread_mutex_lock(&queue.lock);
        consumed_items++;
        pthread_mutex_unlock(&queue.lock);

        printf("Consumer %d consumed %d (total=%d)\n", id, item, consumed_items);
        usleep((rand() % 300) * 1000); 
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    queue_init(&queue, QUEUE_SIZE);

    pthread_t prod_threads[PRODUCERS], cons_threads[CONSUMERS];
    int prod_ids[PRODUCERS], cons_ids[CONSUMERS];

    for (int i = 0; i < PRODUCERS; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
    }

    for (int i = 0; i < CONSUMERS; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons_threads[i], NULL, consumer, &cons_ids[i]);
    }

    for (int i = 0; i < PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    for (int i = 0; i < CONSUMERS; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    printf("All done. Total consumed = %d\n", consumed_items);

    queue_destroy(&queue);
    return 0;
}
