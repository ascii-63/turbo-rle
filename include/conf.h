// conf.h - config & common
#ifndef CONF_H_
#define CONF_H_

#include <stdint.h>
#include <stddef.h>

//------------------------------------------ Compiler ------------------------------------------//
#include <stdint.h>

#define ALIGNED(t, v, n) t v __attribute__((aligned(n)))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define _PACKED __attribute__((packed))
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define bswap64(x) __builtin_bswap64(x)

#define popcnt32(_x_) __builtin_popcount(_x_)
#define popcnt64(_x_) __builtin_popcountll(_x_)

static ALWAYS_INLINE int __bsr32(int x)
{
  asm("bsr  %1,%0" : "=r"(x) : "rm"(x));
  return x;
}
static ALWAYS_INLINE int bsr32(int x)
{
  int b = -1;
  asm("bsrl %1,%0" : "+r"(b) : "rm"(x));
  return b + 1;
}
static ALWAYS_INLINE int bsr64(uint64_t x) { return x ? 64 - __builtin_clzll(x) : 0; }
static ALWAYS_INLINE int __bsr64(uint64_t x) { return 63 - __builtin_clzll(x); }

static ALWAYS_INLINE unsigned rol32(unsigned x, int s)
{
  asm("roll %%cl,%0" : "=r"(x) : "0"(x), "c"(s));
  return x;
}
static ALWAYS_INLINE unsigned ror32(unsigned x, int s)
{
  asm("rorl %%cl,%0" : "=r"(x) : "0"(x), "c"(s));
  return x;
}
static ALWAYS_INLINE uint64_t rol64(uint64_t x, int s)
{
  asm("rolq %%cl,%0" : "=r"(x) : "0"(x), "c"(s));
  return x;
}
static ALWAYS_INLINE uint64_t ror64(uint64_t x, int s)
{
  asm("rorq %%cl,%0" : "=r"(x) : "0"(x), "c"(s));
  return x;
}

#define ctz64(_x_) __builtin_ctzll(_x_)
#define ctz32(_x_) __builtin_ctz(_x_)
#define clz64(_x_) __builtin_clzll(_x_)
#define clz32(_x_) __builtin_clz(_x_)

#define __bsr8(_x_) __bsr32(_x_)
#define __bsr16(_x_) __bsr32(_x_)
#define bsr8(_x_) bsr32(_x_)
#define bsr16(_x_) bsr32(_x_)
#define ctz8(_x_) ctz32(_x_)
#define ctz16(_x_) ctz32(_x_)
#define clz8(_x_) (clz32(_x_) - 24)
#define clz16(_x_) (clz32(_x_) - 16)

#define popcnt8(x) popcnt32(x)
#define popcnt16(x) popcnt32(x)
//------------------------------------------ Unaligned memory access ------------------------------------------//
#define ctou16(_cp_) (*(unsigned short *)(_cp_))
#define ctou32(_cp_) (*(unsigned *)(_cp_))
#define ctof32(_cp_) (*(float *)(_cp_))

#define stou16(_cp_, _x_) (*(unsigned short *)(_cp_) = _x_)
#define stou32(_cp_, _x_) (*(unsigned *)(_cp_) = _x_)
#define stof32(_cp_, _x_) (*(float *)(_cp_) = _x_)

#define ltou32(_px_, _cp_) *(_px_) = *(unsigned *)(_cp_)
#define ltou64(_px_, _cp_) *(_px_) = *(uint64_t *)(_cp_)

#define ctou64(_cp_) (*(uint64_t *)(_cp_))
#define ctof64(_cp_) (*(double *)(_cp_))

#define stou64(_cp_, _x_) (*(uint64_t *)(_cp_) = _x_)
#define stof64(_cp_, _x_) (*(double *)(_cp_) = _x_)

#define ctou24(_cp_) (ctou32(_cp_) & 0xffffff)
#define ctou48(_cp_) (ctou64(_cp_) & 0xffffffffffffull)
#define ctou8(_cp_) (*(_cp_))
//------------------------------------------ Wordsize ------------------------------------------//
#define __WORDSIZE 64

#endif
//------------------------------------------ Misc ------------------------------------------//
#define BZMASK64(_b_) (~(~0ull << (_b_)))
#define BZMASK32(_b_) (~(~0u << (_b_)))
#define BZMASK16(_b_) BZMASK32(_b_)
#define BZMASK8(_b_) BZMASK32(_b_)

#define BZHI64(_u_, _b_) ((_u_) & BZMASK64(_b_)) // b Constant
#define BZHI32(_u_, _b_) ((_u_) & BZMASK32(_b_))
#define BZHI16(_u_, _b_) BZHI32(_u_, _b_)
#define BZHI8(_u_, _b_) BZHI32(_u_, _b_)
#define BEXTR32(x, start, len) (((x) >> (start)) & ((1u << (len)) - 1)) // Bit field extract (with register)

#define bzhi64(_u_, _b_) BZHI64(_u_, _b_)
#define bzhi32(_u_, _b_) BZHI32(_u_, _b_)
#define bextr32(x, start, len) (((x) >> (start)) & ((1u << (len)) - 1)) // Bit field extract (with register)

#define bzhi16(_u_, _b_) bzhi32(_u_, _b_)
#define bzhi8(_u_, _b_) bzhi32(_u_, _b_)

#define SIZE_ROUNDUP(_n_, _a_) (((size_t)(_n_) + (size_t)((_a_)-1)) & ~(size_t)((_a_)-1))
#define ALIGN_DOWN(__ptr, __a) ((void *)((uintptr_t)(__ptr) & ~(uintptr_t)((__a)-1)))

#define T2_(_x_, _y_) _x_##_y_
#define T2(_x_, _y_) T2_(_x_, _y_)

#define T3_(_x_, _y_, _z_) _x_##_y_##_z_
#define T3(_x_, _y_, _z_) T3_(_x_, _y_, _z_)

#define CACHE_LINE_SIZE 64
#define PREFETCH_DISTANCE (CACHE_LINE_SIZE * 4)

#define CLAMP(_x_, _low_, _high_) (((_x_) > (_high_)) ? (_high_) : (((_x_) < (_low_)) ? (_low_) : (_x_)))
//------------------------------------------ NDEBUG ------------------------------------------//
#include <stdio.h>

#define AS(expr, fmt, args...)                                        \
  do                                                                  \
  {                                                                   \
    if (!(expr))                                                      \
    {                                                                 \
      fflush(stdout);                                                 \
      fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
      fprintf(stderr, fmt, ##args);                                   \
      fflush(stderr);                                                 \
      exit(-1);                                                       \
    }                                                                 \
  } while (0)

#define AC(expr, fmt, args...)                                        \
  do                                                                  \
  {                                                                   \
    if (!(expr))                                                      \
    {                                                                 \
      fflush(stdout);                                                 \
      fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
      fprintf(stderr, fmt, ##args);                                   \
      fflush(stderr);                                                 \
      exit(-1);                                                       \
    }                                                                 \
  } while (0)

#define die(fmt, args...)                                           \
  do                                                                \
  {                                                                 \
    fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, fmt, ##args);                                   \
    fflush(stderr);                                                 \
    exit(-1);                                                       \
  } while (0)
