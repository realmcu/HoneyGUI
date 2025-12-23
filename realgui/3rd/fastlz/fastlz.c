/*
  FastLZ - Byte-aligned LZ77 compression library
  Copyright (C) 2005-2020 Ariya Hidayat <ariya.hidayat@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "fastlz.h"

/*
 * Always check for bound when decompressing.
 * Generally it is best to leave it defined.
 */
#define FASTLZ_SAFE
#if defined(FASTLZ_USE_SAFE_DECOMPRESSOR) && (FASTLZ_USE_SAFE_DECOMPRESSOR == 0)
#undef FASTLZ_SAFE
#endif

/*
 * Give hints to the compiler for branch prediction optimization.
 */
#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 2))
#define FASTLZ_LIKELY(c) (__builtin_expect(!!(c), 1))
#define FASTLZ_UNLIKELY(c) (__builtin_expect(!!(c), 0))
#else
#define FASTLZ_LIKELY(c) (c)
#define FASTLZ_UNLIKELY(c) (c)
#endif

/*
 * Specialize custom 64-bit implementation for speed.
 */
#if !defined(FASTLZ_STRICT_ALIGN)
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || \
    defined(_M_X64) || defined(_M_AMD64) || defined(__aarch64__)
#define FLZ_ARCH64
#endif
#endif

/*
 * Prevent accessing more than 8-bit at once, except on x86/x64 architectures.
 */
#if !defined(FASTLZ_STRICT_ALIGN)
#if !defined(__i386__) && !defined(__386) && !defined(_M_IX86)
#if !defined(__x86_64__) && !defined(__amd64__) && !defined(_M_X64) && !defined(_M_AMD64)
#if !defined(__aarch64__)
#define FASTLZ_STRICT_ALIGN
#endif
#endif
#endif
#endif

/*
 * FIXME: use typeof?
 * But then we have to use __typeof__ for portability (since some compilers
 * only accept __typeof__).
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define HASH_LOG 13
#define HASH_SIZE (1 << HASH_LOG)
#define HASH_MASK (HASH_SIZE - 1)

#if defined(FASTLZ_STRICT_ALIGN)

#define FASTLZ_READU16(p) ((p)[0] | (p)[1] << 8)

#define FASTLZ_READU32(p) \
    ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))

#else /* !FASTLZ_STRICT_ALIGN */

#define FASTLZ_READU16(p) (*((const unsigned short *)(p)))
#define FASTLZ_READU32(p) (*((const unsigned int *)(p)))

#endif /* FASTLZ_STRICT_ALIGN */

#define HASH_FUNCTION(v, p)            \
    {                                    \
        v = FASTLZ_READU16(p);             \
        v ^= FASTLZ_READU16(p + 1) ^ (v >> (16 - HASH_LOG)); \
        v &= HASH_MASK;                    \
    }

static void flz_copy64(const void *src, void *dest)
{
    const unsigned char *p = (const unsigned char *)src;
    unsigned char *q = (unsigned char *)dest;
    int i;
    for (i = 0; i < 8; ++i) { *q++ = *p++; }
}

static void flz_copy256(const void *src, void *dest)
{
    flz_copy64((const unsigned char *)src, (unsigned char *)dest);
    flz_copy64((const unsigned char *)src + 8, (unsigned char *)dest + 8);
    flz_copy64((const unsigned char *)src + 16, (unsigned char *)dest + 16);
    flz_copy64((const unsigned char *)src + 24, (unsigned char *)dest + 24);
}

/* Unused function - kept for potential future use
static void flz_memmove(unsigned char *dest, const unsigned char *src, unsigned int count) {
  if ((count > 4) && (dest >= src + count)) {
    unsigned int i;
    for (i = 0; i < count; ++i) dest[i] = src[i];
  } else {
    unsigned int i;
    for (i = count; i > 0; --i) dest[i - 1] = src[i - 1];
  }
}
*/

static void flz_memcpy(unsigned char *dest, const unsigned char *src, unsigned int count)
{
    unsigned int i;
    for (i = 0; i < count; ++i) { dest[i] = src[i]; }
}

