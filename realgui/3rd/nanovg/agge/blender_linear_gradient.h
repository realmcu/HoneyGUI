#pragma once
#include <cassert>
#include <cstdio>
#include "pixel.h"

/* MVE (Helium) intrinsics live in arm_mve.h on Cortex-M55/M85 with mve.fp.
 * __ARM_FEATURE_MVE: 0/undef = none, 1 = integer, 2 = integer + float. */
#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE >= 2)
#  include <arm_mve.h>
#endif

namespace agge
{
template <typename PixelT>
class blender_linear_gradient
{
public:
    typedef PixelT pixel;
    typedef uint8_t cover_type;

public:
    blender_linear_gradient(float sx, float sy, float ex, float ey, pixel32_rgba sc, pixel32_rgba ec);
    void operator()(pixel *pixels, int x, int y, count_t n, const cover_type *covers) const;
    bool gradient(float factor, pixel32_rgba &c) const;
    bool get_color(int x, int y, pixel32_rgba &c) const;

    /* Expose state to the MVE worker (defined in TU below). */
    float dx_()  const { return _dx;  }
    float dy_()  const { return _dy;  }
    float sx_()  const { return _sx;  }
    float sy_()  const { return _sy;  }
    float ex_()  const { return _ex;  }
    float ey_()  const { return _ey;  }
    float inv_() const { return _dot_product_1; }
    pixel32_rgba sc_() const { return _sc; }
    pixel32_rgba ec_() const { return _ec; }

private:
    float _sx;
    float _sy;
    float _ex;
    float _ey;
    float _dx;
    float _dy;
    float _dot_product_1;
    float _xw;
    float _yw;
    pixel32_rgba _sc;
    pixel32_rgba _ec;
};

template <typename PixelT>
inline blender_linear_gradient<PixelT>::blender_linear_gradient(float sx, float sy, float ex,
                                                                float ey,
                                                                pixel32_rgba sc, pixel32_rgba ec) : _sx(sx), _sy(sy), _ex(ex), _ey(ey), _sc(sc), _ec(ec)
{
    if (sx == ex && sy == ey)
    {
        assert(!"invalid params");
        _ex = _sx + 1;
        _ey = _sy + 1;
    }

    _dx = ex - sx;
    _dy = ey - sy;
    _dot_product_1 = 1 / (_dx * _dx + _dy * _dy);
}

template <typename PixelT>
inline bool blender_linear_gradient<PixelT>::gradient(float factor, pixel32_rgba &c) const
{
    if (factor <= 0)
    {
        c = _sc;
    }
    else if (factor >= 1.0f)
    {
        c = _ec;
    }
    else
    {
        c.r = _sc.r + (_ec.r - _sc.r) * factor;
        c.g = _sc.g + (_ec.g - _sc.g) * factor;
        c.b = _sc.b + (_ec.b - _sc.b) * factor;
        c.a = _sc.a + (_ec.a - _sc.a) * factor;
    }

    return true;
}

template <typename PixelT>
inline bool blender_linear_gradient<PixelT>::get_color(int x, int y, pixel32_rgba &c) const
{
    if (_sx == _ex && _sy == _ey)
    {
        c = _sc;
        return true;
    }
    else if (_sx == _ex)
    {
        return this->gradient((y - _sy) / _dy, c);
    }
    else if (_sy == _ey)
    {
        return this->gradient((x - _sx) / _dx, c);
    }
    else
    {
        //https://github.com/SFML/SFML/wiki/Source:-Color-Gradient
        /*if (x < _sx || y < _sy)
        {
            c = _sc;
        }
        else if (x > _ex || y > _ey)
        {
            c = _ec;
        }
        else*/
        {
            float dot_product = (x - _sx) * _dx + (y - _sy) * _dy;
            float factor = dot_product * _dot_product_1;

            return this->gradient(factor, c);
        }
    }

    //return true;
}

template <typename PixelT>
inline void blender_linear_gradient<PixelT>::operator()(pixel *pixels, int x, int y,
                                                        count_t n, const cover_type *covers) const
{
    pixel32_rgba p(0, 0, 0, 0);

    for (; n; --n, ++pixels, ++covers)
    {
        if (this->get_color(x++, y, p))
        {
            uint8_t a = (*covers * p.a) >> 8;
            pixel_blend<PixelT, pixel32_rgba>(*pixels, p, a);
        }
    }
}

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE >= 2)
/* ============================================================================
 * MVE-optimized inner loop, specialised for pixel16_bgr565.
 * Free function so we don't need to specialise a template method (which would
 * have to happen before the primary template gets instantiated).
 *
 *   - 4 pixels per iteration; lane k handles (x_base+k, y).
 *   - The float factor maths is vectorised: 4 fmul + 1 fmla per 4 pixels.
 *   - The 565 pack/unpack stays scalar so we get bit-exact match with the
 *     scalar fallback (verified by an XOR checksum of FB writes).
 *========================================================================== */
