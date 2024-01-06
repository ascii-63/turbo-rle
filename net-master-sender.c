#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>

#define FILES_PATH "files/"

int worker_port;
char *worker_host;
int this_socket;
struct sockaddr_in worker_address;
unsigned long file_size;

char *extention;
int ext_len;

#define _checkpoint(_idx_) fprintf(stdout, "CHECKPOINT #%d\n", _idx_);
#define error_handle    \
    free(worker_host);  \
    free(extention);    \
    close(this_socket); \
    exit(EXIT_FAILURE);

char *findPartFile()
{
    for (;;)
    {
        DIR *dir = opendir(FILES_PATH);

        if (dir == NULL)
        {
            perror("Error opening directory");
            exit(EXIT_FAILURE);
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            char *file_name = entry->d_name;

            size_t len = strlen(file_name);
            if (len >= ext_len && strcmp(file_name + len - ext_len, extention) == 0)
            {
                closedir(dir);
                return file_name;
            }
        }
        closedir(dir);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "%s <worker-host> <worker-port> <file-extention>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    worker_host = strdup(argv[1]);
    if (!worker_host)
    {
        fprintf(stderr, "Malloc error\n");
        exit(EXIT_FAILURE);
    }

    worker_port = atoi(argv[2]);

    extention = strdup(argv[3]);
    if (!extention)
    {
        fprintf(stderr, "Malloc error\n");
        exit(EXIT_FAILURE);
    }

    ext_len = strlen(extention);

    /////////////////////////////////

    if ((this_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error creating this socket");
        free(worker_host);
        free(extention);
        exit(EXIT_FAILURE);
    }

    worker_address.sin_family = AF_INET;
    worker_address.sin_addr.s_addr = inet_addr(worker_host);
    worker_address.sin_port = htons(worker_port);

    /////////////////////////////////

    if (connect(this_socket, (struct sockaddr *)&worker_address, sizeof(worker_address)) == -1)
    {
        fprintf(stderr, "Error connecting to worker");
        error_handle;
    }

    /////////////////////////////////

    char *file = findPartFile();
    if (!file)
    {
        fprintf(stderr, "Failed to find .part file");
        error_handle;
    }
    char *file_path = (char *)malloc(strlen(file) + strlen(FILES_PATH) + 2);
    strcpy(file_path, FILES_PATH);
    strcat(file_path, file);
    // free(file);

    /////////////////////////////////

    FILE *file_in = fopen(file_path, "rb");
    if (!file_in)
    {
        fprintf(stderr, "Error opening file");
        free(file_path);
        error_handle;
    }

    fseek(file_in, 0, SEEK_END);
    file_size = ftell(file_in);
    fseek(file_in, 0, SEEK_SET);
    if (file_size <= 0)
    {
        fprintf(stderr, "File size error");
        free(file_path);
        error_handle;
    }

    char *buffer = (char *)malloc(file_size);
    if (!buffer)
    {
        fprintf(stderr, "Malloc error\n");
        fclose(file_in);
        free(file_path);
        error_handle;
    }

    /////////////////////////////////

    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, file_size, file_in)) > 0)
        send(this_socket, buffer, bytes_read, 0);

    /////////////////////////////////

    free(buffer);
    free(extention);
    free(worker_host);
    fclose(file_in);
    close(this_socket);

    /////////////////////////////////

    return EXIT_SUCCESS;
}