#include "kv_store.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include<stddef.h>

/* -------- Key-Value Store -------- */
typedef struct {
    char key[MAX_CHAR];
    char value[MAX_CHAR];
} kv;

static kv kv_pair[MAX_SIZE];
static int count = 0;
/*-------Server side----------------------*/
void set_kv(const char *key, const char *value)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(kv_pair[i].key, key) == 0) {
            strncpy(kv_pair[i].value, value, MAX_CHAR - 1);
            kv_pair[i].value[MAX_CHAR - 1] = '\0';
            return;
        }
    }

    if (count < MAX_SIZE) {
        strncpy(kv_pair[count].key, key, MAX_CHAR - 1);
        kv_pair[count].key[MAX_CHAR - 1] = '\0';
        strncpy(kv_pair[count].value, value, MAX_CHAR - 1);
        kv_pair[count].value[MAX_CHAR - 1] = '\0';
        count++;
    }
}

char *get_kv(const char *key)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(kv_pair[i].key, key) == 0)
            return kv_pair[i].value;
    }
    return NULL;
}

/* -------- Client Functions -------- */
/*
static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int connect_to_server(const char *socket_path)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        die("socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("connect");

    return fd;
}

ssize_t send_command(int fd, const char *cmd)
{
    return write(fd, cmd, strlen(cmd));
}

ssize_t receive_response(int fd, char *buf, size_t size)
{
    ssize_t n = read(fd, buf, size - 1);
    if (n > 0)
        buf[n] = '\0';
    return n;
}
*/

// Function to connect to server
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