#include "commons.h"

// we assume file_ptr not null
int get_file_size(FILE *file_ptr)
{
    int file_size = 0;
    fseek(file_ptr, 0L, SEEK_END);
    file_size = ftell(file_ptr);
    rewind(file_ptr);

    return file_size;
}

// if value < 0 return -1 == error and write error message
// else return 0 == success
int check(int value_to_test, const char * error_message)
{
    if(value_to_test < 0){
        perror(error_message);
        return -1;
    }
    return 0;
}

// send filename (if exist) to socket
int send_file_to_socket(const char *filename, int socket)
{
#ifdef DEBUG
    printf("in send_file_to_socket func\nfilename: %s\n", filename);
#endif

    FILE *file = fopen(filename, "rb");
    char buff[MAX_LEN] = {};
    int file_size;

    if (file == NULL)
    {
        printf(": %s\n", filename);
        write(socket, "file not found", strlen("file not found"));
        return -1;
    }

    file_size = get_file_size(file);
    int file_size_copy = file_size;

#ifdef DEBUG
    printf("file size: %d\n", file_size);
#endif

    // send empty buffer fix a bug
    // do not remove without testing !
    memset(buff, 0, MAX_LEN);
    send(socket, buff, MAX_LEN, 0);

    int bytes_sent = 0;

    while (file_size > 0)
    {
        int bytes_read = fread(buff, sizeof(char), MAX_LEN, file);

#ifdef DEBUG
        printf("bytes_read: %d\n", bytes_read);
#endif

        int res = send(socket, buff, bytes_read, 0);
        bytes_sent += res;
        memset(buff, 0, MAX_LEN);
        printf("sent: %d/%d\n", bytes_sent, file_size_copy);

        if (res < 0)
        {
            perror("send");
            fclose(file);
            return -1;
        }
        file_size -= bytes_read;
    }

    printf("closing connection for: %d\n", socket);
    fclose(file);
    close(socket);
    return 0; // success
}

// create a local file named filename_copy
// and write data from socket into
int recv_file_from_socket(const char *filename, int socket)
{
    FILE *file = fopen(filename, "wb");
    char buff[MAX_LEN] = {};

    if (file == NULL)
    {
        printf("%s: %s\n", file_error, filename);
        return -1;
    }

    int bytes_read, total_bytes = 0;
    do
    {
        memset(buff, 0, MAX_LEN);
        bytes_read = recv(socket, buff, MAX_LEN, 0);

        printf("bytes read %d\n", bytes_read);

        if (bytes_read < 0)
        {
            perror("error reading from socket");
            fclose(file);
            return -1;
        }

        fwrite(buff, sizeof(char), bytes_read, file);

    } while (bytes_read > 0);

    printf("file downloaded\n");
    /*
    sleep(2);
    system("clear");
    */
    fclose(file);
    return 0;
}

FTP_COMMANDS parse_ftp_cmd(const char * client_message)
{
    if (!strncmp(client_message, "ls", 2))
    {
        return LIST;
    }
    else if (!strncmp(client_message, "dl", 2))
    {
        return DOWNLOAD;
    }
    else if (!strncmp(client_message, "up", 2))
    {
        return UPLOAD;
    }
    else if (!strncmp(client_message, "cd", 2))
    {
        return CD;
    }
    else if (!strncmp(client_message, "mkdir", 5))
    {
        return MKDIR;
    }
    else if(!strncmp(client_message, "exit", 5))
    {
        return EXIT;
    }
    else
    {
        return UNKNOW;
    }
}