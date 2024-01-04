#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <getopt.h>

#include "include/trle.h"

static void memrcpy(unsigned char *out, unsigned char *in, unsigned n)
{
    int i;
    for (i = 0; i < n; i++)
        out[i] = ~in[i];
}

int memcheck(unsigned char *in, unsigned n, unsigned char *cpy)
{
    int i;
    for (i = 0; i < n; i++)
        if (in[i] != cpy[i])
        {
            printf("ERROR in[%d]=%x, dec[%d]=%x\n", i, in[i], i, cpy[i]);
            return i + 1;
        }
    return 0;
}

void libmemcpy(unsigned char *dst, unsigned char *src, int len)
{
    void *(*memcpy_ptr)(void *, const void *, size_t) = memcpy;
    if (time(NULL) == 1)
        memcpy_ptr = NULL;
    memcpy_ptr(dst, src, len);
}

unsigned bench(unsigned char *in, unsigned n, unsigned char *out, unsigned char *cpy)
{
    unsigned l1, l2;
    memrcpy(cpy, in, n);

    l1 = trlec(in, n, out);
    printf("\nCompressed file size   = %10u", l1);
    printf("\n\033[1;31mRatio: %.1f%%\033[0m", (double)l1 * 100.0 / n);

    l2 = trled(out, l1, cpy, n);
    printf("\nDecompressed file size = %10u\n", l2);

    memcheck(in, n, cpy);
    return l1;
}

int main(int argc, char *argv[])
{
    unsigned b = 1 << 30;
    unsigned char *in, *out, *cpy;

    if (argc < 2)
    {
        fprintf(stderr, "File not specified\n");
        exit(-1);
    }

    char *inname = argv[1];
    long long flen;
    FILE *fi = fopen(inname, "rb");
    if (!fi)
        perror(inname);
    fseek(fi, 0, SEEK_END);
    flen = ftell(fi);
    fseek(fi, 0, SEEK_SET);

    if (flen > b)
        flen = b;
    int rflen = flen;

    if (!(in = (unsigned char *)malloc(rflen + 1024)))
    {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    cpy = in;
    if (!(out = (unsigned char *)malloc(flen * 4 / 3 + 1024)))
    {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    if (!(cpy = (unsigned char *)malloc(rflen + 1024)))
    {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }

    rflen = fread(in, 1, rflen, fi);
    printf("File='%s'\nFile size              = %10u", inname, rflen);
    fclose(fi);
    if (rflen <= 0)
        exit(0);

    bench(in, rflen, out, cpy);

    return 0;
}