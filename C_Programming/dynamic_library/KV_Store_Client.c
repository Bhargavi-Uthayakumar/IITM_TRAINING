#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "kv_store.h"

#define SOCKET_PATH "/tmp/uds-kv.sock"

// Function to connect to server
/*
int connect_to_server(const char *path) {
    int fd;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        exit(1);
    }

    return fd;
}

// Function to send a command to the server
int send_command(int fd, const char *cmd) {
    return write(fd, cmd, strlen(cmd));
}

// Function to receive response from the server
int receive_response(int fd, char *buf, size_t size) {
    return read(fd, buf, size);
}
*/
int main(void) {
    int fd;
    char buf[256];

    fd = connect_to_server(SOCKET_PATH);
    printf("[client] connected to server.\n");

    while (1) {
        printf("Enter command (SET key value / GET key / QUIT): ");
        if (!fgets(buf, sizeof(buf), stdin))
            break;

        buf[strcspn(buf, "\n")] = '\0'; // remove newline

        if (strcasecmp(buf, "QUIT") == 0)
            break;

        if (send_command(fd, buf) < 0) {
            perror("send");
            break;
        }

        memset(buf, 0, sizeof(buf));
        if (receive_response(fd, buf, sizeof(buf)) > 0)
            printf("Server: %s\n", buf);
        else
            printf("No response or error.\n");

        close(fd);
        fd = connect_to_server(SOCKET_PATH); // reconnect for next command
    }

    close(fd);
    return 0;
}