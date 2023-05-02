#include "commons.h"

int get_file_size(FILE * file_ptr)
{
    int file_size = 0;
    fseek(file_ptr, 0L, SEEK_END);
    file_size = ftell(file_ptr);
    rewind(file_ptr);

    return file_size;
}


/**
 * 
 * client: upload <filename>                    server: getfilename
 * client: send_file_to_socket(server_sock)     server: recv_file_from_socket()
*/
int send_file_to_socket(const char * filename, int socket)
{
    printf("in send_file_to_socket func\nfilename: %s\n", filename);

    FILE * file = fopen(filename, "rb");
    char buff[MAX_LEN] = {};
    int file_size;

    if(file == NULL){
        printf(": %s\n",filename);
        //write(socket, file_error, strlen(file_error));
        return -1;
    }


    // will help to display a progress bar
    file_size = get_file_size(file);

    printf("file size: %d\n", file_size);

    while (file_size > 0)
    {
        size_t bytes_to_read = (file_size < MAX_LEN) ? file_size : MAX_LEN;
        size_t bytes_read = fread(buff, sizeof(char), bytes_to_read, file);

        if (bytes_read < bytes_to_read)
        {
            perror("file read");
            fclose(file);
            return -1;
        }

        // handle partial sent
        size_t bytes_sent = 0;
        while (bytes_sent < bytes_read)
        {
            size_t res = send(socket, buff + bytes_sent, bytes_read - bytes_sent, 0);
            
            if (res < 0)
            {
                perror("send");
                fclose(file);
                return -1;
            }
            bytes_sent += res;
        }

        file_size -= bytes_read;
    }
/*
   while (true)
   {
        char buff[MAX_LEN] = {};
        int read = fread(buff, sizeof(char), MAX_LEN, file);

        if(read > 0)
        {
            printf("sending...\n");
            write(socket, buff, MAX_LEN);
        }

        if(read < MAX_LEN)
        {
            if(feof(file)){
                printf("transfer complete to: %d\n", socket);
                break;
            }
            if(ferror(file)){
                printf("error with file\n");
                break;
            }
        }
        
   }
   */
   
    printf("closing connection for: %d\n", socket);
    fclose(file);
    close(socket);
    return 0; // success
}

int recv_file_from_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "ab");
    
    const size_t MAX_READ_LEN = MAX_LEN -1;

    if (file == NULL)
    {
        printf("%s: %s\n", file_error, filename);
        return -1;
    }

    printf("receving file...\n");

    size_t bytes_read;
    do
    {
        char buff[MAX_LEN] = {};
        bytes_read = read(socket, buff, MAX_READ_LEN);
        
        if(bytes_read < 0){
            perror("error reading from socket");
            fclose(file);
            return -1;
        }

        fwrite(buff, sizeof(char), bytes_read, file);

    } while (bytes_read > 0);

    printf("file downloaded\n");
    
    fclose(file);
    return 0;
}