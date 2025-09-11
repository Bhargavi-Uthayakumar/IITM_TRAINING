#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <liburing.h>

static inline long long ts_ns(const struct timespec *t) {
    return (long long)t->tv_sec * 1000000000LL + t->tv_nsec;
}

int main(int argc, char **argv) {
    const char *filename = "/tmp/io_uring_demo.bin";
    size_t write_size = 4096;
    size_t total_mb = 64;
    if (argc >= 2) filename = argv[1];
    if (argc >= 3) write_size = (size_t)atoi(argv[2]);
    if (argc >= 4) total_mb = (size_t)atoi(argv[3]);

    size_t iterations = (total_mb * 1024 * 1024) / write_size;
    if (iterations == 0) { fprintf(stderr, "increase total_mb or reduce write_size\n"); return 1; }

    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }

    unsigned char *buffer = aligned_alloc(4096, write_size); 
    if (!buffer) { perror("aligned_alloc"); return 1; }
    for (size_t i = 0; i < write_size; ++i) buffer[i] = (unsigned char)(i & 0xFF);

    struct io_uring ring;
    if (io_uring_queue_init(256, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        return 1;
    }

    struct timespec t0, t1;
    long long sum_ns = 0, min_ns = (1LL<<62), max_ns = 0;

    for (size_t i = 0; i < iterations; ++i) {
        off_t offset = i * (off_t)write_size;
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if (!sqe) { fprintf(stderr, "io_uring_get_sqe returned NULL\n"); return 1; }

        io_uring_prep_write(sqe, fd, buffer, write_size, offset);

        if (clock_gettime(CLOCK_MONOTONIC, &t0) < 0) { perror("clock_gettime"); return 1; }

        io_uring_submit(&ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) { fprintf(stderr, "io_uring_wait_cqe: %s\n", strerror(-ret)); return 1; }

        if (cqe->res < 0) { fprintf(stderr, "cqe->res err: %s\n", strerror(-cqe->res)); return 1; }
        if ((size_t)cqe->res != write_size) { fprintf(stderr, "short write cqe->res=%d\n", cqe->res); return 1; }

        if (clock_gettime(CLOCK_MONOTONIC, &t1) < 0) { perror("clock_gettime"); return 1; }

        io_uring_cqe_seen(&ring, cqe);

        long long ns = ts_ns(&t1) - ts_ns(&t0);
        sum_ns += ns;
        if (ns < min_ns) min_ns = ns;
        if (ns > max_ns) max_ns = ns;
    }

    double avg_ms = (double)sum_ns / iterations / 1e6;
    printf("io_uring: ops=%zu avg=%.3fms min=%.3fms max=%.3fms\n", iterations, avg_ms, min_ns/1e6, max_ns/1e6);

    io_uring_queue_exit(&ring);
    close(fd);
    free(buffer);
    return 0;
}