static unsigned char *flz_literals(unsigned int runs, const unsigned char *src,
                                   unsigned char *dest)
{
    while (runs >= 32)
    {
        *dest++ = (unsigned char)((32 - 1) << 5);
        flz_copy256(src, dest);
        src += 32;
        dest += 32;
        runs -= 32;
    }
    if (runs > 0)
    {
        *dest++ = (runs - 1) << 5;
        flz_memcpy(dest, src, runs);
        dest += runs;
    }
    return dest;
}

static unsigned char *flz1_match(unsigned int len, unsigned int distance,
                                 unsigned char *op)
{
    --distance;
    if (FASTLZ_UNLIKELY(len < 7))
    {
        *op++ = (len << 5) + (distance >> 8);
        *op++ = (distance & 255);
    }
    else
    {
        *op++ = (7 << 5) + (distance >> 8);
        for (len -= 7; len >= 255; len -= 255) { *op++ = 255; }
        *op++ = len;
        *op++ = (distance & 255);
    }
    return op;
}

int fastlz1_compress(const void *input, int length, void *output)
{
    const unsigned char *ip = (const unsigned char *)input;
    const unsigned char *ip_bound = ip + length - 4;
    const unsigned char *ip_limit = ip + length - 12 - 1;
    unsigned char *op = (unsigned char *)output;

    const unsigned char *htab[HASH_SIZE];
    unsigned int hval;
    int copy;

    /* initializes hash table */
    for (hval = 0; hval < HASH_SIZE; ++hval) { htab[hval] = ip; }

    /* we start with literal copy */
    copy = 0;
    *op++ = 0;

    /* main loop */
    while (FASTLZ_LIKELY(ip < ip_limit))
    {
        const unsigned char *ref;
        unsigned int distance;
        unsigned int len;

        /* find potential match */
        HASH_FUNCTION(hval, ip);
        ref = htab[hval];
        htab[hval] = ip;

        /* is this a potential match? */
        distance = ip - ref;
        if (FASTLZ_UNLIKELY(distance > 8191 || ref >= ip))
        {
            ++copy;
            if (FASTLZ_UNLIKELY(copy == 32))
            {
                copy = 0;
                op = flz_literals(32, ip - 32 + 1, op - 1);
                *op++ = 0;
            }
            ++ip;
            continue;
        }

        /* is this a match? */
        if (FASTLZ_LIKELY(FASTLZ_READU32(ref) == FASTLZ_READU32(ip)))
        {
            /* copy pending literal */
            if (copy > 0)
            {
                op = flz_literals(copy, ip - copy, op - 1);
                copy = 0;
            }

            /* match length */
            ip += 4;
            ref += 4;
            len = 4;
            while ((ip < ip_bound) && (*ref == *ip))
            {
                ++ip;
                ++ref;
                ++len;
            }

            /* encode match */
            op = flz1_match(len, distance, op);

            /* keep a literal copy started */
            *op++ = 0;
            continue;
        }

        /* no match */
        ++copy;
        if (FASTLZ_UNLIKELY(copy == 32))
        {
            copy = 0;
            op = flz_literals(32, ip - 32 + 1, op - 1);
            *op++ = 0;
        }
        ++ip;
    }

    /* remaining literals */
    if (copy > 0)
    {
        op = flz_literals(copy, ip - copy, op - 1);
        copy = 0;
    }

    /* if we have remaining data, use the last 4 bytes as literal */
    {
        const unsigned char *ip_end = (const unsigned char *)input + length;
        unsigned int left = ip_end - ip;
        if (left > 0)
        {
            if (copy == 0) { *op++ = 0; }
            op = flz_literals(left, ip, op - 1);
        }
    }

    return op - (unsigned char *)output;
}

