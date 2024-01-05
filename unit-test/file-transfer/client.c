#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int server_port;
char *server_host;
char *file_path;

int client_socket;
struct sockaddr_in server_address;
unsigned long file_size;

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "%s <server-host> <server-port> <path-to-file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_host = strdup(argv[1]);
    if (!server_host)
    {
        fprintf(stderr, "Malloc error\n");
        exit(EXIT_FAILURE);
    }

    server_port = atoi(argv[2]);

    file_path = strdup(argv[3]);
    if (!file_path)
    {
        fprintf(stderr, "Malloc error\n");
        free(server_host);
        exit(EXIT_FAILURE);
    }

    /////////////////////////////////

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating socket");
        free(server_host);
        free(file_path);
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_host);
    server_address.sin_port = htons(server_port);

    /////////////////////////////////

    printf("Connecting to server...\n");
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        fprintf(stderr, "Error connecting to server");
        goto error_handle;
    }
    printf("Connected to server.\n");

    /////////////////////////////////

    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        fprintf(stderr, "Error opening file");
        goto error_handle;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (file_size <= 0)
    {
        fprintf(stderr, "File size error");
        goto error_handle;
    }

    char *buffer = malloc(file_size);
    if (!buffer)
    {
        fprintf(stderr, "Malloc error\n");
        fclose(file);
        goto error_handle;
    }
    printf("File opened successfully.\n");

    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, file_size, file)) > 0)
        send(client_socket, buffer, bytes_read, 0);

    printf("File sent successfully.\n");

    /////////////////////////////////

    fclose(file);
    free(buffer);
    free(server_host);
    free(file_path);
    close(client_socket);

    return EXIT_SUCCESS;

error_handle:
    free(server_host);
    free(file_path);
    close(client_socket);
    exit(EXIT_FAILURE);
}
