#define _GNU_SOURCE 
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/uds-demo.sock" 
#define BACKLOG 10                       
#define BUF_SIZE 1024                    
#define MAX_KV_STORE 100

static int listen_fd = -1;

typedef struct{
     char key[100];
     char value[200];
}KV_PAIR;

KV_PAIR hashMap[MAX_KV_STORE];
int hmsize = 0;

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static void cleanup(void)
{
    if (listen_fd != -1)
        close(listen_fd);
    unlink(SOCKET_PATH);
}

static void on_signal(int sig)
{
    (void)sig; 
    exit(0);
}

static ssize_t read_line(int fd, char *buf, size_t maxlen)
{
    size_t n = 0;
    while (n + 1 < maxlen)
    {
        char c;
        ssize_t r = read(fd, &c, 1);
        if (r == 0)
            break; 
        if (r < 0)
        {
            if (errno == EINTR)
                continue; 
            return -1;    
        }

        buf[n++] = c;
        if (c == '\n')
            break;
    }
    buf[n] = '\0'; 
    return (ssize_t)n;
}

/
static int write_all(int fd, const void *buf, size_t len)
{
    const char *p = (const char *)buf;
    while (len > 0)
    {
        ssize_t w = write(fd, p, len);
        if (w < 0)
        {
            if (errno == EINTR)
                continue; 
            return -1;    
        }
        p += w;
        len -= (size_t)w;
    }
    return 0;
}

static void print_peer_credentials(int client_fd)
{
#ifdef __linux__
    struct ucred cred;
    socklen_t len = sizeof(cred);

    if (getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0)
    {
        fprintf(stderr, "[server] peer pid=%d uid=%d gid=%d\n", cred.pid, cred.uid, cred.gid);
    }
    else
    {
        perror("getsockopt(SO_PEERCRED)");
    }
#else
    (void)client_fd; 
    fprintf(stderr, "[server] peer credential fetch not implemented on this OS\n");
#endif
}

static void set_value(const char* key, const char *value)
{
     for(int i=0; i<hmsize; i++)
     {
          if(strcmp(hashMap[i].key, key)==0)
          {
               strncpy(hashMap[i].value, value, sizeof(value)-1);
               printf("value : %s is stored\n", value);
               return;
          }
     }
     if(hmsize<MAX_KV_STORE)
     {
         strncpy(hashMap[hmsize].key , key, sizeof(key)-1);
         strncpy(hashMap[hmsize].value, value, sizeof(value)-1);
         printf("value : %s is stored\n", value);
         hmsize++;
     }
     else
      printf("key - value store is full..\n");
}

static char* get_value(const char* key)
{
     for(int i=0; i<hmsize; i++)
     {
          if(strcmp(hashMap[i].key, key)==0)
          {
               return hashMap[i].value;
          }
     }
     return NULL;
}

int main(void)
{
    atexit(cleanup);

    struct sigaction sa = {0};
    sa.sa_handler = on_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    umask(077);

    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1)
        die("socket(AF_UNIX)");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("bind");

    if (chmod(SOCKET_PATH, 0600) == -1)
        die("chmod(socket)");

    if (listen(listen_fd, BACKLOG) == -1)
        die("listen");

    fprintf(stderr, "[server] listening on %s\n", SOCKET_PATH);

    for (;;)
    {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) { if (errno == EINTR) continue; die("accept"); }

        char buf[BUF_SIZE];
        ssize_t n;
        while ((n = read_line(client_fd, buf, sizeof(buf))) > 0) {
            char cmd[16], key[256], value[256];
            cmd[0] = key[0] = value[0] = 0;

            if (sscanf(buf, "%15s %255s %255s", cmd, key, value) >= 2) {
                if (strcmp(cmd, "SET") == 0) {
                    set_value(key, value);
                    write_all(client_fd, "OK\n", 3);
                } else if (strcmp(cmd, "GET") == 0) {
                    const char *val = get_value(key);
                    if (val)
                        write_all(client_fd, val, strlen(val)), write_all(client_fd, "\n", 1);
                    else
                        write_all(client_fd, "NOT_FOUND\n", 10);
                } else {
                    write_all(client_fd, "ERROR: Unknown command\n", 23);
                }
            } else {
                write_all(client_fd, "ERROR: Invalid format\n", 23);
            }
        }

        close(client_fd);
    }
    close(listen_fd);
    unlink(SOCKET_PATH);
    return 0;
}