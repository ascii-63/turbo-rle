#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 12345
#define BUFFER_SIZE 1024
#define DEFAULT_FILE "file"

unsigned server_socket, client_socket;
struct sockaddr_in server_address, client_address;
unsigned server_port;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Running server with default port: %d\n", DEFAULT_PORT);
        server_port = DEFAULT_PORT;
    }
    else
        server_port = atoi(argv[1]);

    /////////////////////////////////

    socklen_t addr_size = sizeof(struct sockaddr_in);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        fprintf(stderr, "Error binding socket");
        goto error_handle;
    }

    /////////////////////////////////

    if (listen(server_socket, 5) == -1)
    {
        fprintf(stderr, "Error listening for connections");
        goto error_handle;
    }
    printf("Server listening on port %d...\n", server_port);

    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addr_size)) == -1)
    {
        fprintf(stderr, "Error accepting connection");
        goto error_handle;
    }
    printf("Connection accepted from %s:%d\n",
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    /////////////////////////////////

    FILE *file = fopen(DEFAULT_FILE, "wb");
    if (!file)
    {
        fprintf(stderr, "Error creating file.");
        close(client_socket);
        goto error_handle;
    }

    char *buffer = malloc(BUFFER_SIZE);
    ssize_t bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
        fwrite(buffer, 1, bytes_read, file);

    printf("File received successfully.\n");

    /////////////////////////////////

    free(buffer);
    fclose(file);
    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;

error_handle:
    close(server_socket);
    exit(EXIT_FAILURE);
}
