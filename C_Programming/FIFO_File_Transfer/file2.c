#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX 100

int main() {
    char buf[MAX];
    int read_fd  = open("fifo1", O_RDONLY);
    int write_fd = open("fifo2", O_WRONLY);

    if (write_fd < 0 || read_fd < 0) {
        perror("open");
        exit(1);
    }

    printf("Process B started. Waiting for messages...\n");
    while (1) {
        int n = read(read_fd, buf, MAX);
        if (n > 0) {
            printf("fileA: %s", buf);

            printf("fileB: ");
            fflush(stdout);
            if (!fgets(buf, MAX, stdin)) break;
            write(write_fd, buf, strlen(buf)+1);
        }
    }

    close(write_fd);
    close(read_fd);
    return 0;
}