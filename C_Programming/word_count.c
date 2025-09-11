#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_THREADS 4

long totalwordCount = 0;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    int fd;
    off_t start;
    off_t end;
    int threadID;
} ThreadArg;

int is_separator(char c)
{
    return isspace((unsigned char)c);
}

void *word_count(void *arg)
{
    ThreadArg *targ = arg;
    int fd = targ->fd;
    off_t start = targ->start;
    off_t end = targ->end;

    char buf[4096];
    int in_word = 0;
    long count = 0;
    off_t pos = start;

    while (pos < end)
    {
        size_t to_read = sizeof(buf);
        if (end - pos < (off_t)to_read)
            to_read = end - pos;

        ssize_t n = pread(fd, buf, to_read, pos);
        if (n <= 0)
            break;

        for (ssize_t i = 0; i < n; i++)
        {
            if (is_separator(buf[i]))
            {
                if (in_word)
                {
                    count++;
                    in_word = 0;
                }
            }
            else
            {
                in_word = 1;
            }
        }
        pos += n;
    }

    if (in_word)
        count++;
    pthread_mutex_lock(&id_mutex);
    totalwordCount += count;
    printf("Total count from this thread ID - %d : %ld\n", targ->threadID, count);
    printf("Starting from offset : %lld  ,Ending offset : %lld\n\n", targ->start, targ->end);
    pthread_mutex_unlock(&id_mutex);

    free(targ); 
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open file");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("fstat");
        return 1;
    }

    if (st.st_size == 0)
    {
        fprintf(stderr, "File is empty\n");
        return 1;
    }

    off_t chunk = st.st_size / MAX_THREADS;
    pthread_t counters[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        ThreadArg *targ = malloc(sizeof(ThreadArg));
        targ->fd = fd;
        targ->threadID = i + 1;
        targ->start = i * chunk;
        targ->end = (i == MAX_THREADS - 1) ? st.st_size : (i + 1) * chunk;

        pthread_create(&counters[i], NULL, word_count, targ);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(counters[i], NULL);
    }

    printf("Total words: %ld\n", totalwordCount);
    pthread_mutex_destroy(&id_mutex);
    close(fd);
    return 0;
}

