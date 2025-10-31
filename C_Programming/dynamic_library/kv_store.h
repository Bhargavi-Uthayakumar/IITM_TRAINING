#ifndef KVSTORE_H
#define KVSTORE_H
#include <stddef.h>
#include <sys/types.h> 
#define SOCKET_PATH "/tmp/uds-kv.sock"
#define BUF_SIZE 1024
#define MAX_CHAR 128
#define MAX_SIZE 20

/* ---------- Server side ---------- */
void set_kv(const char *key, const char *value);
char *get_kv(const char *key);

/* ---------- Client side ---------- */
int connect_to_server(const char *socket_path);
int send_command(int fd, const char *cmd);
int receive_response(int fd, char *buf, size_t size);

#endif