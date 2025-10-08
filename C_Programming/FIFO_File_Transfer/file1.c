#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX 100

int main() {
    char buf[MAX];
    int write_fd = open("fifo1", O_WRONLY);
    int read_fd = open("fifo2", O_RDONLY);

    if (write_fd < 0 || read_fd< 0) {
        perror("open");
        exit(1);
    }

    printf("Process A started. Type messages:\n");
    while (1) {
        printf("fileA: ");
        fflush(stdout);

        if (!fgets(buf, MAX, stdin)) break;
        write(write_fd, buf, strlen(buf)+1);

        int n = read(read_fd, buf, MAX);
        if (n > 0) {
            printf("fileB: %s", buf);
        }
    }

    close(write_fd);
    close(read_fd);
    return 0;
}