int fastlz1_decompress(const void *input, int length, void *output, int maxout)
{
    const unsigned char *ip = (const unsigned char *)input;
    const unsigned char *ip_limit = ip + length;
    const unsigned char *ip_bound = ip_limit - 2;
    unsigned char *op = (unsigned char *)output;
    unsigned char *op_limit = op + maxout;
    unsigned int ctrl = (*ip++) & 31;

    while (1)
    {
        if (ctrl >= 32)
        {
            unsigned int len = (ctrl >> 5) - 1;
            unsigned int ofs = (ctrl & 31) << 8;
            const unsigned char *ref = op - ofs - 1;
            if (len == 7 - 1)
            {
#ifdef FASTLZ_SAFE
                if (FASTLZ_UNLIKELY(ip > ip_bound)) { return 0; }
#endif
                len += *ip++;
            }
            ref -= *ip++;
            len += 3;
#ifdef FASTLZ_SAFE
            if (FASTLZ_UNLIKELY(op + len > op_limit)) { return 0; }
            if (FASTLZ_UNLIKELY(ref < (unsigned char *)output)) { return 0; }
#endif
            if (FASTLZ_LIKELY(ip < ip_limit)) { ctrl = *ip++; }
            else { break; }
            if (ref == op - 1)
            {
                unsigned char b = *ref;
                unsigned int i;
                for (i = 0; i < len; ++i) { *op++ = b; }
            }
            else
            {
                const unsigned char *src = ref;
                unsigned char *dest = op;
                unsigned int i;
                for (i = 0; i < len; ++i) { *dest++ = *src++; }
                op += len;
            }
        }
        else
        {
            ctrl++;
#ifdef FASTLZ_SAFE
            if (FASTLZ_UNLIKELY(op + ctrl > op_limit)) { return 0; }
            if (FASTLZ_UNLIKELY(ip + ctrl > ip_limit)) { return 0; }
#endif
            flz_memcpy(op, ip, ctrl);
            ip += ctrl;
            op += ctrl;
            if (FASTLZ_LIKELY(ip < ip_limit)) { ctrl = *ip++; }
            else { break; }
        }
    }

    return op - (unsigned char *)output;
}

static unsigned char *flz2_match(unsigned int len, unsigned int distance,
                                 unsigned char *op)
{
    --distance;
    if (distance < 8192)
    {
        if (len < 7)
        {
            *op++ = (len << 5) + (distance >> 8);
            *op++ = (distance & 255);
        }
        else
        {
            *op++ = (7 << 5) + (distance >> 8);
            for (len -= 7; len >= 255; len -= 255) { *op++ = 255; }
            *op++ = len;
            *op++ = (distance & 255);
        }
    }
    else
    {
        if (len < 7)
        {
            distance -= 8192;
            *op++ = (len << 5) + 31;
            *op++ = 255;
            *op++ = distance >> 8;
            *op++ = distance & 255;
        }
        else
        {
            distance -= 8192;
            *op++ = (7 << 5) + 31;
            for (len -= 7; len >= 255; len -= 255) { *op++ = 255; }
            *op++ = len;
            *op++ = 255;
            *op++ = distance >> 8;
            *op++ = distance & 255;
        }
    }
    return op;
}

int fastlz2_compress(const void *input, int length, void *output)
{
    const unsigned char *ip = (const unsigned char *)input;
    const unsigned char *ip_bound = ip + length - 4;
    const unsigned char *ip_limit = ip + length - 12 - 1;
    unsigned char *op = (unsigned char *)output;

    const unsigned char *htab[HASH_SIZE];
    unsigned int hval;
    int copy;

    /* initializes hash table */
    for (hval = 0; hval < HASH_SIZE; ++hval) { htab[hval] = ip; }

    /* we start with literal copy */
    copy = 0;
    *op++ = 0;

    /* main loop */
    while (FASTLZ_LIKELY(ip < ip_limit))
    {
        const unsigned char *ref;
        unsigned int distance;
        unsigned int len;

        /* find potential match */
        HASH_FUNCTION(hval, ip);
        ref = htab[hval];
        htab[hval] = ip;

        /* is this a potential match? */
        distance = ip - ref;
        if (FASTLZ_UNLIKELY(distance > 8191 + 8192 || ref >= ip))
        {
            ++copy;
            if (FASTLZ_UNLIKELY(copy == 32))
            {
                copy = 0;
                op = flz_literals(32, ip - 32 + 1, op - 1);
                *op++ = 0;
            }
            ++ip;
            continue;
        }

        /* is this a match? */
        if (FASTLZ_LIKELY(FASTLZ_READU32(ref) == FASTLZ_READU32(ip)))
        {
            /* copy pending literal */
            if (copy > 0)
            {
                op = flz_literals(copy, ip - copy, op - 1);
                copy = 0;
            }

            /* match length */
            ip += 4;
            ref += 4;
            len = 4;
            while ((ip < ip_bound) && (*ref == *ip))
            {
                ++ip;
                ++ref;
                ++len;
            }

            /* encode match */
            op = flz2_match(len, distance, op);

            /* keep a literal copy started */
            *op++ = 0;
            continue;
        }

        /* no match */
        ++copy;
        if (FASTLZ_UNLIKELY(copy == 32))
        {
            copy = 0;
            op = flz_literals(32, ip - 32 + 1, op - 1);
            *op++ = 0;
        }
        ++ip;
    }

    /* remaining literals */
    if (copy > 0)
    {
        op = flz_literals(copy, ip - copy, op - 1);
        copy = 0;
    }

    /* if we have remaining data, use the last 4 bytes as literal */
    {
        const unsigned char *ip_end = (const unsigned char *)input + length;
        unsigned int left = ip_end - ip;
        if (left > 0)
        {
            if (copy == 0) { *op++ = 0; }
            op = flz_literals(left, ip, op - 1);
        }
    }

    /* marker for fastlz2 */
    *(unsigned char *)output |= (1 << 5);

    return op - (unsigned char *)output;
}

