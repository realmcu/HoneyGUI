/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * The shadow rule, the direction presets, and the shadow entry point.
 *
 * A shadow is the body's own outline, blurred: solid inside, falling off
 * outside, and nothing to compensate for -- the body is composited over it
 * afterwards, which is exactly the layering a drop shadow wants.
 *
 * The solid interior is not cosmetic.  The offset slides the whole mask
 * sideways, so the strip of interior just past the body's edge ends up
 * visible, and it has to arrive at full strength; an empty interior would leave
 * a gap there and the shadow would look detached from the widget instead of
 * cast by it.  That is the one line where this rule differs from the glow's,
 * and the reason the two live in separate files.
 *
 * Everything around the rule -- walking one corner or quadrant, mirroring it,
 * filling the interior runs, the cache, the pieces, the state machine -- lives
 * in gui_geometry_effect.c and is shared with the glow.
 */

#include <string.h>
#include "gui_geometry_effect_internal.h"
#include "gui_geometry_shadow.h"

/* Same curve as the glow, without the edge-coverage division: the body
 * composites over the shadow afterwards, so the mask is the coverage itself. */
static uint8_t shadow_value(float d, float blur_px, uint32_t edge_alpha)
{
    (void)edge_alpha;   /* a transparent fill or a stroke does not change the
                         * outline that casts the shadow */

    if (!(d > 0.0f))
    {
        return 255u;
    }
    if (blur_px <= 0.0f || d >= blur_px)
    {
        return 0u;
    }

    /* (1 - d/blur)^2 in 8 fractional bits; 255 * (256 - t)^2 / 65536 is the
     * same integer expression the glow's falloff evaluates, kept spelled out
     * here so the two rules can be read and changed independently. */
    uint32_t t = (uint32_t)(d * 256.0f / blur_px);
    if (t >= 256u)
    {
        return 0u;
    }

    uint32_t u = 256u - t;
    uint32_t v = (u * u) >> 8;
    return (uint8_t)(v > 255u ? 255u : v);
}

gui_shadow_t gui_shadow_default(void)
{
    gui_shadow_t shadow;

    memset(&shadow, 0x00, sizeof(shadow));
    shadow.color = gui_rgba(0, 0, 0, 100);
    shadow.dx = 4;
    shadow.dy = 4;
    shadow.blur = 4;
    shadow.spread = 0;

    return shadow;
}

void gui_shadow_set_direction(gui_shadow_t *shadow, gui_shadow_dir_t dir, uint8_t distance)
{
    if (shadow == NULL)
    {
        return;
    }

    int32_t d = (int32_t)distance;
    /* round(distance / sqrt(2)) in integers: 46341/65536 is 1/sqrt(2) to
     * within 1e-5, and the round-to-nearest offset makes it exact for every
     * distance up to 255.  A coarser 181/256 is one low at 169 and 227. */
    int32_t diagonal = (int32_t)(((uint32_t)distance * 46341u + 32768u) >> 16);

    switch (dir)
    {
    case GUI_SHADOW_DIR_TOP_LEFT:
        shadow->dx = (int16_t)(-diagonal);
        shadow->dy = (int16_t)(-diagonal);
        break;
    case GUI_SHADOW_DIR_TOP:
        shadow->dx = 0;
        shadow->dy = (int16_t)(-d);
        break;
    case GUI_SHADOW_DIR_TOP_RIGHT:
        shadow->dx = (int16_t)diagonal;
        shadow->dy = (int16_t)(-diagonal);
        break;
    case GUI_SHADOW_DIR_LEFT:
        shadow->dx = (int16_t)(-d);
        shadow->dy = 0;
        break;
    case GUI_SHADOW_DIR_CENTER:
        shadow->dx = 0;
        shadow->dy = 0;
        break;
    case GUI_SHADOW_DIR_RIGHT:
        shadow->dx = (int16_t)d;
        shadow->dy = 0;
        break;
    case GUI_SHADOW_DIR_BOTTOM_LEFT:
        shadow->dx = (int16_t)(-diagonal);
        shadow->dy = (int16_t)diagonal;
        break;
    case GUI_SHADOW_DIR_BOTTOM:
        shadow->dx = 0;
        shadow->dy = (int16_t)d;
        break;
    case GUI_SHADOW_DIR_BOTTOM_RIGHT:
        shadow->dx = (int16_t)diagonal;
        shadow->dy = (int16_t)diagonal;
        break;
    default:
        break;   /* unknown direction keeps the offset it had */
    }
}

void gui_geometry_shadow_prepare(gui_geometry_effect_t **slot,
                                 const gui_geometry_effect_input_t *in,
                                 const gui_matrix_t *matrix,
                                 int32_t placement_x, int32_t placement_y,
                                 gui_color_t color)
{
    gui_geometry_effect_prepare(slot, in, matrix, placement_x, placement_y, color);
}

const gui_geometry_effect_rule_t *gui_geometry_shadow_rule(void)
{
    static const gui_geometry_effect_rule_t rule =
    {
        .value = shadow_value,
        .interior = 255u,
    };
    return &rule;
}
