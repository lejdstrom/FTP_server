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
        check(sockfd = socket(AF_INET, SOCK_STREAM, 0), "Warning creating socket");
        
        /* Connect to server */
        check(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0, "Warning connecting");

        // connect is a fd to server
        // test download logic
        puts("please enter a filename you want to dl");
        char buffer[MAX_LEN];

        // get filename
        fgets(buffer, MAX_LEN, stdin);
        // remove new line
        buffer[strcspn(buffer, "\n")] = 0;

        if (!strncmp(buffer, "exit", 4))
        {
            puts("Exiting...");
            break;
        }

        // send filename
        check(send(sockfd, buffer, MAX_LEN, 0) < 0, "Client error sending data");

        char buftmp[MAX_LEN];
        int n = recv(sockfd, buftmp, MAX_LEN, 0);
        buftmp[n] = 0;

        if(!strcmp(buftmp, "file not found")){
            printf("file not found\n");
            continue;
        }

        // we add _copy to filename
        char copy[MAX_LEN];
        strcpy(copy, buffer);
        strcat(copy, "_copy");

        // we are now waiting to get the file from server
        if(recv_file_from_socket(copy, sockfd) < 0)
        {
            puts("problem while dl...");
            break;
        }
    }
    return 0;
}