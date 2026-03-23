/**
 * @file trvg_mve.h
 * @brief TRVG MVE (Helium) Acceleration Support
 *
 * ARM Cortex-M55/M85 MVE (M-Profile Vector Extension) optimizations
 * Compiler: armclang 6.22+
 * Target: -march=armv8.1-m.main+dsp+mve.fp+fp.dp
 */

#ifndef TRVG_MVE_H
#define TRVG_MVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * MVE Feature Detection
 * ============================================================================ */

/*
 * __ARM_FEATURE_MVE:
 *   0 or undefined - MVE not available
 *   1 - MVE integer only
 *   2 - MVE integer + floating-point (mve.fp)
 */
#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE >= 1)
#define TRVG_USE_MVE        1
#include <arm_mve.h>
#else
#define TRVG_USE_MVE        0
#endif

/* MVE vector width: 128 bits */
#define TRVG_MVE_VECTOR_BITS    128
#define TRVG_MVE_U32_LANES      4       /* 128 / 32 = 4 lanes */
#define TRVG_MVE_U16_LANES      8       /* 128 / 16 = 8 lanes */
#define TRVG_MVE_U8_LANES       16      /* 128 / 8  = 16 lanes */

/* ============================================================================
 * MVE Optimized Functions
 * ============================================================================ */

#if TRVG_USE_MVE

/**
 * @brief MVE optimized memory fill with 32-bit value
 * @param dst Destination pointer (should be 4-byte aligned for best performance)
 * @param value 32-bit value to fill
 * @param count Number of 32-bit words to fill
 */
static inline void trvg_mve_fill_u32(uint32_t *dst, uint32_t value, int count)
{
    /* Create vector with duplicated value */
    uint32x4_t vval = vdupq_n_u32(value);

    /* Process 4 pixels per iteration */
    int vec_count = count >> 2;  /* count / 4 */

    for (int i = 0; i < vec_count; i++)
    {
        vst1q_u32(dst, vval);
        dst += 4;
    }

    /* Handle remaining pixels */
    int remaining = count & 3;  /* count % 4 */
    for (int i = 0; i < remaining; i++)
    {
        *dst++ = value;
    }
}

/**
 * @brief MVE optimized memory fill with 16-bit value
 * @param dst Destination pointer (should be 2-byte aligned for best performance)
 * @param value 16-bit value to fill
 * @param count Number of 16-bit halfwords to fill
 */
static inline void trvg_mve_fill_u16(uint16_t *dst, uint16_t value, int count)
{
    /* Create vector with duplicated value */
    uint16x8_t vval = vdupq_n_u16(value);

    /* Process 8 pixels per iteration */
    int vec_count = count >> 3;  /* count / 8 */

    for (int i = 0; i < vec_count; i++)
    {
        vst1q_u16(dst, vval);
        dst += 8;
    }

    /* Handle remaining pixels */
    int remaining = count & 7;  /* count % 8 */
    for (int i = 0; i < remaining; i++)
    {
        *dst++ = value;
    }
}

/**
 * @brief MVE optimized memory fill with 8-bit value
 * @param dst Destination pointer
 * @param value 8-bit value to fill
 * @param count Number of bytes to fill
 */
static inline void trvg_mve_fill_u8(uint8_t *dst, uint8_t value, int count)
{
    /* Create vector with duplicated value */
    uint8x16_t vval = vdupq_n_u8(value);

    /* Process 16 bytes per iteration */
    int vec_count = count >> 4;  /* count / 16 */

    for (int i = 0; i < vec_count; i++)
    {
        vst1q_u8(dst, vval);
        dst += 16;
    }

    /* Handle remaining bytes */
    int remaining = count & 15;  /* count % 16 */
    for (int i = 0; i < remaining; i++)
    {
        *dst++ = value;
    }
}

/**
 * @brief MVE optimized RGBA8888 pixel fill
 * @param dst Destination pointer (uint8_t*, pixel buffer)
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 * @param count Number of pixels to fill
 */
static inline void trvg_mve_fill_rgba8888(uint8_t *dst, uint8_t r, uint8_t g,
                                          uint8_t b, uint8_t a, int count)
{
    /* Pack RGBA into 32-bit value */
    uint32_t pixel = ((uint32_t)a << 24) | ((uint32_t)b << 16) |
                     ((uint32_t)g << 8) | (uint32_t)r;
    trvg_mve_fill_u32((uint32_t *)dst, pixel, count);
}

/**
 * @brief MVE optimized BGRA8888 pixel fill
 * @param dst Destination pointer (uint8_t*, pixel buffer)
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 * @param count Number of pixels to fill
 */
static inline void trvg_mve_fill_bgra8888(uint8_t *dst, uint8_t r, uint8_t g,
                                          uint8_t b, uint8_t a, int count)
{
    /* Pack BGRA into 32-bit value */
    uint32_t pixel = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                     ((uint32_t)g << 8) | (uint32_t)b;
    trvg_mve_fill_u32((uint32_t *)dst, pixel, count);
}

