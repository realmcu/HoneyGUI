/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Regression test: the regional glow generator against a reference one.
 *
 * The shipped generator builds a mask from one corner (rect) or one quadrant
 * (circle), then mirrors and copies.  That is fast but easy to get subtly
 * wrong: an off-by-one mirror axis, a row range that misses the edge, a
 * fractional blur that pushes a bound past the buffer.  This file keeps a plain
 * per-pixel generator that does no such cleverness, and checks the two agree
 * byte for byte.
 *
 * It is NOT part of the firmware build.  It lives under tests/ and is compiled
 * only by whoever wants to run it, because it allocates two scratch masks per
 * case and would distort both timing and peak memory if it ran in a frame.
 *
 * What a pass means: the two generators agree.  It says nothing about whether
 * the mask formula itself is right, nor about what the result looks like after
 * blitting -- those need their own checks.
 *
 * Last full run: 324 cases, 0 differences (410x502, 16 bpp simulator).
 *
 * To run it, add this file (and gui_geometry_glow.c) to a host build that can
 * reach the geometry headers, then call gui_geometry_glow_regional_test().
 */

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "gui_geometry_distance.h"
#include "gui_geometry_glow_internal.h"

/*============================================================================*
 *                         Reference generator
 *============================================================================*/

/**
 * One buffer, every pixel evaluated.  No mirrors, no copies, no bounds
 * arithmetic beyond the loop itself -- the point is that it is obviously
 * correct by inspection.
 */
static void ref_fill_rect(uint8_t *body, int w, int h, int body_w, int body_h,
                          int radius, int m, uint32_t blur_q, uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;

    for (int y = 0; y < h; y++)
    {
        uint8_t *row = body + (size_t)y * (size_t)w;

        for (int x = 0; x < w; x++)
        {
            float d = gui_geometry_rounded_rect_distance((float)x + 0.5f - (float)m,
                                                         (float)y + 0.5f - (float)m,
                                                         0.0f, 0.0f,
                                                         (float)body_w, (float)body_h,
                                                         (float)radius);
            row[x] = gui_geometry_glow_mask_value(d, blur_px, edge_alpha);
        }
    }
}

static void ref_fill_circle(uint8_t *body, int w, int h, int radius, int m,
                            uint32_t blur_q, uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;
    float c = (float)(radius + m);

    for (int y = 0; y < h; y++)
    {
        uint8_t *row = body + (size_t)y * (size_t)w;

        for (int x = 0; x < w; x++)
        {
            float dx = (float)x + 0.5f - c;
            float dy = (float)y + 0.5f - c;

            row[x] = gui_geometry_glow_mask_value(sqrtf(dx * dx + dy * dy) - (float)radius,
                                                  blur_px, edge_alpha);
        }
    }
}

/*============================================================================*
 *                         Test driver
 *============================================================================*/

typedef struct
{
    uint32_t cases;
    uint32_t failures;
} test_tally_t;

