#include "commons.h"

void handle_connection(int client_socket)
{
    printf("connection accepted - id: %d\n", client_socket);

    char buffer[MAX_LEN];
    int n;
    int new_socket = client_socket;

    n = recv(new_socket, buffer, MAX_LEN, 0);

    if (n < 0)
    {
        perror("server error receiving data");
        goto exit;
    }

    // null terminate the message
    buffer[n] = 0;
    printf("server recv: %s\n", buffer);

    // client want us to send him a file
    if(send_file_to_socket(buffer, new_socket) < 0)
    {
        goto exit;
    }

exit:
    #ifdef DEBUG
    printf("closing socket %d\nin server.c\n", new_socket);
    #endif
    close(new_socket);
    return;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int sockfd;
    struct sockaddr_in servaddr = {}, cliaddr = {};
    socklen_t len = sizeof(cliaddr);

    /* Create a socket */
    check(sockfd = socket(AF_INET, SOCK_STREAM, 0), "Warning creating socket");

    /* Bind the socket to a specific port */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    check(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0, "Warning binding socket");
    check(listen(sockfd, 1) < 0, "Warning listenning");

    while (true)
    {
        printf("waiting for connections ...\n");
        int new_sock = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t*)&len);
        if (new_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        printf("connected\n");

        handle_connection(new_sock);
    }
    return 0;
}