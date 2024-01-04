#ifndef USIZE

#include <string.h>
#include <emmintrin.h>
#include <immintrin.h>

#include "include/conf.h"
#include "include/trle.h"
#include "trle_.h"

#define PREFETCH(_ip_, _rw_) __builtin_prefetch(_ip_, _rw_)

//------------------------------------- RLE 8 with Escape char ------------------------------------------------------------------
#define SRLE8 32
#define USIZE 8
#include "trled.c"

#define rmemset8(_op_, _c_, _i_) \
  while (_i_--)                  \
  *_op_++ = _c_

#define rmemset(_op_, _c_, _i_)                         \
  do                                                    \
  {                                                     \
    __m256i cv = T2(_mm256_set1_epi, USIZE)(_c_);       \
    unsigned char *_p = _op_;                           \
    _op_ += _i_;                                        \
    do                                                  \
      _mm256_storeu_si256((__m256i *)_p, cv), _p += 32; \
    while (_p < _op_);                                  \
  } while (0)

unsigned _srled8(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen, unsigned char e)
{
  uint8_t *ip = in, *op = out, c, *oe = out + outlen;
  uint32_t r;

  __m128i ev = _mm_set1_epi8(e);

  if (outlen >= SRLE8)
    while (op < out + (outlen - SRLE8))
    {
      uint32_t mask;
      __m128i v = _mm_loadu_si128((__m128i *)ip);
      _mm_storeu_si128((__m128i *)op, v);
      mask = _mm_movemask_epi8(_mm_cmpeq_epi8(v, ev));
      if (mask)
        goto a;
      ip += 16;
      op += 16;
      v = _mm_loadu_si128((__m128i *)ip);
      _mm_storeu_si128((__m128i *)op, v);
      mask = _mm_movemask_epi8(_mm_cmpeq_epi8(v, ev));
      if (mask)
        goto a;
      ip += 16;
      op += 16;

      PREFETCH(ip + 512, 0);
      continue;
    a:
      r = ctz32(mask);
      ip += r + 1;
      PREFETCH(ip + 512, 0);
      op += r;

      vlget32(ip, r);
      if (likely(r) >= 3)
      {
        uint8_t c = *ip++;
        r = r - 3 + 4;
        rmemset(op, c, r);
      }
      else
      {
        r++;
        rmemset8(op, e, r);
      }
    }

  while (op < out + outlen)
    if (likely((c = *ip++) != e))
    {
      *op++ = c;
    }
    else
    {
      vlget32(ip, r);
      if (likely(r) >= 3)
      {
        c = *ip++;
        r = r - 3 + 4;
        rmemset8(op, c, r);
      }
      else
      {
        r++;
        rmemset8(op, e, r);
      }
    }
  return ip - in;
}

static inline unsigned _srled8x(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen, unsigned char e, unsigned char ix)
{
  uint8_t *ip = in, *op = out, c, *oe = out + outlen;
  uint32_t r;
  __m128i ev = _mm_set1_epi8(e);

  if (outlen >= SRLE8)
    while (op < out + (outlen - SRLE8))
    {

      uint32_t mask;
      __m128i v = _mm_loadu_si128((__m128i *)ip);
      _mm_storeu_si128((__m128i *)op, v);
      mask = _mm_movemask_epi8(_mm_cmpeq_epi8(v, ev));
      if (mask)
        goto a;
      ip += 16;
      op += 16;
      v = _mm_loadu_si128((__m128i *)ip);
      _mm_storeu_si128((__m128i *)op, v);
      mask = _mm_movemask_epi8(_mm_cmpeq_epi8(v, ev));
      if (mask)
        goto a;
      ip += 16;
      op += 16;

      PREFETCH(ip + 512, 0);
      continue;
    a:
      r = ctz32(mask);
      ip += r + 1;
      PREFETCH(ip + 512, 0);
      op += r;

      vlget32(ip, r);

      int f = r & 1;
      r >>= 1;
      if (likely(r) >= 3)
      {
        uint8_t y = ip[0], c = f ? y : ix;
        ip += f;
        r = r - 3 + 4;
        rmemset(op, c, r);
      }
      else
      {
        r++;
        rmemset8(op, e, r);
      }
    }

  while (op < out + outlen)
    if (likely((c = *ip++) != e))
    {
      *op++ = c;
    }
    else
    {
      vlget32(ip, r);
      int f = r & 1;
      r >>= 1;
      if (likely(r) >= 3)
      {
        uint8_t c = f ? ip[0] : ix;
        ip += f;
        r = r - 3 + 4;
        rmemset(op, c, r);
      }
      else
      {
        r++;
        rmemset8(op, e, r);
      }
    }
  return ip - in;
}

