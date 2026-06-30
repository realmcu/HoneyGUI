#pragma once

#include "pixel.h"

/* MVE (Helium) intrinsics live in arm_mve.h on Cortex-M55/M85 with mve.fp.
 * __ARM_FEATURE_MVE: 0/undef = none, 1 = integer, 2 = integer + float. */
#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE >= 2)
#  include <arm_mve.h>
#endif

namespace agge
{
template <typename PixelT>
class blender_solid_color_rgb
{
public:
    typedef PixelT pixel;
    typedef uint8_t cover_type;

public:
    blender_solid_color_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);

    void operator()(pixel *pixels, int x, int y, count_t n) const;
    void operator()(pixel *pixels, int x, int y, count_t n, const cover_type *covers) const;

    /* Expose constants to free-function MVE worker. */
    uint8_t r_() const { return _r; }
    uint8_t g_() const { return _g; }
    uint8_t b_() const { return _b; }
    uint8_t a_() const { return _a; }

private:
    uint8_t _r, _g, _b, _a;
};

template <typename PixelT>
inline blender_solid_color_rgb<PixelT>::blender_solid_color_rgb(uint8_t r, uint8_t g, uint8_t b,
                                                                uint8_t a)
    : _r(r), _g(g), _b(b), _a(a)
{
}

template <typename PixelT>
inline void blender_solid_color_rgb<PixelT>::operator()(pixel *pixels, int /*x*/, int /*y*/,
                                                        count_t n) const
{
    pixel32_rgba p(_r, _g, _b, _a);
    for (; n; --n, ++pixels)
    {
        pixel_blend<PixelT, pixel32_rgba>(*pixels, p, p.a);
    }
}

template <typename PixelT>
inline void blender_solid_color_rgb<PixelT>::operator()(pixel *pixels, int /*x*/, int /*y*/,
                                                        count_t n, const cover_type *covers) const
{
    pixel32_rgba p(_r, _g, _b, _a);
    for (; n; --n, ++pixels, ++covers)
    {
        uint8_t a = (_a * covers[0]) >> 8;

        pixel_blend<PixelT, pixel32_rgba>(*pixels, p, a);
    }
}

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE >= 2)
/* ============================================================================
 * MVE-optimized solid color BGR565 blender (covers version).
 *
 * Speed strategy:
 *   - 8 pixels per iteration (one u16x8 vector = 8 x BGR565).
 *   - Unpack/pack BGR565 entirely in lanes:  px & 0x1F (b), (px>>5)&0x3F (g),
 *     (px>>11)&0x1F (r).  This is the hot bit the linear-gradient version
 *     left scalar -- biggest win lives here.
 *   - Alpha-class predicates (a>0xf4 / 0x01<a<=0xf4 / a<=0x01) become vcmpq
 *     masks; we issue three predicated stores, no branching in the inner loop.
 *   - Constants (_r>>3, _g>>2, _b>>3, _r*a, _g*a, _b*a as a function of a)
 *     can't be fully hoisted because 'a' varies per lane via covers, but the
 *     source-side multiplies become uniform broadcasts.
 *
 * Bit-exactness vs scalar pixel_blend (pixel.h:212):
 *   - branch a>0xf4   : t = (s.r>>3, s.g>>2, s.b>>3)
 *   - branch a>0x01   : t.r = (s.r*a + (t.r<<3)*m_a) >> 11   (etc)
 *   - else            : leave t untouched
 *========================================================================== */
