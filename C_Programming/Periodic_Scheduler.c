#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define PERIOD_MS 500    
#define RUN_COUNT 10    

static int task_count = 0;
static struct timespec start_time, prev_time;

double elapsed_ms(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * 1000.0 +
           (end->tv_nsec - start->tv_nsec) / 1.0e6;
}

void periodic_task(int sig)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    double elapsed = elapsed_ms(&start_time, &now);
    double delta = elapsed_ms(&prev_time, &now);

    printf("Task %d executed | Total elapsed: %.3f ms | Interval: %.3f ms\n",
           ++task_count, elapsed, delta);

    prev_time = now; 

    if (task_count >= RUN_COUNT)
    {
        printf("\nReached %d executions. Stopping scheduler.\n", RUN_COUNT);
        exit(0);
    }
}

int main()
{
    struct itimerval timer;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    prev_time = start_time;

    struct sigaction sa;
    sa.sa_handler = &periodic_task;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    timer.it_interval.tv_sec = PERIOD_MS / 1000;
    timer.it_interval.tv_usec = (PERIOD_MS % 1000) * 1000;
    timer.it_value = timer.it_interval; 

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
    {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    printf("Scheduler started with period: %d ms\n", PERIOD_MS);
    printf("Executing %d periodic tasks...\n\n", RUN_COUNT);

    while (1)
        pause();

    return 0;
}
