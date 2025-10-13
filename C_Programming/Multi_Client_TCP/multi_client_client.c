#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to chat server.\nType messages below (Ctrl+C to exit):\n");

    fd_set readfds;
    int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                send(sock, buffer, strlen(buffer), 0);
            }
        }

        if (FD_ISSET(sock, &readfds))
        {
            memset(buffer, 0, sizeof(buffer));
            int valread = recv(sock, buffer, sizeof(buffer), 0);
            if (valread <= 0)
            {
                printf("Server disconnected.\n");
                break;
            }
            printf("Message: %s", buffer);
        }
    }

    close(sock);
    return 0;
}