static inline void blender_sc_mve_bgr565(
    const blender_solid_color_rgb<pixel16_bgr565> &b,
    pixel16_bgr565 *pixels, count_t n, const uint8_t *covers)
{
    const uint8_t sr = b.r_(), sg = b.g_(), sb = b.b_(), sa = b.a_();

    /* High-alpha precomputes (constant for the whole call). */
    const uint16_t sr5 = (uint16_t)(sr >> 3);
    const uint16_t sg6 = (uint16_t)(sg >> 2);
    const uint16_t sb5 = (uint16_t)(sb >> 3);

    const uint16x8_t v_sr8  = vdupq_n_u16(sr);          /* full 8-bit source channels */
    const uint16x8_t v_sg8  = vdupq_n_u16(sg);
    const uint16x8_t v_sb8  = vdupq_n_u16(sb);
    const uint16x8_t v_sr5  = vdupq_n_u16(sr5);         /* high-alpha shortcuts */
    const uint16x8_t v_sg6  = vdupq_n_u16(sg6);
    const uint16x8_t v_sb5  = vdupq_n_u16(sb5);

    const uint16x8_t v_mask_5 = vdupq_n_u16(0x1F);
    const uint16x8_t v_mask_6 = vdupq_n_u16(0x3F);
    const uint16x8_t v_0xff   = vdupq_n_u16(0xFF);

    uint16_t *p16 = reinterpret_cast<uint16_t *>(pixels);

    while (n > 0)
    {
        mve_pred16_t pg = vctp16q((uint32_t)n);          /* tail mask, lane-wise */

        /* covers: u8x8 widen to u16x8 (predicate load handles tail). */
        uint16x8_t v_cov = vldrbq_z_u16(covers, pg);

        /* a = (_a * cover) >> 8, per lane */
        uint16x8_t v_a = vshrq_n_u16(vmulq_n_u16(v_cov, (uint16_t)sa), 8);

        /* alpha-class predicates */
        mve_pred16_t p_hi  = vcmpcsq_m_n_u16(v_a, 0xf5, pg);          /* a >= 0xf5 (== a > 0xf4) */
        mve_pred16_t p_mid = vcmpcsq_m_n_u16(v_a, 0x02, pg);          /* a >= 0x02 (== a > 0x01) */
        /* p_real_mid = a >= 0x02 AND NOT high -- i.e. 0x02..0xf4 */

        /* Load destination pixels */
        uint16x8_t v_px = vldrhq_z_u16(p16, pg);

        /* Unpack BGR565 -> b/g/r u16 (each lane 5/6/5 bit value) */
        uint16x8_t v_tb = vandq_u16(v_px, v_mask_5);
        uint16x8_t v_tg = vandq_u16(vshrq_n_u16(v_px, 5), v_mask_6);
        uint16x8_t v_tr = vshrq_n_u16(v_px, 11);          /* already 5-bit because u16 */

        /* High-alpha lanes: t = (sr5, sg6, sb5) -- overwrite r/g/b */
        v_tr = vpselq_u16(v_sr5, v_tr, p_hi);
        v_tg = vpselq_u16(v_sg6, v_tg, p_hi);
        v_tb = vpselq_u16(v_sb5, v_tb, p_hi);

        /* Mid-alpha lanes: t.r = (sr*a + (t.r<<3)*m_a) >> 11
         *                  t.g = (sg*a + (t.g<<2)*m_a) >> 10
         *                  t.b = (sb*a + (t.b<<3)*m_a) >> 11
         *
         * We need the mid mask = p_mid AND NOT p_hi, computed by inverting
         * p_hi on top of p_mid. */
        mve_pred16_t p_mid_only = p_mid & (mve_pred16_t)(~p_hi);

        uint16x8_t v_ma = vsubq_u16(v_0xff, v_a);

        /* r: (sr*a + (tr<<3)*m_a) >> 11 */
        uint16x8_t v_tr8 = vshlq_n_u16(v_tr, 3);
        uint16x8_t v_rmix = vshrq_n_u16(
                                vaddq_u16(vmulq_u16(v_sr8, v_a),
                                          vmulq_u16(v_tr8, v_ma)),
                                11);
        v_tr = vpselq_u16(v_rmix, v_tr, p_mid_only);

        /* g: (sg*a + (tg<<2)*m_a) >> 10 */
        uint16x8_t v_tg8 = vshlq_n_u16(v_tg, 2);
        uint16x8_t v_gmix = vshrq_n_u16(
                                vaddq_u16(vmulq_u16(v_sg8, v_a),
                                          vmulq_u16(v_tg8, v_ma)),
                                10);
        v_tg = vpselq_u16(v_gmix, v_tg, p_mid_only);

        /* b: (sb*a + (tb<<3)*m_a) >> 11 */
        uint16x8_t v_tb8 = vshlq_n_u16(v_tb, 3);
        uint16x8_t v_bmix = vshrq_n_u16(
                                vaddq_u16(vmulq_u16(v_sb8, v_a),
                                          vmulq_u16(v_tb8, v_ma)),
                                11);
        v_tb = vpselq_u16(v_bmix, v_tb, p_mid_only);

        /* Pack back to BGR565: (r<<11) | (g<<5) | b */
        uint16x8_t v_out = vorrq_u16(
                               vorrq_u16(vshlq_n_u16(v_tr, 11),
                                         vshlq_n_u16(v_tg, 5)),
                               v_tb);

        /* Store with predicate (handles tail and the low-alpha "do nothing" lanes:
         * for low-alpha lanes v_out equals the loaded value, so writing them is a nop). */
        vstrhq_p_u16(p16, v_out, pg);

        int lanes = (n < 8) ? (int)n : 8;
        p16    += lanes;
        covers += lanes;
        n      -= lanes;
    }
}

/* MVE specialization for BGR565 -- picks the vectorised path unconditionally.
 * Other PixelT instantiations keep the scalar primary template above. */
template <>
inline void blender_solid_color_rgb<pixel16_bgr565>::operator()(
    pixel16_bgr565 *pixels, int /*x*/, int /*y*/,
    count_t n, const cover_type *covers) const
{
    blender_sc_mve_bgr565(*this, pixels, n, covers);
}
#endif  /* __ARM_FEATURE_MVE */

}  // namespace agge
