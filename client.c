#include "commons.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in servaddr = {};

    /* Connect to the server */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(atoi(argv[1]));

    while (true)
    {
           /* Create a socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            perror("Warning creating socket");
            return 1;
        }

        /* Connect to server */
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("Warning connecting");
            return 1;
        }

        // connect is a fd to server
        // test download logic
        puts("please enter a filename you want to dl");
        char buffer[MAX_LEN];

        // get filename
        fgets(buffer, MAX_LEN - 1, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (!strncmp(buffer, "exit", 4))
        {
            puts("Exiting...");
            break;
        }
        // send filename
        if (send(sockfd, buffer, MAX_LEN, 0) < 0)
        {
            perror("Client error sending data");
            return 1;
        }

        //debug
        char copy[MAX_LEN];
        printf("buffer: %s\n", buffer);
        strcpy(copy, buffer);
        strcat(copy, "_copy");

        printf("copy: %s\n", copy);

        // we are now waiting to get the file from server
        if(recv_file_from_socket(copy, sockfd) < 0)
        {
            puts("problem while dl...");
            break;
        }

    }

    return 0;
}