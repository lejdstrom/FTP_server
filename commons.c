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
    #ifdef DEBUG
    printf("in send_file_to_socket func\nfilename: %s\n", filename);
    #endif

    FILE * file = fopen(filename, "rb");
    char buff[MAX_LEN] = {};
    int file_size;

    if(file == NULL){
        printf(": %s\n",filename);
        write(socket, "file not found", strlen("file not found"));
        return -1;
    }

    // will help to display a progress bar
    file_size = get_file_size(file);
    int file_size_copy = file_size;

    #ifdef DEBUG
    printf("file size: %d\n", file_size);
    #endif

    // send empty buffer fix a bug
    // do not remove without testing !
    memset(buff, 0, MAX_LEN);
    send(socket, buff, MAX_LEN, 0);

    while (file_size > 0)
    {  
        int bytes_read = fread(buff, sizeof(char), MAX_LEN, file);
        printf("bytes_read: %d\n", bytes_read);
        
        // handle partial sent
        int bytes_sent = 0;
        int res = send(socket, buff, bytes_read, 0);
        memset(buff, 0, MAX_LEN);
        printf("sent: %d/%d\n", bytes_sent, file_size_copy);

        if (res < 0)
        {
            perror("send");
            fclose(file);
            return -1;
        }

        bytes_sent += res;
        file_size -= bytes_read;
    }

    printf("closing connection for: %d\n", socket);
    fclose(file);
    close(socket);
    return 0; // success
}

int recv_file_from_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "wb");
    char buff[MAX_LEN] = {};
    //const size_t MAX_READ_LEN = MAX_LEN -1;

    if (file == NULL)
    {
        printf("%s: %s\n", file_error, filename);
        return -1;
    }
    // test
    int a = recv(socket, buff, MAX_LEN, 0);
    int size = 0;
    sscanf(buff, "%d", &size);
    printf("\treceving size: %d\n", size);
    //
    ssize_t bytes_read, total_bytes = 0;
    do
    {
        memset(buff, 0, MAX_LEN);
        
        bytes_read = recv(socket,buff,MAX_LEN,0);/*read(socket, buff, MAX_LEN);*/
        
        printf("bytes read %d\n", bytes_read);

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