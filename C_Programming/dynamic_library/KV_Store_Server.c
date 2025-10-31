#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dlfcn.h>

#define SOCKET_PATH "/tmp/uds-kv.sock"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buf[256];

    // Step 1: Load the shared library dynamically
    void *handle = dlopen("./libkvstore.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(1);
    }

    // Step 2: Get function pointers from the library
    void (*set_kv)(const char*, const char*) = dlsym(handle, "set_kv");
    char* (*get_kv)(const char*) = dlsym(handle, "get_kv");
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error loading symbols: %s\n", error);
        dlclose(handle);
        exit(1);
    }

    // Step 3: Create a UNIX domain socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        dlclose(handle);
        exit(1);
    }

    unlink(SOCKET_PATH); // remove existing socket file
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Step 4: Bind and listen
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        dlclose(handle);
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        dlclose(handle);
        exit(1);
    }

    printf("[server] listening on %s...\n", SOCKET_PATH);

    // Step 5: Accept client connections
    while ((client_fd = accept(server_fd, NULL, NULL)) > 0) {
        memset(buf, 0, sizeof(buf));
        read(client_fd, buf, sizeof(buf));

        // Process client command
        char cmd[10], key[100], value[100];
        if (sscanf(buf, "%s %s %s", cmd, key, value) >= 2) {
            if (strcmp(cmd, "SET") == 0) {
                set_kv(key, value);
                write(client_fd, "OK\n", 3);
            } else if (strcmp(cmd, "GET") == 0) {
                char *val = get_kv(key);
                if (val)
                    write(client_fd, val, strlen(val));
                else
                    write(client_fd, "NOT FOUND\n", 10);
            } else {
                write(client_fd, "INVALID COMMAND\n", 16);
            }
        }
        close(client_fd);
    }

    // Step 6: Cleanup
    close(server_fd);
    unlink(SOCKET_PATH);
    dlclose(handle);

    return 0;
}