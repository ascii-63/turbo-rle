#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define error_handle              \
    close(master_receive_socket); \
    exit(EXIT_FAILURE);

unsigned master_receive_socket, worker_send_socket;
struct sockaddr_in master_receive_address, worker_send_address;
unsigned master_receive_port;

char *file_path;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "%s <master_receive_port> <file-name>", argv[0]);
        exit(EXIT_FAILURE);
    }

    master_receive_port = atoi(argv[1]);
    file_path = strdup(argv[2]);
    if (!file_path)
    {
        fprintf(stderr, "Malloc error\n");
        exit(EXIT_FAILURE);
    }

    /////////////////////////////////
    
    socklen_t addr_size = sizeof(struct sockaddr_in);

    if ((master_receive_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    master_receive_address.sin_family = AF_INET;
    master_receive_address.sin_addr.s_addr = INADDR_ANY;
    master_receive_address.sin_port = htons(master_receive_port);
    
    if (bind(master_receive_socket, (struct sockaddr *)&master_receive_address, sizeof(master_receive_address)) == -1)
    {
        fprintf(stderr, "Error binding socket");
        error_handle;
    }
    
    /////////////////////////////////

    if (listen(master_receive_socket, 5) == -1)
    {
        fprintf(stderr, "Error listening for connections");
        error_handle;
    }
    
    if ((worker_send_socket = accept(master_receive_socket, (struct sockaddr *)&worker_send_address, &addr_size)) == -1)
    {
        fprintf(stderr, "Error accepting connection");
        error_handle;
    }
    
    /////////////////////////////////

    FILE *file = fopen(file_path, "wb");
    if (!file)
    {
        fprintf(stderr, "Error creating file.");
        close(worker_send_socket);
        error_handle;
    }

    char *buffer = (char *)malloc(BUFFER_SIZE);
    ssize_t bytes_read;

    while ((bytes_read = recv(worker_send_socket, buffer, BUFFER_SIZE, 0)) > 0)
        fwrite(buffer, 1, bytes_read, file);

    /////////////////////////////////

    free(buffer);
    fclose(file);
    close(master_receive_socket);
    close(worker_send_socket);

    return EXIT_SUCCESS;
}
