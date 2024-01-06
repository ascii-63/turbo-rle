#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <getopt.h>

#include "include/trle.h"

#define COMPRESSED_FILE "files/com_file"
#define COMPRESSED_FAILED_VALUE 1
#define PORT_OFFSET 10000
#define BUFFER_SIZE 1024
#define DEFAULT_FILE "files/file"

unsigned worker_receive_socket, master_send_socket;
struct sockaddr_in worker_receive_address, master_send_address;
unsigned worker_receive_port;

unsigned master_receive_port;
char *master_receive_host;
unsigned worker_send_socket;
struct sockaddr_in master_receive_address;

void sendBack(unsigned len)
{
    fprintf(stderr, "b");
    master_receive_port = worker_receive_port + 10000;
    fprintf(stderr, "b");
    if ((worker_send_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating this socket");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "b");
    master_receive_address.sin_family = AF_INET;
    master_receive_address.sin_addr.s_addr = inet_addr(master_receive_host);
    master_receive_address.sin_port = htons(master_receive_port);
    fprintf(stderr, "b");
    if (connect(worker_send_socket, (struct sockaddr *)&master_receive_address, sizeof(master_receive_address)) == -1)
    {
        fprintf(stderr, "Error connecting to worker");
        close(worker_send_socket);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "b");
    /////////////////////////////////

    FILE *file_in = fopen(COMPRESSED_FILE, "rb");
    if (!file_in)
    {
        fprintf(stderr, "Error opening file");
        close(worker_send_socket);
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(len);
    if (!buffer)
    {
        fprintf(stderr, "Malloc error\n");
        fclose(file_in);
        close(worker_send_socket);
        exit(EXIT_FAILURE);
    }

    /////////////////////////////////

    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, len, file_in)) > 0)
        send(worker_send_socket, buffer, bytes_read, 0);

    /////////////////////////////////

    free(buffer);
    fclose(file_in);
    close(worker_send_socket);
}

unsigned compress(unsigned char *in, unsigned n, unsigned char *out)
{
    unsigned len = trlec(in, n, out);
    fprintf(stderr, "a0");
    FILE *fout = fopen(COMPRESSED_FILE, "wb");
    if (!fout)
    {
        fprintf(stderr, "Error creating file.");
        return COMPRESSED_FAILED_VALUE;
    }

    fwrite(out, 1, len, fout);
    fclose(fout);
    fprintf(stderr, "a1");

    return len;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "%s <worker-port> <master-host>", argv[0]);
        exit(EXIT_FAILURE);
    }

    worker_receive_port = atoi(argv[1]);
    master_receive_host = strdup(argv[2]);
    if (!master_receive_host)
    {
        fprintf(stderr, "Malloc error\n");
        exit(EXIT_FAILURE);
    }

    /////////////////////////////////

    socklen_t addr_size = sizeof(struct sockaddr_in);

    if ((worker_receive_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating socket");
        exit(EXIT_FAILURE);
    }

    worker_receive_address.sin_family = AF_INET;
    worker_receive_address.sin_addr.s_addr = INADDR_ANY;
    worker_receive_address.sin_port = htons(worker_receive_port);

    if (bind(worker_receive_socket, (struct sockaddr *)&worker_receive_address, sizeof(worker_receive_address)) == -1)
    {
        fprintf(stderr, "Error binding socket");
        goto error_handle;
    }

    /////////////////////////////////

    if (listen(worker_receive_socket, 10) == -1)
    {
        fprintf(stderr, "Error listening for connections");
        goto error_handle;
    }

    if ((master_send_socket = accept(worker_receive_socket, (struct sockaddr *)&master_send_address, &addr_size)) == -1)
    {
        fprintf(stderr, "Error accepting connection");
        goto error_handle;
    }

    /////////////////////////////////

    FILE *file = fopen(DEFAULT_FILE, "wb");
    if (!file)
    {
        fprintf(stderr, "Error creating file.");
        close(master_send_socket);
        goto error_handle;
    }

    char *buffer = malloc(BUFFER_SIZE);
    ssize_t bytes_read;

    while ((bytes_read = recv(master_send_socket, buffer, BUFFER_SIZE, 0)) > 0)
        fwrite(buffer, 1, bytes_read, file);

    /////////////////////////////////

    free(buffer);
    fclose(file);
    close(worker_receive_socket);
    close(master_send_socket);

    /*********************************************/

    unsigned char *in, *out;
    char *inname = (char *)malloc(strlen(DEFAULT_FILE) + 1);
    strcpy(inname, DEFAULT_FILE);
    long long flen;
    FILE *fi = fopen(inname, "rb");
    if (!fi)
        perror(inname);
    fseek(fi, 0, SEEK_END);
    flen = ftell(fi);
    fseek(fi, 0, SEEK_SET);

    if (!(in = (unsigned char *)malloc(flen + 1024)))
    {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    if (!(out = (unsigned char *)malloc(flen * 4 / 3 + 1024)))
    {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }

    flen = fread(in, 1, flen, fi);
    fclose(fi);
    if (flen <= 0)
        exit(0);

    fprintf(stderr, "a-1");
    unsigned com_file_len = compress(in, flen, out);

    /*********************************************/
    fprintf(stderr, "a");
    sendBack(com_file_len);

    return EXIT_SUCCESS;

error_handle:
    close(worker_receive_socket);
    exit(EXIT_FAILURE);
}