static inline void blender_lg_mve_bgr565(
    const blender_linear_gradient<pixel16_bgr565> &g,
    pixel16_bgr565 *pixels, int x_base, int y, count_t n,
    const uint8_t *covers)
{
    /* Degenerate gradient (sx==ex && sy==ey) -> defer to scalar; rare. */
    if (g.sx_() == g.ex_() && g.sy_() == g.ey_())
    {
        pixel32_rgba p(0, 0, 0, 0);
        int xi = x_base;
        for (count_t i = 0; i < n; ++i)
        {
            if (g.get_color(xi++, y, p))
            {
                uint8_t a = (covers[i] * p.a) >> 8;
                pixel_blend<pixel16_bgr565, pixel32_rgba>(pixels[i], p, a);
            }
        }
        return;
    }

    /* Splat invariants */
    const float32x4_t v_inv  = vdupq_n_f32(g.inv_());
    const float32x4_t v_dy   = vdupq_n_f32(g.dy_());
    const float32x4_t v_sx   = vdupq_n_f32(g.sx_());
    const float32x4_t v_zero = vdupq_n_f32(0.0f);
    const float32x4_t v_one  = vdupq_n_f32(1.0f);

    /* (y - sy) * dy is loop-invariant within one scanline */
    const float32x4_t v_y_term = vmulq_n_f32(v_dy, (float)y - g.sy_());

    /* Endpoint colours; precompute deltas as floats */
    pixel32_rgba sc = g.sc_();
    pixel32_rgba ec = g.ec_();
    const float32x4_t v_sc_r = vdupq_n_f32((float)sc.r);
    const float32x4_t v_sc_g = vdupq_n_f32((float)sc.g);
    const float32x4_t v_sc_b = vdupq_n_f32((float)sc.b);
    const float32x4_t v_sc_a = vdupq_n_f32((float)sc.a);
    const float32x4_t v_dr   = vdupq_n_f32((float)((int)ec.r - (int)sc.r));
    const float32x4_t v_dg   = vdupq_n_f32((float)((int)ec.g - (int)sc.g));
    const float32x4_t v_db   = vdupq_n_f32((float)((int)ec.b - (int)sc.b));
    const float32x4_t v_da   = vdupq_n_f32((float)((int)ec.a - (int)sc.a));

    /* {0,1,2,3} lane offset */
    const int32_t lane_init[4] = {0, 1, 2, 3};
    const float32x4_t v_lane = vcvtq_f32_s32(vld1q_s32(lane_init));

    float x_cur = (float)x_base;
    int   x_int = x_base;
    (void)x_int;

    while (n > 0)
    {
        int lanes = (n < 4) ? (int)n : 4;
        mve_pred16_t pg = vctp32q((uint32_t)n);

        float32x4_t v_x   = vaddq_f32(vdupq_n_f32(x_cur), v_lane);
        float32x4_t v_xa  = vsubq_f32(v_x, v_sx);
        float32x4_t v_dot = vfmaq_f32(v_y_term, v_xa, vdupq_n_f32(g.dx_()));
        float32x4_t v_f   = vmulq_f32(v_dot, v_inv);

        /* clamp to [0,1] using IEEE-quiet min/max */
        v_f = vminnmq_f32(v_f, v_one);
        v_f = vmaxnmq_f32(v_f, v_zero);

        /* Interp channels (float) and convert to u32 lanes */
        float32x4_t v_pr  = vfmaq_f32(v_sc_r, v_dr, v_f);
        float32x4_t v_pg_ = vfmaq_f32(v_sc_g, v_dg, v_f);
        float32x4_t v_pb  = vfmaq_f32(v_sc_b, v_db, v_f);
        float32x4_t v_pa  = vfmaq_f32(v_sc_a, v_da, v_f);

        /* As u32 lanes (we know each is 0..255) */
        uint32x4_t v_sr = vreinterpretq_u32_s32(vcvtq_s32_f32(v_pr));
        uint32x4_t v_sg = vreinterpretq_u32_s32(vcvtq_s32_f32(v_pg_));
        uint32x4_t v_sb = vreinterpretq_u32_s32(vcvtq_s32_f32(v_pb));
        uint32x4_t v_sa = vreinterpretq_u32_s32(vcvtq_s32_f32(v_pa));

        /* a_eff = (cover * src.a) >> 8 per lane */
        uint32x4_t v_cov = vldrbq_z_u32(covers, pg);          /* u8->u32 widen */
        uint32x4_t v_a   = vshrq_n_u32(vmulq_u32(v_cov, v_sa), 8);

        /* Load 4 destination BGR565 lanes (each lane holds one u16 in low 16 bits) */
        uint32x4_t v_px = vldrhq_z_u32((uint16_t *)pixels, pg);

        /* Unpack BGR565 lanes -> t_b/g/r per u32 lane (5/6/5 bits) */
        uint32x4_t v_tb = vandq_u32(v_px, vdupq_n_u32(0x1F));
        uint32x4_t v_tg = vandq_u32(vshrq_n_u32(v_px, 5), vdupq_n_u32(0x3F));
        uint32x4_t v_tr = vshrq_n_u32(v_px, 11);              /* upper bits already cleared by u16 load */

        /* alpha-class predicates (combine with tail pg) */
        mve_pred16_t p_hi  = vcmpcsq_m_n_u32(v_a, 0xf5, pg);                  /* a >= 0xf5 */
        mve_pred16_t p_mid = vcmpcsq_m_n_u32(v_a, 0x02, pg);                  /* a >= 0x02 */
        mve_pred16_t p_mid_only = p_mid & (mve_pred16_t)(~p_hi);

        /* High-alpha: t = (sr>>3, sg>>2, sb>>3) */
        uint32x4_t v_sr5 = vshrq_n_u32(v_sr, 3);
        uint32x4_t v_sg6 = vshrq_n_u32(v_sg, 2);
        uint32x4_t v_sb5 = vshrq_n_u32(v_sb, 3);
        v_tr = vpselq_u32(v_sr5, v_tr, p_hi);
        v_tg = vpselq_u32(v_sg6, v_tg, p_hi);
        v_tb = vpselq_u32(v_sb5, v_tb, p_hi);

        /* Mid-alpha: t.r = (sr*a + (tr<<3)*ma) >> 11   etc */
        uint32x4_t v_ma = vsubq_u32(vdupq_n_u32(0xFF), v_a);

        uint32x4_t v_rmix = vshrq_n_u32(
                                vaddq_u32(vmulq_u32(v_sr, v_a),
                                          vmulq_u32(vshlq_n_u32(v_tr, 3), v_ma)),
                                11);
        v_tr = vpselq_u32(v_rmix, v_tr, p_mid_only);

        uint32x4_t v_gmix = vshrq_n_u32(
                                vaddq_u32(vmulq_u32(v_sg, v_a),
                                          vmulq_u32(vshlq_n_u32(v_tg, 2), v_ma)),
                                10);
        v_tg = vpselq_u32(v_gmix, v_tg, p_mid_only);

        uint32x4_t v_bmix = vshrq_n_u32(
                                vaddq_u32(vmulq_u32(v_sb, v_a),
                                          vmulq_u32(vshlq_n_u32(v_tb, 3), v_ma)),
                                11);
        v_tb = vpselq_u32(v_bmix, v_tb, p_mid_only);

        /* Pack back to BGR565 in low 16 bits of each u32 lane */
        uint32x4_t v_out = vorrq_u32(
                               vorrq_u32(vshlq_n_u32(v_tr, 11),
                                         vshlq_n_u32(v_tg, 5)),
                               v_tb);

        /* Predicated narrowing store: u32 -> u16 with tail predicate */
        vstrhq_p_u32((uint16_t *)pixels, v_out, pg);

        pixels += lanes;
        covers += lanes;
        x_cur  += (float)lanes;
        n      -= lanes;
    }
}

/* MVE specialization for BGR565 -- picks the vectorised path unconditionally.
 * Other PixelT instantiations keep the scalar primary template above. */
template <>
inline void blender_linear_gradient<pixel16_bgr565>::operator()(
    pixel16_bgr565 *pixels, int x, int y, count_t n, const cover_type *covers) const
{
    blender_lg_mve_bgr565(*this, pixels, x, y, n, covers);
}
#endif /* __ARM_FEATURE_MVE */
}  // namespace agge
