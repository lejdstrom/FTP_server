// regroup commons include between server and client
// to get a cleaner code

#ifndef COMMONS_H
#define COMMONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <dirent.h>

void send_file_to_socket(const char * filename, int socket);

void recv_file_from_socket(const char * filename, int socket);


#endif