#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#define SOCKET_PATH "/tmp/kv_socket"
#define BUFFER_SIZE 256
#define MAX_KEYS 100

typedef struct {
    char key[50];
    char value[100];
} kv_pair;

kv_pair store[MAX_KEYS];
int store_count = 0;

// Mutex for thread-safe access to key-value store
pthread_mutex_t lock;

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(client_fd, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        char command[10], key[50], value[100];
        sscanf(buffer, "%s %s %100[^\n]", command, key, value);

        pthread_mutex_lock(&lock);
        if (strcmp(command, "SET") == 0) {
            int found = 0;
            for (int i = 0; i < store_count; i++) {
                if (strcmp(store[i].key, key) == 0) {
                    strcpy(store[i].value, value);
                    found = 1;
                    break;
                }
            }
            if (!found && store_count < MAX_KEYS) {
                strcpy(store[store_count].key, key);
                strcpy(store[store_count].value, value);
                store_count++;
            }
            write(client_fd, "OK\n", 3);
        } else if (strcmp(command, "GET") == 0) {
            int found = 0;
            for (int i = 0; i < store_count; i++) {
                if (strcmp(store[i].key, key) == 0) {
                    write(client_fd, store[i].value, strlen(store[i].value));
                    write(client_fd, "\n", 1);
                    found = 1;
                    break;
                }
            }
            if (!found) write(client_fd, "NOT_FOUND\n", 10);
        } else {
            write(client_fd, "ERROR\n", 6);
        }
        pthread_mutex_unlock(&lock);
    }

    close(client_fd);
    free(arg);
    return NULL;
}

int main() {
    int server_fd, *client_fd;
    struct sockaddr_un addr;
    
    pthread_mutex_init(&lock, NULL);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    unlink(SOCKET_PATH);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); exit(1); }
    if (listen(server_fd, 5) < 0) { perror("listen"); exit(1); }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);
        if (*client_fd < 0) { perror("accept"); free(client_fd); continue; }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd);
        pthread_detach(tid);
    }

    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}