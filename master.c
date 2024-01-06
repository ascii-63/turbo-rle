#pragma GCC optimize("O3")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <dirent.h>

#ifndef NUM_WORKERS
#define NUM_WORKERS 3
#endif

#define PID_FAIL_VALUE -1
#define PID_CHILD_VALUE 0
#define SYSTEM_CALL_FAIL_VALUE -1

#define COMPRESSION_EXTENTION ".rle"
#define SPACE_STRING " "
#define FILES_DIR "files/"
#define TEMP_FILES_DIR "files/temp/"

#define _display(_cmd_) fprintf(stdout, "%s\n", _cmd_);
#define _checkpoint(_idx_) fprintf(stdout, "CHECKPOINT #%d\n", _idx_);

//_______________________OPTIMIZE_______________________//

#define ALWAYS_INLINE inline __attribute__((always_inline))
typedef uint8_t status;

status ALWAYS_INLINE split(char *__restrict _fin)
{
#define SPLIT_COMMAND "/bin/bash ./bash/split.sh "
    uint8_t l1 = strlen(SPLIT_COMMAND);
    uint8_t l2 = strlen(_fin);

    char *cmd = (char *)malloc(l1 + l2 + 1);
    if (!cmd)
    {
        fprintf(stderr, "[ERROR]: Split failed: malloc error");
        return EXIT_FAILURE;
    }
    strcpy(cmd, SPLIT_COMMAND);
    strcat(cmd, _fin);

    if (system(cmd) == SYSTEM_CALL_FAIL_VALUE)
    {
        fprintf(stderr, "[ERROR]: Split failed: system call error");
        _display(cmd);
        free(cmd);
        return EXIT_FAILURE;
    }

    free(cmd);
    return EXIT_SUCCESS;
}

status ALWAYS_INLINE cleanUp(char *__restrict _dir)
{
#define CLEAN_COMMAND "/bin/bash ./bash/clean.sh "
    uint8_t l1 = strlen(CLEAN_COMMAND);
    uint8_t l2 = strlen(_dir);

    char *cmd = (char *)malloc(l1 + l2 + 1);
    if (!cmd)
    {
        fprintf(stderr, "[ERROR]: Split failed: malloc error");
        return EXIT_FAILURE;
    }
    strcpy(cmd, CLEAN_COMMAND);
    strcat(cmd, _dir);

    if (system(cmd) == SYSTEM_CALL_FAIL_VALUE)
    {
        fprintf(stderr, "[ERROR]: Clean failed: system call error");
        _display(cmd);
        free(cmd);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

status ALWAYS_INLINE merge(char *__restrict _dir, char *__restrict _fout)
{
#define MERGE_COMMAND "/bin/bash ./bash/merge.sh "
    uint8_t l1 = strlen(MERGE_COMMAND);
    uint8_t l2 = strlen(_dir);
    uint8_t l3 = strlen(_fout) + strlen(COMPRESSION_EXTENTION);

    char *cmd = (char *)malloc(l1 + l2 + l3 + 2);
    if (!cmd)
    {
        fprintf(stderr, "[ERROR]: Merge failed: malloc error");
        return EXIT_FAILURE;
    }
    strcpy(cmd, MERGE_COMMAND);
    strcat(cmd, _dir);
    strcat(cmd, SPACE_STRING);
    strcat(cmd, _fout);
    strcat(cmd, COMPRESSION_EXTENTION);

    if (system(cmd) == SYSTEM_CALL_FAIL_VALUE)
    {
        fprintf(stderr, "[ERROR]: Merge failed: system call error");
        _display(cmd);
        free(cmd);
        return EXIT_FAILURE;
    }
    free(cmd);
    return EXIT_SUCCESS;
}

int ALWAYS_INLINE countFilesInDir(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    // Open the directory
    dir = opendir(path);

    if (dir == NULL)
    {
        perror("Error opening directory");
        return -1;
    }

    // Count the files
    while ((entry = readdir(dir)) != NULL)
    {
        count++;
    }

    // Close the directory
    closedir(dir);

    return count - 2;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "Usage: %s [path-to-file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file_path = (char *)malloc(strlen(argv[1]));
    strcpy(file_path, argv[1]);

    ///////////////////////////////////////////////////////////////////

    status split_res = split(file_path);
    if (split_res == EXIT_FAILURE)
        exit(EXIT_FAILURE);

    ///////////////////////////////////////////////////////////////////

    int num_files = countFilesInDir(TEMP_FILES_DIR);
    fprintf(stdout, "%d ", num_files);
    while (num_files != NUM_WORKERS)
    {
        num_files = countFilesInDir(TEMP_FILES_DIR);
        fprintf(stdout, "%d ", num_files);
    }

    ///////////////////////////////////////////////////////////////////

    char *rle_file = (char *)malloc(strlen(file_path) + strlen(COMPRESSION_EXTENTION) + 1);
    strcpy(rle_file, file_path);
    strcat(rle_file, COMPRESSION_EXTENTION);
    status merge_res = merge(TEMP_FILES_DIR, rle_file);
    if (merge_res == EXIT_FAILURE)
        exit(EXIT_FAILURE);

    return EXIT_SUCCESS;
}