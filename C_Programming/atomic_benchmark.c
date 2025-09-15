#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

static long mutex_counter = 0;          
static atomic_long atomic_counter = 0; 

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    long iterations;
} benchmark_arg_t;

static void* worker_atomic(void *arg) {
    benchmark_arg_t *barg = (benchmark_arg_t *)arg;
    long iterations = barg->iterations;

    for (long i = 0; i < iterations; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}

static void* worker_mutex(void *arg) {
    benchmark_arg_t *barg = (benchmark_arg_t *)arg;
    long iterations = barg->iterations;

    for (long i = 0; i < iterations; i++) {
        pthread_mutex_lock(&counter_mutex);
        mutex_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

static double now_sec() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + (t.tv_nsec / 1e9);
}

int main(int argc, char **argv) {
    long num_threads   = (argc > 1) ? atol(argv[1]) : 64;      
    long num_iterations = (argc > 2) ? atol(argv[2]) : 2000000; 

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    benchmark_arg_t args = { num_iterations };

    printf("Benchmarking with %ld threads, %ld iterations each\n\n", num_threads, num_iterations);

    double start_mutex = now_sec();
    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_mutex, &args);
    }
    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    double end_mutex = now_sec();

    printf("Mutex Counter  : %ld (Expected %ld), Time = %.6f sec\n",
           mutex_counter, num_threads * num_iterations, end_mutex - start_mutex);

    double start_atomic = now_sec();
    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_atomic, &args);
    }
    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    double end_atomic = now_sec();

    printf("Atomic Counter : %ld (Expected %ld), Time = %.6f sec\n",
           atomic_counter, num_threads * num_iterations, end_atomic - start_atomic);

    free(threads);
    return 0;
}
