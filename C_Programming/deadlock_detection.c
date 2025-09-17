#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

#define MAX_KEYS 8
#define MAX_LOCKS 8
#define MAX_THREADS 4
#define MAX_VERSIONS 16

typedef struct Version {
    int value;
    unsigned long begin_ts;
    unsigned long end_ts;
    struct Version *next;
} Version;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t owner;
    int key_id;
} TrackLock;

typedef struct {
    TrackLock lock;
    Version *head;
} Row;

static Row rows[MAX_KEYS];
static pthread_t threads[MAX_THREADS];
static int abort_flag[MAX_THREADS];
static int wait_for[MAX_THREADS][MAX_THREADS];
static pthread_mutex_t wfg_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ts_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned long global_ts = 1UL;

static int equate(pthread_t t1, pthread_t t2) 
{  
    return pthread_equal(t1, t2); 
}

static int index_of(pthread_t thread) 
{
    for (int i = 0; i < MAX_THREADS; i++)
        if (equate(threads[i], thread)) 
            return i;
    return -1;
}

static void wfg_add_edge(int waiter, int owner) 
{
    pthread_mutex_lock(&wfg_mutex);
    wait_for[waiter][owner] = 1;
    pthread_mutex_unlock(&wfg_mutex);
}

static void wfg_remove_all_from(int waiter)
{
    pthread_mutex_lock(&wfg_mutex);
    for (int i = 0; i < MAX_THREADS; i++) 
        wait_for[waiter][i] = 0;
    pthread_mutex_unlock(&wfg_mutex);
}

static int detect_cycle_dfs(int u, int *vis, int *stk) 
{
    vis[u] = 1;
    stk[u] = 1;
    for (int v = 0; v < MAX_THREADS; v++) 
    {
        if (!wait_for[u][v]) 
            continue;
        if (!vis[v] && detect_cycle_dfs(v, vis, stk)) 
            return 1;
        if (stk[v]) 
            return 1;
    }
    stk[u] = 0;
    return 0;
}

static int has_cycle(void) 
{
    int vis[MAX_THREADS] = {0}, stk[MAX_THREADS] = {0};

    for (int i = 0; i < MAX_THREADS; i++)
        if (!vis[i] && detect_cycle_dfs(i, vis, stk)) 
            return 1;
    return 0;
}

static void dump_wfg(void) 
{
    pthread_mutex_lock(&wfg_mutex);
    printf("WFG:\n");

    for (int i = 0; i < MAX_THREADS; i++) 
    {
        for (int j = 0; j < MAX_THREADS; j++) 
            printf("%d ", wait_for[i][j]);
        printf("\n");
    }
    pthread_mutex_unlock(&wfg_mutex);
}

static int track_acquire(TrackLock *lk) 
{
    pthread_t self = pthread_self();
    int self_idx = index_of(self);
    pthread_mutex_lock(&lk->mutex);

    while (lk->owner && !equate(lk->owner, self)) 
    {
        int owner_idx = index_of(lk->owner);
        if (self_idx >= 0 && owner_idx >= 0) 
            wfg_add_edge(self_idx, owner_idx);
        pthread_cond_wait(&lk->cond, &lk->mutex);
        if (self_idx >= 0 && abort_flag[self_idx]) 
        {
            wfg_remove_all_from(self_idx);
            pthread_mutex_unlock(&lk->mutex);
            return 0;
        }
    }
    lk->owner = self;
    if (self_idx >= 0) 
        wfg_remove_all_from(self_idx);

    pthread_mutex_unlock(&lk->mutex);
    return 1;
}

static void track_release(TrackLock *lk) 
{
    pthread_mutex_lock(&lk->mutex);
    if (lk->owner && equate(lk->owner, pthread_self())) 
    {
        lk->owner = (pthread_t)0;
        pthread_cond_broadcast(&lk->cond);
    }
    pthread_mutex_unlock(&lk->mutex);
}

static unsigned long next_ts(void) 
{
    pthread_mutex_lock(&ts_mutex);
    unsigned long t = ++global_ts;
    pthread_mutex_unlock(&ts_mutex);
    return t;
}