static void check_one(test_tally_t *tally, const gui_geometry_glow_input_t *in,
                      uint32_t s, uint32_t r, uint32_t b)
{
    int m = (int)gui_geometry_glow_margin(in->blur_q);
    int w = in->w + 2 * m;
    int h = in->h + 2 * m;
    int first_x = -1;
    int first_y = -1;
    int max_delta = 0;
    uint32_t diff = 0;

    if (w <= 0 || h <= 0 || (uint64_t)w * (uint64_t)h > (2u << 20))
    {
        return;
    }

    uint8_t *regional = gui_malloc((size_t)w * (size_t)h);
    uint8_t *reference = gui_malloc((size_t)w * (size_t)h);

    if (regional == NULL || reference == NULL)
    {
        gui_free(regional);
        gui_free(reference);
        return;
    }

    bool filled;

    if (in->shape == GUI_GEOMETRY_GLOW_SHAPE_CIRCLE)
    {
        filled = gui_geometry_glow_build_circle_regional(regional, w, h, in->radius, m,
                                                         in->blur_q, in->edge_alpha);
        ref_fill_circle(reference, w, h, in->radius, m, in->blur_q, in->edge_alpha);
    }
    else
    {
        filled = gui_geometry_glow_build_rect_regional(regional, w, h, in->w, in->h,
                                                       in->radius, m, in->blur_q,
                                                       in->edge_alpha);
        ref_fill_rect(reference, w, h, in->w, in->h, in->radius, m, in->blur_q,
                      in->edge_alpha);
    }

    if (!filled)
    {
        /* The generator refusing a shape is a result worth reporting, not a
         * case to skip silently. */
        tally->cases++;
        tally->failures++;
        gui_log("GLOW_REGIONAL REFUSED s=%u r=%u b=%u shape=%u w=%d h=%d R=%d blur_q=%u\n",
                (unsigned)s, (unsigned)r, (unsigned)b, (unsigned)in->shape,
                in->w, in->h, in->radius, (unsigned)in->blur_q);
        gui_free(regional);
        gui_free(reference);
        return;
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int a = regional[(size_t)y * (size_t)w + (size_t)x];
            int e = reference[(size_t)y * (size_t)w + (size_t)x];

            if (a == e)
            {
                continue;
            }

            diff++;
            if (first_x < 0)
            {
                first_x = x;
                first_y = y;
            }
            if (abs(a - e) > max_delta)
            {
                max_delta = abs(a - e);
            }
        }
    }

    tally->cases++;

    if (diff != 0u)
    {
        tally->failures++;
        gui_log("GLOW_REGIONAL FAIL s=%u r=%u b=%u shape=%u w=%d h=%d R=%d blur_q=%u "
                "alpha=%u diff_px=%u max_delta=%d first=%d,%d\n",
                (unsigned)s, (unsigned)r, (unsigned)b, (unsigned)in->shape,
                in->w, in->h, in->radius, (unsigned)in->blur_q,
                (unsigned)in->edge_alpha, (unsigned)diff, max_delta, first_x, first_y);
    }

    gui_free(regional);
    gui_free(reference);
}

/**
 * Run the regional generator against the reference over the boundary cases.
 *
 * Covers zero radius, radius equal to half the body, non-square and odd-sized
 * bodies, integer and fractional blur radii, and a part-transparent body.  The
 * fractional blurs matter: 10.125 px is the value that once made a whole row of
 * the circular mask come out empty.
 *
 * @return Number of failing cases; 0 means every case matched.
 */
uint32_t gui_geometry_glow_regional_test(void)
{
    /* uint32_t, not uint8_t: 256 is the 32 px ceiling and does not fit a byte.
     * A byte table truncates it to 0, which is not a valid blur at all. */
    static const uint32_t blurs[] = { 8, 40, 81, 84, 87, 256 };   /* 1 .. 32 px */
    static const int radii[] = { 0, 1, 2, 9, 50 };
    static const int sizes[][2] = { { 90, 90 }, { 120, 60 }, { 61, 91 } };
    test_tally_t tally = { 0, 0 };

    for (uint32_t s = 0; s < sizeof(sizes) / sizeof(sizes[0]); s++)
    {
        for (uint32_t r = 0; r < sizeof(radii) / sizeof(radii[0]); r++)
        {
            for (uint32_t b = 0; b < sizeof(blurs) / sizeof(blurs[0]); b++)
            {
                for (uint32_t shape = 0; shape < 2; shape++)
                {
                    for (uint32_t alpha = 0; alpha < 2; alpha++)
                    {
                        gui_geometry_glow_input_t in;

                        memset(&in, 0x00, sizeof(in));
                        in.shape = shape ? GUI_GEOMETRY_GLOW_SHAPE_CIRCLE
                                   : GUI_GEOMETRY_GLOW_SHAPE_RECT;
                        in.radius = radii[r];
                        in.blur_q = blurs[b];
                        in.edge_alpha = alpha ? 128u : 255u;

                        if (in.shape == GUI_GEOMETRY_GLOW_SHAPE_CIRCLE)
                        {
                            /* Same relation gui_circle.c establishes: the body
                             * is 2r across, and the buffer adds a margin on
                             * each side.  Anything else puts the centre off the
                             * mirror axis and tests a shape the widget never
                             * produces. */
                            in.w = 2 * in.radius;
                            in.h = in.w;

                            if (in.w <= 0)
                            {
                                continue;   /* rejected before either runs */
                            }
                        }
                        else
                        {
                            in.w = sizes[s][0];
                            in.h = sizes[s][1];

                            if (in.radius * 2 > _UI_MIN(in.w, in.h))
                            {
                                in.radius = _UI_MIN(in.w, in.h) / 2;
                            }
                        }

                        check_one(&tally, &in, s, r, b);
                    }
                }
            }
        }
    }

    gui_log("GLOW_REGIONAL done, %u of %u cases failed\n",
            (unsigned)tally.failures, (unsigned)tally.cases);

    return tally.failures;
}
