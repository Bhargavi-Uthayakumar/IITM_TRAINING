#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

static long long timespec_to_ns(const struct timespec *t)
{
    return (long long)t->tv_sec * 1000000000LL + t->tv_nsec;
}

int main(int argc, char **argv)
{
    const char *filename = "/tmp/aio_demo.bin";

    size_t write_size = 4096;
    size_t total_mb = 32;

    if(argc >= 2)
    {
        filename = argv[1];
    }

    if(argc >= 3)
    {
        write_size = (size_t)atoi(argv[2]);
    }

    if(argc >= 4)
    {
        write_size = (size_t)atoi(argv[3]);
    }

    size_t iterations = (total_mb * 1024 * 1024) / write_size;

    if(iterations == 0)
    {
        fprintf(stderr, "Either increase the total_mb or reduce the write_size \n");
        return 1;
    }

    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if(fd < 0)
    {
        perror("open");
        return 1;
    }

    unsigned char *buffer = malloc(write_size);

    if(!buffer)
    {
        perror("malloc");
        return 1;
    }

    for (size_t iterator = 0; iterator < iterations; ++iterator)
    {
        buffer[iterator] = (unsigned char)(iterator & 0xFF);
    }

    struct aiocb *cbs = calloc(iterations, sizeof(struct aiocb));

    if(!cbs)
    {
        perror("calloc");
        return 1;
    }

    struct timespec tstart, tend;

    long long sum_ns = 0, max_ns = 0, min_ns = (1LL<<62);

    printf("POSIX AIO demo : %zu ops of %zu bytes \n", iterations, write_size);

    for (size_t iterator = 0; iterator < iterations; ++iterator)
    {
        off_t offset = iterator * (off_t)write_size;

        memset(&cbs[iterator], 0, sizeof(struct aiocb));

        cbs[iterator].aio_fildes = fd;
        cbs[iterator].aio_buf = buffer;
        cbs[iterator].aio_nbytes = write_size;
        cbs[iterator].aio_offset = offset;

        if(clock_gettime(CLOCK_MONOTONIC, &tstart) < 0)
        {
            perror("clock get time");
            return 1;
        }

        if(aio_write(&cbs[iterator]) < 0)
        {
            perror("aio_write");
            return 1;
        }

        const struct aiocb *list[1] = {&cbs[iterator]};
        int ret = aio_suspend(list, 1, NULL);
        if(ret < 0)
        {
            perror("aio_suspend");
            return 1;
        }

        int err = aio_error(&cbs[iterator]);
        if(err != 0)
        {
            fprintf(stderr, "aio_error : %s \n", strerror(err));
            return 1;
        }

        ssize_t r = aio_return(&cbs[iterator]);

        if(r != (ssize_t)write_size)
        {
            fprintf(stderr, "aio_return short %zd \n", r);
            return 1;
        }

        if(clock_gettime(CLOCK_MONOTONIC, &tend) < 0)
        {
            perror("clock get time");
            return 1;
        }

        long long ns = timespec_to_ns(&tend) - timespec_to_ns(&tstart);
        
        sum_ns += ns;

        if(ns < min_ns)
        {
            min_ns = ns;
        }

        if(ns > max_ns)
        {
            max_ns = ns;
        }
    }

    double avg_ns = (double)sum_ns / iterations / 1e6;

    printf("POSIX AIO : ops = %zu, avg = %.3f ms, min = %.3f ms, max = %.3f ms \n", iterations, avg_ns, min_ns/1e6, max_ns/1e6);

    close(fd);
    free(buffer);
    free(cbs);

    return 0;
}