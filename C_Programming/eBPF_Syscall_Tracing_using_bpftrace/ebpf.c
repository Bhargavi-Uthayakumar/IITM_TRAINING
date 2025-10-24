
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
     
    const char *filename = "sample.txt";

   
    int fd = open(filename, O_CREAT | O_WRONLY |O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    write(fd, "This is ebpf file\n", 50);
    close(fd);
    
    printf("PID: %d\n", getpid());
    
    for (int i = 0; i < 3; i++) {
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            perror("open");
            return 1;
        }

        char buffer[1024];
        read(fd, buffer, sizeof(buffer));
        close(fd);

        sleep(3); 
    }

    return 0;
}