static unsigned long begin_ts(void) 
{
    pthread_mutex_lock(&ts_mutex);
    unsigned long t = global_ts;
    pthread_mutex_unlock(&ts_mutex);
    return t;
}

static void row_init(Row *row, int key_id, int initial_value) 
{
    pthread_mutex_init(&row->lock.mutex, NULL);
    pthread_cond_init(&row->lock.cond, NULL);
    row->lock.owner = (pthread_t)0;
    row->lock.key_id = key_id;

    Version *v = malloc(sizeof(Version));

    v->value = initial_value;
    v->begin_ts = 1UL;
    v->end_ts = ULONG_MAX;
    v->next = NULL;
    row->head = v;
}

static int mvcc_read(Row *row, unsigned long start_ts) 
{
    pthread_mutex_lock(&row->lock.mutex);
    Version *v = row->head;
    while (v) 
    {
        if (v->begin_ts <= start_ts && start_ts < v->end_ts) 
        {
            int val = v->value;
            pthread_mutex_unlock(&row->lock.mutex);
            return val;
        }
        v = v->next;
    }
    pthread_mutex_unlock(&row->lock.mutex);
    return -1;
}

static void mvcc_write_commit(Row *row, int new_value) 
{
    unsigned long commit_ts = next_ts();
    pthread_mutex_lock(&row->lock.mutex);

    Version *old = row->head;
    if (old && old->end_ts == ULONG_MAX) 
        old->end_ts = commit_ts;
    
        Version *new = malloc(sizeof(Version));

    new->value = new_value;
    new->begin_ts = commit_ts;
    new->end_ts = ULONG_MAX;
    new->next = row->head;
    row->head = new;
    pthread_mutex_unlock(&row->lock.mutex);
}

static void *monitor_function(void *arg) 
{
    (void)arg;
    for (;;) 
    {
        usleep(100000);
        pthread_mutex_lock(&wfg_mutex);
        int cyc = has_cycle();
        pthread_mutex_unlock(&wfg_mutex);

        if (cyc) 
        {
            dump_wfg();
            int victim = 1;
            abort_flag[victim] = 1;
            printf("Deadlock detected. Aborting T%d\n", victim);
            for (int i = 0; i < MAX_KEYS; i++)
            {
                pthread_mutex_lock(&rows[i].lock.mutex);
                pthread_cond_broadcast(&rows[i].lock.cond);
                pthread_mutex_unlock(&rows[i].lock.mutex);
            }
            break;
        }
    }
    return NULL;
}

static void *worker(void *arg) 
{
    int tid = (int)(intptr_t)arg;
    unsigned long s = begin_ts();

    printf("T%d begin_ts=%lu\n", tid, s);

    if (!track_acquire(&rows[0].lock))
    {
         printf("T%d aborted on key0\n", tid); 
         return NULL;
    }
    int val0 = mvcc_read(&rows[0], s);

    printf("T%d read key0=%d\n", tid, val0);
    usleep(200000);

    if (!track_acquire(&rows[1].lock)) 
    { 
        printf("T%d aborted on key1\n", tid); 
        track_release(&rows[0].lock); 
        return NULL; 
    }

    int val1 = mvcc_read(&rows[1], s);

    printf("T%d read key1=%d\n", tid, val1);
    
    mvcc_write_commit(&rows[1], val1 + 10);
    
    printf("T%d wrote key1=%d\n", tid, val1 + 10);
    
    track_release(&rows[1].lock);
    track_release(&rows[0].lock);
    
    return NULL;
}

int main(void) 
{
    for (int i = 0; i < MAX_KEYS; i++) 
        row_init(&rows[i], i, 100 + i);

    pthread_t monitor_thread;
    
    pthread_create(&monitor_thread, NULL, monitor_function, NULL);

    for (int i = 0; i < 2; i++)
    {
        pthread_create(&threads[i], NULL, worker, (void*)(intptr_t)i);
    }

    for (int i = 0; i < 2; i++) pthread_join(threads[i], NULL);

    pthread_join(monitor_thread, NULL);
    printf("Demo complete.\n");

    return 0;
}