unsigned _srled(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen)
{
  return _srled8(in + 1, out, outlen, *in);
}

unsigned srled(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, unsigned outlen)
{
  unsigned l;
  if (inlen == outlen)
    memcpy(out, in, outlen); // No compression
  else if (inlen == 1)
    memset(out, in[0], outlen); // Only 1 char
  else
    _srled8x(in + 2, out, outlen, in[0], in[1]); // AS((ip-in) == inlen,"FatalI l>inlen %d ", (ip-in) - inlen);
  return inlen;
}
//------------------------------------- TurboRLE ------------------------------------------
unsigned _trled(const unsigned char *__restrict in, unsigned char *__restrict out, unsigned outlen)
{
  uint8_t rmap[256] = {0}, *op = out, *ip = in;
  unsigned m = 0, i, c;

  if (!outlen)
    return 0;

  if (!(c = *ip++))
    return _srled8(ip + 1, out, outlen, *ip) + 2;

  for (i = 0; i != c; i++)
  {
    uint8_t *pb = &rmap[i << 3];
    unsigned u = ip[i], v;
    v = (u >> 0) & 1;
    m += v;
    pb[0] = v ? m : 0;
    v = (u >> 1) & 1;
    m += v;
    pb[1] = v ? m : 0;
    v = (u >> 2) & 1;
    m += v;
    pb[2] = v ? m : 0;
    v = (u >> 3) & 1;
    m += v;
    pb[3] = v ? m : 0;
    v = (u >> 4) & 1;
    m += v;
    pb[4] = v ? m : 0;
    v = (u >> 5) & 1;
    m += v;
    pb[5] = v ? m : 0;
    v = (u >> 6) & 1;
    m += v;
    pb[6] = v ? m : 0;
    v = (u >> 7) & 1;
    m += v;
    pb[7] = v ? m : 0;
  }
  ip += c;
  for (i = c * 8; i != 256; i++)
    rmap[i] = ++m;

  m--;
  unsigned char ix = *ip++;

  if (outlen >= 32)
    while (op < out + (outlen - 32))
    {
      uint64_t z = (uint64_t)rmap[ip[7]] << 56 | (uint64_t)rmap[ip[6]] << 48 | (uint64_t)rmap[ip[5]] << 40 | (uint64_t)rmap[ip[4]] << 32 | (uint32_t)rmap[ip[3]] << 24 | (uint32_t)rmap[ip[2]] << 16 | (uint32_t)rmap[ip[1]] << 8 | rmap[ip[0]];
      ctou64(op) = ctou64(ip);
      if (z)
        goto a;
      ip += 8;
      op += 8;
      continue;
    a:
      z = ctz64(z) >> 3;

      ip += z;
      op += z;
      c = rmap[*ip++];
      vlzget(ip, i, m, c - 1);
      if (i & 1)
        c = ix;
      else
        c = *ip++;
      i = (i >> 1) + TMIN;
      rmemset(op, c, i);
      PREFETCH(ip + 512, 0);
    }

  while (op < out + outlen)
  {
    if (likely(!(c = rmap[*ip])))
      *op++ = *ip++;
    else
    {
      ip++;
      vlzget(ip, i, m, c - 1);
      if (i & 1)
        c = ix;
      else
        c = *ip++;
      i = (i >> 1) + TMIN;
      rmemset8(op, c, i);
    }
  }
  return ip - in;
}

unsigned trled(const unsigned char *__restrict in, unsigned inlen, unsigned char *__restrict out, unsigned outlen)
{
  if (inlen == outlen)
    memcpy(out, in, outlen);
  else if (inlen == 1)
    memset(out, in[0], outlen);
  else
  {
    unsigned l = _trled(in, out, outlen); // AS(l == inlen,"FatalI l>inlen %d ", l - inlen);
  }
  // return inlen;
  return outlen;
}

#undef USIZE
#undef rmemset
#undef SRLE8

#endif
