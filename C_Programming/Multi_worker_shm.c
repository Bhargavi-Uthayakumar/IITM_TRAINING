#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define WORKERS     6
#define ITERATIONS  50000   

static int shmid = -1;     

void cleanup_shm(int sig) {
    if (shmid != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "cleanup: shmctl(IPC_RMID) failed: %s\n", strerror(errno));
        } else {
            fprintf(stderr, "cleanup: removed shared memory segment\n");
        }
        shmid = -1;
    }
    if (sig) exit(0);
}

int main(void) {
    pid_t pids[WORKERS];
    int *counters = NULL;
    size_t bytes = WORKERS * sizeof(int);

    struct sigaction sa;
    sa.sa_handler = cleanup_shm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    shmid = shmget(IPC_PRIVATE, bytes, IPC_CREAT | IPC_EXCL | 0600);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    counters = (int *) shmat(shmid, NULL, 0);
    if (counters == (void *) -1) {
        perror("shmat");
        cleanup_shm(0);
        exit(EXIT_FAILURE);
    }

   for (int i = 0; i < WORKERS; ++i) counters[i] = 0;

     for (int i = 0; i < WORKERS; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            for (int j = 0; j < i; ++j) kill(pids[j], SIGTERM);
            shmdt(counters);
            cleanup_shm(0);
            exit(EXIT_FAILURE);
        } else if (pid == 0) {

            int *local_counters = (int *) shmat(shmid, NULL, 0);
            if (local_counters == (void *) -1) {
                perror("child shmat");
                _exit(EXIT_FAILURE);
            }

            for (int k = 0; k < ITERATIONS; ++k) {
                local_counters[i] += 1;

            }

            shmdt(local_counters);
            _exit(0);
        } else {
            pids[i] = pid;
        }
    }

    for (int i = 0; i < WORKERS; ++i) {
        int status;
        pid_t w = waitpid(pids[i], &status, 0);
        if (w == -1) {
            perror("waitpid");
        } else {
            if (WIFEXITED(status)) {
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "worker %d (pid %d) terminated by signal %d\n",
                        i, pids[i], WTERMSIG(status));
            }
        }
    }

    long total = 0;
    printf("Per-worker counters:\n");
    for (int i = 0; i < WORKERS; ++i) {
        printf("  worker %d: %d\n", i, counters[i]);
        total += counters[i];
    }
    printf("Total (should be %d * %d = %d): %ld\n",
           WORKERS, ITERATIONS, WORKERS * ITERATIONS, total);

    if (shmdt(counters) == -1) {
        perror("shmdt");
    }
    cleanup_shm(0);

    return 0;
}
