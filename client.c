#include "commons.h"

void display_client_menu()
{
    puts("\nWelcome to ftp client\n");
    puts("Call one of the following functions:");

    printf("dl <filename>\t\tDownload file\n");
    printf("up <filename\t\tUpload file\n");
    printf("ls\t\t\tList files in server\n");
    printf("exit\t\t\tClose the client\n");
}

void download_file(const char * downloade_file_name)
{

}





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
        check(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)), "Warning connecting");

        display_client_menu();
        // connect is a fd to server
        // test download logic
        puts("\nplease enter a filename you want to dl");
        printf(">>> ");
        char buffer[MAX_LEN];

        // get filename
        fgets(buffer, MAX_LEN, stdin);
        // remove new line
        buffer[strcspn(buffer, "\n")] = 0;


        switch (parse_ftp_cmd(buffer))
        {
        case DOWNLOAD:
            // call dl
            break;
        case UPLOAD:
            // call up
            break;

        case LIST:
            // call ls
            break;

        case MKDIR:
            // call mkdir
            break;

        case CD:
            //call cd
            break;

        case EXIT:
            goto exit;
            break;

        case UNKNOW:
            break;

        default:
            break;
        }

        // send filename
        check(send(sockfd, buffer, MAX_LEN, 0), "Client error sending data");

        char buftmp[MAX_LEN];
        int n = recv(sockfd, buftmp, MAX_LEN, 0);
        buftmp[n] = 0;

        #ifdef DEBUG
        printf("\nbuf tmp: %s\n", buftmp);
        #endif
        
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

exit:
    puts("Exiting...");

    return 0;
}