int fastlz2_decompress(const void *input, int length, void *output, int maxout)
{
    const unsigned char *ip = (const unsigned char *)input;
    const unsigned char *ip_limit = ip + length;
    const unsigned char *ip_bound = ip_limit - 2;
    unsigned char *op = (unsigned char *)output;
    unsigned char *op_limit = op + maxout;
    unsigned int ctrl = (*ip++) & 31;

    while (1)
    {
        if (ctrl >= 32)
        {
            unsigned int len = (ctrl >> 5) - 1;
            unsigned int ofs = (ctrl & 31) << 8;
            const unsigned char *ref = op - ofs - 1;
            unsigned char code;
            if (len == 7 - 1)
            {
                do
                {
#ifdef FASTLZ_SAFE
                    if (FASTLZ_UNLIKELY(ip > ip_bound)) { return 0; }
#endif
                    code = *ip++;
                    len += code;
                }
                while (code == 255);
            }
            code = *ip++;
            ref -= code;
            len += 3;

            /* match from 16-bit distance */
            if (FASTLZ_UNLIKELY(code == 255))
            {
#ifdef FASTLZ_SAFE
                if (FASTLZ_UNLIKELY(ip > ip_bound)) { return 0; }
#endif
                ofs = (*ip++) << 8;
                ofs += *ip++;
                ref = op - ofs - 8192 - 1;
            }

#ifdef FASTLZ_SAFE
            if (FASTLZ_UNLIKELY(op + len > op_limit)) { return 0; }
            if (FASTLZ_UNLIKELY(ref < (unsigned char *)output)) { return 0; }
#endif
            if (FASTLZ_LIKELY(ip < ip_limit)) { ctrl = *ip++; }
            else { break; }
            if (ref == op - 1)
            {
                unsigned char b = *ref;
                unsigned int i;
                for (i = 0; i < len; ++i) { *op++ = b; }
            }
            else
            {
                const unsigned char *src = ref;
                unsigned char *dest = op;
                unsigned int i;
                for (i = 0; i < len; ++i) { *dest++ = *src++; }
                op += len;
            }
        }
        else
        {
            ctrl++;
#ifdef FASTLZ_SAFE
            if (FASTLZ_UNLIKELY(op + ctrl > op_limit)) { return 0; }
            if (FASTLZ_UNLIKELY(ip + ctrl > ip_limit)) { return 0; }
#endif
            flz_memcpy(op, ip, ctrl);
            ip += ctrl;
            op += ctrl;
            if (FASTLZ_LIKELY(ip < ip_limit)) { ctrl = *ip++; }
            else { break; }
        }
    }

    return op - (unsigned char *)output;
}

int fastlz_compress(const void *input, int length, void *output)
{
    /* for short block, choose fastlz1 */
    if (length < 65536) { return fastlz1_compress(input, length, output); }

    /* else... */
    return fastlz2_compress(input, length, output);
}

int fastlz_decompress(const void *input, int length, void *output, int maxout)
{
    /* magic identifier for compression level */
    int level = ((*(const unsigned char *)input) >> 5) + 1;

    if (level == 1) { return fastlz1_decompress(input, length, output, maxout); }
    if (level == 2) { return fastlz2_decompress(input, length, output, maxout); }

    /* unknown level, trigger error */
    return 0;
}

int fastlz_compress_level(int level, const void *input, int length, void *output)
{
    if (level == 1) { return fastlz1_compress(input, length, output); }
    if (level == 2) { return fastlz2_compress(input, length, output); }

    return 0;
}
