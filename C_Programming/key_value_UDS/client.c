#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/uds-demo.sock" 
#define BUF_SIZE 1024                   

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static ssize_t read_all_or_until_block(int fd, char *buf, size_t cap)
{
    ssize_t total = 0;

    for (;;)
    {
        ssize_t r = read(fd, buf + total, cap - (size_t)total);

        if (r > 0)
        {
            total += r;
            if ((size_t)total == cap)
                return total; 

            if (r < 128)
                return total;
        }
        else if (r == 0)
        {
           return total;
        }
        else
        {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return total;

            return -1;
        }
    }
}

int main(void)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
        die("socket(AF_UNIX)");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("connect");

    while(1)
    {
          char buffer[256];
          printf("> ");
          fgets(buffer, sizeof(buffer), stdin);
          if (strncmp(buffer, "exit", 4) == 0)
               break;
          if (write(fd, buffer, strlen(buffer)) < 0)
               die("write");

          ssize_t n = read_all_or_until_block(fd, buffer, BUF_SIZE);
          if (n < 0)
               die("read(reply)");
          buffer[n] = '\0';
          fprintf(stderr, "[client] server reply: %s\n", buffer);
    }
    close(fd);
    return 0;
}