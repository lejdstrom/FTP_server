#include "commons.h"

int get_file_size(FILE * file_ptr)
{
    int file_size = 0;
    fseek(file_ptr, 0L, SEEK_END);
    file_size = ftell(file_ptr);
    rewind(file_ptr);

    return file_size;
}

int send_file_to_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "rb");
    char buff[MAX_LEN] = {};
    int file_size;

    if(file == NULL){
        printf("%s: %s\n", file_error ,filename);
        write(socket, file_error, strlen(file_error));
        return -1;
    }

    // will help to display a progress bar
    file_size = get_file_size(file);

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
    
    fclose(file);
    close(socket);
    return 0; // success
}

int recv_file_from_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "wb");
    char buff[MAX_LEN] = {};
    const size_t MAX_READ_LEN = MAX_LEN -1;

    if (file == NULL)
    {
        printf("%s: %s\n", file_error, filename);
        return -1;
    }

    size_t bytes_read;
    do
    {
        bytes_read = read(socket, buff, MAX_READ_LEN);
        
        if(bytes_read < 0){
            perror("error reading from socket");
            fclose(file);
        }

        fwrite(buff, sizeof(char), bytes_read, file);
        memset(buff, 0, MAX_LEN);

    } while (bytes_read > 0);
    
    fclose(file);
    return 0;
}