/**
 * @brief MVE optimized RGB565 pixel fill
 * @param dst Destination pointer
 * @param r Red component (8-bit, will be converted to 5-bit)
 * @param g Green component (8-bit, will be converted to 6-bit)
 * @param b Blue component (8-bit, will be converted to 5-bit)
 * @param count Number of pixels to fill
 */
static inline void trvg_mve_fill_rgb565(uint16_t *dst, uint8_t r, uint8_t g,
                                        uint8_t b, int count)
{
    /* Convert RGB888 to RGB565 */
    uint16_t pixel = ((uint16_t)(r >> 3) << 11) |
                     ((uint16_t)(g >> 2) << 5) |
                     (uint16_t)(b >> 3);
    trvg_mve_fill_u16(dst, pixel, count);
}

/**
 * @brief MVE optimized BGR565 pixel fill
 * @param dst Destination pointer
 * @param r Red component (8-bit, will be converted to 5-bit)
 * @param g Green component (8-bit, will be converted to 6-bit)
 * @param b Blue component (8-bit, will be converted to 5-bit)
 * @param count Number of pixels to fill
 */
static inline void trvg_mve_fill_bgr565(uint16_t *dst, uint8_t r, uint8_t g,
                                        uint8_t b, int count)
{
    /* Convert RGB888 to BGR565 */
    uint16_t pixel = ((uint16_t)(b >> 3) << 11) |
                     ((uint16_t)(g >> 2) << 5) |
                     (uint16_t)(r >> 3);
    trvg_mve_fill_u16(dst, pixel, count);
}

/**
 * @brief MVE optimized memory copy
 * @param dst Destination pointer
 * @param src Source pointer
 * @param bytes Number of bytes to copy
 */
static inline void trvg_mve_memcpy(void *dst, const void *src, int bytes)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;

    /* Process 16 bytes per iteration */
    int vec_count = bytes >> 4;

    for (int i = 0; i < vec_count; i++)
    {
        uint8x16_t v = vld1q_u8(s);
        vst1q_u8(d, v);
        s += 16;
        d += 16;
    }

    /* Handle remaining bytes */
    int remaining = bytes & 15;
    for (int i = 0; i < remaining; i++)
    {
        *d++ = *s++;
    }
}

#else /* !TRVG_USE_MVE */

/* Fallback implementations when MVE is not available */

static inline void trvg_mve_fill_u32(uint32_t *dst, uint32_t value, int count)
{
    for (int i = 0; i < count; i++)
    {
        *dst++ = value;
    }
}

static inline void trvg_mve_fill_u16(uint16_t *dst, uint16_t value, int count)
{
    for (int i = 0; i < count; i++)
    {
        *dst++ = value;
    }
}

static inline void trvg_mve_fill_u8(uint8_t *dst, uint8_t value, int count)
{
    for (int i = 0; i < count; i++)
    {
        *dst++ = value;
    }
}

static inline void trvg_mve_fill_rgba8888(uint8_t *dst, uint8_t r, uint8_t g,
                                          uint8_t b, uint8_t a, int count)
{
    uint32_t pixel = ((uint32_t)a << 24) | ((uint32_t)b << 16) |
                     ((uint32_t)g << 8) | (uint32_t)r;
    trvg_mve_fill_u32((uint32_t *)dst, pixel, count);
}

static inline void trvg_mve_fill_bgra8888(uint8_t *dst, uint8_t r, uint8_t g,
                                          uint8_t b, uint8_t a, int count)
{
    uint32_t pixel = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                     ((uint32_t)g << 8) | (uint32_t)b;
    trvg_mve_fill_u32((uint32_t *)dst, pixel, count);
}

static inline void trvg_mve_fill_rgb565(uint16_t *dst, uint8_t r, uint8_t g,
                                        uint8_t b, int count)
{
    uint16_t pixel = ((uint16_t)(r >> 3) << 11) |
                     ((uint16_t)(g >> 2) << 5) |
                     (uint16_t)(b >> 3);
    trvg_mve_fill_u16(dst, pixel, count);
}

static inline void trvg_mve_fill_bgr565(uint16_t *dst, uint8_t r, uint8_t g,
                                        uint8_t b, int count)
{
    uint16_t pixel = ((uint16_t)(b >> 3) << 11) |
                     ((uint16_t)(g >> 2) << 5) |
                     (uint16_t)(r >> 3);
    trvg_mve_fill_u16(dst, pixel, count);
}

static inline void trvg_mve_memcpy(void *dst, const void *src, int bytes)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (int i = 0; i < bytes; i++)
    {
        *d++ = *s++;
    }
}

#endif /* TRVG_USE_MVE */

#ifdef __cplusplus
}
#endif

#endif /* TRVG_MVE_H */
