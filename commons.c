#include "commons.h"

int get_file_size(FILE * file_ptr)
{
    int file_size = 0;
    fseek(file_ptr, 0L, SEEK_END);
    file_size = ftell(file_ptr);
    rewind(file_ptr);

    return file_size;
}

void send_file_to_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "rb");
    char buff[MAX_LEN] = {};
    int file_size;

    if(file == NULL){
        printf("%s: %s\n", file_error ,filename);
        write(socket, file_error, strlen(file_error));
        return;
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
            exit(EXIT_FAILURE);
        }
        if (send(socket, buff, bytes_read, 0) < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        file_size -= bytes_read;
    }
    
    fclose(file);
    close(socket);
}

void recv_file_from_socket(const char * filename, int socket)
{
    FILE * file = fopen(filename, "wb");
    char buff[MAX_LEN] = {};

    if (file == NULL)
    {
        printf("%s: %s\n", file_error, filename);
        return;
    }

    while (read(socket, buff, MAX_LEN) > 0)
    {
        fwrite(buff, sizeof(char), strlen(buff), file);
        memset(buff, 0, sizeof(buff));
    }

    fclose(file);
}