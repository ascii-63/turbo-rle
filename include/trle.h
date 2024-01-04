#include <stdint.h>

// RLE with specified escape char
unsigned _srlec8(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, uint8_t e);
unsigned _srled8(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen, uint8_t e);

// functions w/ overflow handling
unsigned srlec8(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, uint8_t e);
unsigned srled8(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, unsigned outlen, uint8_t e);

// RLE w. automatic escape char determination
unsigned srlec(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out);
unsigned _srled(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen);
unsigned srled(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, unsigned outlen);

// Turbo RLE
unsigned trlec(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out);
unsigned _trled(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen);
unsigned trled(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, unsigned outlen);
