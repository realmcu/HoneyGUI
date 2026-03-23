/**
 * @file lane_guidance.c
 * @brief Lane guidance implementation for junction navigation
 *
 * Phase 1: Basic turn indicators and distance hints
 */

#include "lane_guidance.h"
#include "render_api.h"
#include "../core/nav_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include TRVG for vector rendering */
#include "../graphics/trvg/trvg.h"

/* ============================================================================
 * Constants
 * ============================================================================
 */

#define PI 3.14159265358979323846f
#define RAD_TO_DEG (180.0f / PI)
#define DEG_TO_RAD (PI / 180.0f)

/* Angle thresholds for maneuver classification (degrees) */
#define ANGLE_STRAIGHT_THRESHOLD    20.0f   /* -20 to 20 = straight */
#define ANGLE_SLIGHT_THRESHOLD      45.0f   /* 20-45 = slight turn */
#define ANGLE_NORMAL_THRESHOLD      120.0f  /* 45-120 = normal turn */
#define ANGLE_SHARP_THRESHOLD       160.0f  /* 120-160 = sharp turn */
/* > 160 = U-turn */

/* Minimum segment length to calculate angle (meters) */
#define MIN_SEGMENT_LENGTH          5.0f

/* ============================================================================
 * Maneuver Names (for debugging)
 * ============================================================================
 */

static const char *maneuver_names[] =
{
    "None",
    "Straight",
    "Left",
    "Right",
    "Slight Left",
    "Slight Right",
    "Sharp Left",
    "Sharp Right",
    "U-Turn Left",
    "U-Turn Right",
    "Ramp Left",
    "Ramp Right",
    "Merge",
    "Arrive"
};

/* ============================================================================
 * Helper Functions
 * ============================================================================
 */

/**
 * @brief Calculate distance between two coordinates (Haversine formula)
 */
static float calc_distance(float lat1, float lon1, float lat2, float lon2)
{
    float dlat = (lat2 - lat1) * DEG_TO_RAD;
    float dlon = (lon2 - lon1) * DEG_TO_RAD;
    float a = sinf(dlat / 2) * sinf(dlat / 2) +
              cosf(lat1 * DEG_TO_RAD) * cosf(lat2 * DEG_TO_RAD) *
              sinf(dlon / 2) * sinf(dlon / 2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1 - a));
    return 6371000.0f * c;  /* Earth radius in meters */
}

/**
 * @brief Calculate bearing from point 1 to point 2
 */
static float calc_bearing(float lat1, float lon1, float lat2, float lon2)
{
    float dlon = (lon2 - lon1) * DEG_TO_RAD;
    float lat1_rad = lat1 * DEG_TO_RAD;
    float lat2_rad = lat2 * DEG_TO_RAD;

    float x = sinf(dlon) * cosf(lat2_rad);
    float y = cosf(lat1_rad) * sinf(lat2_rad) -
              sinf(lat1_rad) * cosf(lat2_rad) * cosf(dlon);

    float bearing = atan2f(x, y) * RAD_TO_DEG;

    /* Normalize to 0-360 */
    while (bearing < 0)
    {
        bearing += 360.0f;
    }
    while (bearing >= 360.0f)
    {
        bearing -= 360.0f;
    }

    return bearing;
}

/* ============================================================================
 * Guidance State Functions
 * ============================================================================
 */

map_error_t guidance_init(guidance_state_t *state, const path_t *path)
{
    if (!state || !path)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    if (path->count < 2)
    {
        return MAP_ERR_NO_PATH;
    }

    state->path = path;
    state->current_segment = 0;
    state->traveled_distance = 0.0f;

    return MAP_OK;
}

map_error_t guidance_update(guidance_state_t *state,
                            float current_lat, float current_lon)
{
    if (!state || !state->path)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    const path_t *path = state->path;

    /* Find the closest point on path and update segment index */
    float min_dist = 1e10f;
    uint32_t best_segment = state->current_segment;

    /* Search from current segment forward (don't go backwards) */
    for (uint32_t i = state->current_segment; i < path->count - 1; i++)
    {
        const coord_t *p1 = &path->points[i];
        const coord_t *p2 = &path->points[i + 1];

        /* Simple point-to-segment distance approximation */
        float dist_to_p1 = calc_distance(current_lat, current_lon, p1->lat, p1->lon);
        float dist_to_p2 = calc_distance(current_lat, current_lon, p2->lat, p2->lon);
        float min_seg_dist = (dist_to_p1 < dist_to_p2) ? dist_to_p1 : dist_to_p2;

        if (min_seg_dist < min_dist)
        {
            min_dist = min_seg_dist;
            best_segment = i;
        }

        /* If we're getting further from the path, stop searching */
        if (min_seg_dist > min_dist * 2.0f && i > state->current_segment + 5)
        {
            break;
        }
    }

    state->current_segment = best_segment;

    /* Calculate traveled distance on current segment */
    const coord_t *seg_start = &path->points[best_segment];
    state->traveled_distance = calc_distance(seg_start->lat, seg_start->lon,
                                             current_lat, current_lon);

    return MAP_OK;
}

/* ============================================================================
 * Turn Angle Calculation
 * ============================================================================
 */

float guidance_calculate_turn_angle(const coord_t *p1,
                                    const coord_t *p2,
                                    const coord_t *p3)
{
    if (!p1 || !p2 || !p3)
    {
        return 0.0f;
    }

    /* Calculate bearing of incoming segment */
    float bearing1 = calc_bearing(p1->lat, p1->lon, p2->lat, p2->lon);

    /* Calculate bearing of outgoing segment */
    float bearing2 = calc_bearing(p2->lat, p2->lon, p3->lat, p3->lon);

    /* Calculate turn angle (positive = right turn, negative = left turn) */
    float angle = bearing2 - bearing1;

    /* Normalize to -180 to 180 */
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }
    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}

maneuver_type_t guidance_angle_to_maneuver(float angle)
{
    float abs_angle = fabsf(angle);

    if (abs_angle < ANGLE_STRAIGHT_THRESHOLD)
    {
        return MANEUVER_STRAIGHT;
    }
    else if (abs_angle < ANGLE_SLIGHT_THRESHOLD)
    {
        return (angle > 0) ? MANEUVER_SLIGHT_RIGHT : MANEUVER_SLIGHT_LEFT;
    }
    else if (abs_angle < ANGLE_NORMAL_THRESHOLD)
    {
        return (angle > 0) ? MANEUVER_RIGHT : MANEUVER_LEFT;
    }
    else if (abs_angle < ANGLE_SHARP_THRESHOLD)
    {
        return (angle > 0) ? MANEUVER_SHARP_RIGHT : MANEUVER_SHARP_LEFT;
    }
    else
    {
        return (angle > 0) ? MANEUVER_UTURN_RIGHT : MANEUVER_UTURN_LEFT;
    }
}

const char *guidance_maneuver_name(maneuver_type_t maneuver)
{
    if (maneuver >= 0 && maneuver < MANEUVER_COUNT)
    {
        return maneuver_names[maneuver];
    }
    return "Unknown";
}

/* ============================================================================
 * Junction Guidance Calculation
 * ============================================================================
 */

/**
 * @brief Check if a path point is a significant turn (junction)
 */
static int is_significant_turn(const path_t *path, uint32_t index)
{
    if (index < 1 || index >= path->count - 1)
    {
        return 0;
    }

    const coord_t *p1 = &path->points[index - 1];
    const coord_t *p2 = &path->points[index];
    const coord_t *p3 = &path->points[index + 1];

    /* Check segment lengths */
    float dist1 = calc_distance(p1->lat, p1->lon, p2->lat, p2->lon);
    float dist2 = calc_distance(p2->lat, p2->lon, p3->lat, p3->lon);

    if (dist1 < MIN_SEGMENT_LENGTH || dist2 < MIN_SEGMENT_LENGTH)
    {
        return 0;
    }

    /* Calculate turn angle */
    float angle = guidance_calculate_turn_angle(p1, p2, p3);

    /* Consider it a junction if turn angle is significant */
    return fabsf(angle) > ANGLE_STRAIGHT_THRESHOLD;
}

map_error_t guidance_get_next_junction(const guidance_state_t *state,
                                       junction_guidance_t *guidance)
{
    if (!state || !state->path || !guidance)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    const path_t *path = state->path;

    /* Initialize output */
    memset(guidance, 0, sizeof(junction_guidance_t));

    /* Check if we've reached the destination */
    if (state->current_segment >= path->count - 2)
    {
        guidance->maneuver = MANEUVER_ARRIVE;
        guidance->distance = calc_distance(
                                 path->points[state->current_segment].lat,
                                 path->points[state->current_segment].lon,
                                 path->points[path->count - 1].lat,
                                 path->points[path->count - 1].lon
                             );
        return MAP_OK;
    }

    /* Search for next significant turn */
    float accumulated_distance = 0.0f;

    /* Start from current position */
    const coord_t *prev_point = &path->points[state->current_segment];

    for (uint32_t i = state->current_segment + 1; i < path->count - 1; i++)
    {
        const coord_t *curr_point = &path->points[i];

        /* Accumulate distance */
        accumulated_distance += calc_distance(prev_point->lat, prev_point->lon,
                                              curr_point->lat, curr_point->lon);

        /* Check if this is a significant turn */
        if (is_significant_turn(path, i))
        {
            const coord_t *p1 = &path->points[i - 1];
            const coord_t *p2 = &path->points[i];
            const coord_t *p3 = &path->points[i + 1];

            float angle = guidance_calculate_turn_angle(p1, p2, p3);

            guidance->distance = accumulated_distance - state->traveled_distance;
            guidance->turn_angle = angle;
            guidance->maneuver = guidance_angle_to_maneuver(angle);
            guidance->segment_index = i;

            return MAP_OK;
        }

        prev_point = curr_point;
    }

    /* No significant turn found, show straight to destination */
    const coord_t *last_point = &path->points[path->count - 1];
    accumulated_distance += calc_distance(prev_point->lat, prev_point->lon,
                                          last_point->lat, last_point->lon);

    guidance->distance = accumulated_distance - state->traveled_distance;
    guidance->maneuver = MANEUVER_ARRIVE;
    guidance->segment_index = path->count - 1;

    return MAP_OK;
}

/* ============================================================================
 * Distance Formatting
 * ============================================================================
 */

void guidance_format_distance(float distance, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size < 1)
    {
        return;
    }

    if (distance < 0)
    {
        snprintf(buffer, buffer_size, "0 m");
    }
    else if (distance < 100)
    {
        /* Round to nearest 10m for short distances */
        int meters = ((int)(distance / 10.0f + 0.5f)) * 10;
        if (meters < 10)
        {
            meters = 10;
        }
        snprintf(buffer, buffer_size, "%d m", meters);
    }
    else if (distance < 1000)
    {
        /* Round to nearest 50m */
        int meters = ((int)(distance / 50.0f + 0.5f)) * 50;
        snprintf(buffer, buffer_size, "%d m", meters);
    }
    else if (distance < 10000)
    {
        /* Show with one decimal for 1-10km */
        float km = distance / 1000.0f;
        snprintf(buffer, buffer_size, "%.1f km", (double)km);
    }
    else
    {
        /* Round to nearest km for > 10km */
        int km = (int)(distance / 1000.0f + 0.5f);
        snprintf(buffer, buffer_size, "%d km", km);
    }
}

/* ============================================================================
 * Guidance Style
 * ============================================================================
 */

void guidance_style_init(guidance_style_t *style)
{
    if (!style)
    {
        return;
    }

    /* Default colors - dark semi-transparent panel */
    style->background_color = 0x000000C0;   /* Black with 75% opacity */
    style->arrow_color = 0xFFFFFFFF;        /* White */
    style->text_color = 0xFFFFFFFF;         /* White */
    style->highlight_color = 0x4CAF50FF;    /* Green accent */

    /* Default sizes */
    style->arrow_size = 48.0f;
    style->font_size = 20.0f;
    style->panel_padding = 12.0f;
    style->corner_radius = 8.0f;
}

void guidance_style_init_preset(guidance_style_t *style, guidance_style_preset_t preset)
{
    if (!style)
    {
        return;
    }

    /* Initialize with default sizes first */
    style->arrow_size = 48.0f;
    style->font_size = 20.0f;
    style->panel_padding = 12.0f;
    style->corner_radius = 8.0f;
    switch (preset)
    {
    case GUIDANCE_STYLE_GOOGLE:
        /* Google Maps style - modern dark panel with blue highlight */
        style->background_color = 0x2D2D2DE8;   /* Dark gray with high opacity */
        style->arrow_color = 0xFFFFFFFF;        /* White arrow */
        style->text_color = 0xFFFFFFFF;         /* White text */
        style->highlight_color = 0x4285F4FF;    /* Google Blue accent */
        break;

    case GUIDANCE_STYLE_DARK:
        /* Dark mode - dark panel with bright arrow */
        style->background_color = 0x1A1A2EE8;   /* Dark blue-black with high opacity */
        style->arrow_color = 0x4A90D9FF;        /* Bright blue arrow */
        style->text_color = 0xE0E0E0FF;         /* Light gray text */
        style->highlight_color = 0x4A90D9FF;    /* Blue accent */
        break;

    case GUIDANCE_STYLE_AMAP:
        /* AMap (Gaode) style - blue navigation panel */
        style->background_color = 0x3385FFE8;   /* AMap Blue with high opacity */
        style->arrow_color = 0xFFFFFFFF;        /* White arrow */
        style->text_color = 0xFFFFFFFF;         /* White text */
        style->highlight_color = 0xFFCC00FF;    /* Yellow accent */
        break;

    case GUIDANCE_STYLE_BAIDU:
        /* Baidu Maps style - red-orange navigation panel */
        style->background_color = 0x3385FFE8;   /* Baidu Blue with high opacity */
        style->arrow_color = 0xFFFFFFFF;        /* White arrow */
        style->text_color = 0xFFFFFFFF;         /* White text */
        style->highlight_color = 0xFF6600FF;    /* Orange accent */
        break;

    case GUIDANCE_STYLE_DEFAULT:
    default:
        /* Default dark semi-transparent panel */
        style->background_color = 0x000000C0;   /* Black with 75% opacity */
        style->arrow_color = 0xFFFFFFFF;        /* White */
        style->text_color = 0xFFFFFFFF;         /* White */
        style->highlight_color = 0x4CAF50FF;    /* Green accent */
        break;
    }
}

void guidance_style_init_from_theme(guidance_style_t *style, const map_theme_t *map_theme)
{
    if (!style)
    {
        return;
    }

    /* Initialize with default sizes */
    style->arrow_size = 48.0f;
    style->font_size = 20.0f;
    style->panel_padding = 12.0f;
    style->corner_radius = 8.0f;

    if (!map_theme)
    {
        /* Fall back to default style if no theme provided */
        guidance_style_init(style);
        return;
    }

    /* Determine if the theme is dark or light based on background color */
    uint32_t bg = map_theme->background;
    uint8_t bg_r = (bg >> 24) & 0xFF;
    uint8_t bg_g = (bg >> 16) & 0xFF;
    uint8_t bg_b = (bg >> 8) & 0xFF;

    /* Calculate luminance (simplified) */
    int luminance = (bg_r * 299 + bg_g * 587 + bg_b * 114) / 1000;
    int is_dark_theme = (luminance < 128);

    if (is_dark_theme)
    {
        /* Dark theme: use lighter panel with dark arrow */
        style->background_color = 0x3D3D56E8;       /* Semi-transparent dark gray */
        style->arrow_color = 0x4A90D9FF;            /* Bright blue arrow */
        style->text_color = 0xE0E0E0FF;             /* Light gray text */
        style->highlight_color = map_theme->road_motorway; /* Use motorway color as accent */
    }
    else
    {
        /* Light theme (Google-like): green panel with white arrow */
        style->background_color = 0x34A853E8;       /* Google Green with high opacity */
        style->arrow_color = 0xFFFFFFFF;            /* White arrow */
        style->text_color = 0xFFFFFFFF;             /* White text */
        /* Use a complementary color to the road colors for accent */
        style->highlight_color = 0x4285F4FF;        /* Google Blue accent */
    }
}

/* ============================================================================
 * TRVG Vector Rendering Helpers
 * ============================================================================
 */

/**
 * @brief Get TRVG context from renderer
 */
#define GET_TRVG(renderer) (&(renderer)->trvg)

/**
 * @brief Convert RGBA color (0xRRGGBBAA) to TRVG color
 */
static trvg_color_t color_to_trvg(uint32_t color)
{
    unsigned char r = (color >> 24) & 0xFF;
    unsigned char g = (color >> 16) & 0xFF;
    unsigned char b = (color >> 8) & 0xFF;
    unsigned char a = color & 0xFF;
    return trvg_rgba(r, g, b, a);
}

/* ============================================================================
 * Arrow Icon Rendering (TRVG Vector Graphics)
 * ============================================================================
 */

/**
 * @brief Draw straight arrow (up arrow) using TRVG fast API
 */
static void draw_arrow_straight(trvg_context_t *ctx, float cx, float cy,
                                float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.22f;
    float head_w = size * 0.45f;
    float head_h = size * 0.4f;

    /* Arrow shape: 7 points */
    int16_t points[14] =
    {
        (int16_t)cx, (int16_t)(cy - half),                              /* Top tip */
        (int16_t)(cx + head_w), (int16_t)(cy - half + head_h),          /* Right of head */
        (int16_t)(cx + shaft_w / 2), (int16_t)(cy - half + head_h),     /* Right shaft top */
        (int16_t)(cx + shaft_w / 2), (int16_t)(cy + half * 0.6f),       /* Right shaft bottom */
        (int16_t)(cx - shaft_w / 2), (int16_t)(cy + half * 0.6f),       /* Left shaft bottom */
        (int16_t)(cx - shaft_w / 2), (int16_t)(cy - half + head_h),     /* Left shaft top */
        (int16_t)(cx - head_w), (int16_t)(cy - half + head_h)           /* Left of head */
    };

    trvg_fast_fill_polygon(ctx, points, 7, color.r, color.g, color.b);
}

/**
 * @brief Draw left turn arrow using TRVG fast API
 */
static void draw_arrow_left(trvg_context_t *ctx, float cx, float cy,
                            float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.18f;
    float head_w = size * 0.4f;

    /* Arrow pointing left with vertical stem */
    float corner_y = cy - half * 0.35f;  /* Where the arrow bends */
    float arrow_tip_x = cx - half * 0.85f;

    /* 9 points */
    int16_t points[18] =
    {
        (int16_t)(cx + shaft_w / 2), (int16_t)(cy + half * 0.7f),       /* Bottom right of shaft */
        (int16_t)(cx + shaft_w / 2), (int16_t)corner_y,                 /* Up to corner */
        (int16_t)(cx - half * 0.4f), (int16_t)corner_y,                 /* Horizontal to left */
        (int16_t)(cx - half * 0.4f), (int16_t)(corner_y - head_w),      /* Top of arrow back */
        (int16_t)arrow_tip_x, (int16_t)corner_y,                        /* Arrow tip */
        (int16_t)(cx - half * 0.4f), (int16_t)(corner_y + head_w),      /* Bottom of arrow back */
        (int16_t)(cx - half * 0.4f), (int16_t)(corner_y + shaft_w),     /* Back to shaft level */
        (int16_t)(cx - shaft_w / 2), (int16_t)(corner_y + shaft_w),     /* Inner corner */
        (int16_t)(cx - shaft_w / 2), (int16_t)(cy + half * 0.7f)        /* Bottom left of shaft */
    };

    trvg_fast_fill_polygon(ctx, points, 9, color.r, color.g, color.b);
}

/**
 * @brief Draw right turn arrow using TRVG fast API
 */
static void draw_arrow_right(trvg_context_t *ctx, float cx, float cy,
                             float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.18f;
    float head_w = size * 0.4f;

    /* Arrow pointing right with vertical stem */
    float corner_y = cy - half * 0.35f;
    float arrow_tip_x = cx + half * 0.85f;

    /* 9 points */
    int16_t points[18] =
    {
        (int16_t)(cx - shaft_w / 2), (int16_t)(cy + half * 0.7f),       /* Bottom left of shaft */
        (int16_t)(cx - shaft_w / 2), (int16_t)corner_y,                 /* Up to corner */
        (int16_t)(cx + half * 0.4f), (int16_t)corner_y,                 /* Horizontal to right */
        (int16_t)(cx + half * 0.4f), (int16_t)(corner_y - head_w),      /* Top of arrow back */
        (int16_t)arrow_tip_x, (int16_t)corner_y,                        /* Arrow tip */
        (int16_t)(cx + half * 0.4f), (int16_t)(corner_y + head_w),      /* Bottom of arrow back */
        (int16_t)(cx + half * 0.4f), (int16_t)(corner_y + shaft_w),     /* Back to shaft level */
        (int16_t)(cx + shaft_w / 2), (int16_t)(corner_y + shaft_w),     /* Inner corner */
        (int16_t)(cx + shaft_w / 2), (int16_t)(cy + half * 0.7f)        /* Bottom right of shaft */
    };

    trvg_fast_fill_polygon(ctx, points, 9, color.r, color.g, color.b);
}

/**
 * @brief Draw slight left arrow (diagonal up-left) using TRVG fast API
 * Arrow pointing up and slightly to the left (like a fork road going left)
 */
static void draw_arrow_slight_left(trvg_context_t *ctx, float cx, float cy,
                                   float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.20f;
    float head_w = size * 0.40f;
    float head_h = size * 0.35f;

    /* Arrow tip position (up and to the left, about 30 degrees) */
    float tip_x = cx - half * 0.45f;
    float tip_y = cy - half * 0.85f;

    /* Shaft base (bottom center) */
    float base_x = cx;
    float base_y = cy + half * 0.6f;

    /* Direction from base to tip */
    float dir_x = tip_x - base_x;
    float dir_y = tip_y - base_y;
    float len = sqrtf(dir_x * dir_x + dir_y * dir_y);
    dir_x /= len;
    dir_y /= len;

    /* Perpendicular for width */
    float perp_x = -dir_y;
    float perp_y = dir_x;

    /* Arrow head base position */
    float head_base_x = tip_x - dir_x * head_h;
    float head_base_y = tip_y - dir_y * head_h;

    /* 7 points for arrow shape */
    int16_t points[14] =
    {
        (int16_t)tip_x, (int16_t)tip_y,                                         /* Tip */
        (int16_t)(head_base_x + perp_x * head_w), (int16_t)(head_base_y + perp_y * head_w),  /* Right wing */
        (int16_t)(head_base_x + perp_x *shaft_w / 2), (int16_t)(head_base_y + perp_y *shaft_w / 2), /* Right shaft top */
        (int16_t)(base_x + perp_x *shaft_w / 2), (int16_t)(base_y + perp_y *shaft_w / 2), /* Right shaft bottom */
        (int16_t)(base_x - perp_x *shaft_w / 2), (int16_t)(base_y - perp_y *shaft_w / 2), /* Left shaft bottom */
        (int16_t)(head_base_x - perp_x *shaft_w / 2), (int16_t)(head_base_y - perp_y *shaft_w / 2), /* Left shaft top */
        (int16_t)(head_base_x - perp_x * head_w), (int16_t)(head_base_y - perp_y * head_w)   /* Left wing */
    };

    trvg_fast_fill_polygon(ctx, points, 7, color.r, color.g, color.b);
}

/**
 * @brief Draw slight right arrow (diagonal up-right) using TRVG fast API
 * Arrow pointing up and slightly to the right (like a fork road going right)
 */
static void draw_arrow_slight_right(trvg_context_t *ctx, float cx, float cy,
                                    float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.20f;
    float head_w = size * 0.40f;
    float head_h = size * 0.35f;

    /* Arrow tip position (up and to the right, about 30 degrees) */
    float tip_x = cx + half * 0.45f;
    float tip_y = cy - half * 0.85f;

    /* Shaft base (bottom center) */
    float base_x = cx;
    float base_y = cy + half * 0.6f;

    /* Direction from base to tip */
    float dir_x = tip_x - base_x;
    float dir_y = tip_y - base_y;
    float len = sqrtf(dir_x * dir_x + dir_y * dir_y);
    dir_x /= len;
    dir_y /= len;

    /* Perpendicular for width */
    float perp_x = -dir_y;
    float perp_y = dir_x;

    /* Arrow head base position */
    float head_base_x = tip_x - dir_x * head_h;
    float head_base_y = tip_y - dir_y * head_h;

    /* 7 points for arrow shape */
    int16_t points[14] =
    {
        (int16_t)tip_x, (int16_t)tip_y,                                         /* Tip */
        (int16_t)(head_base_x + perp_x * head_w), (int16_t)(head_base_y + perp_y * head_w),  /* Right wing */
        (int16_t)(head_base_x + perp_x *shaft_w / 2), (int16_t)(head_base_y + perp_y *shaft_w / 2), /* Right shaft top */
        (int16_t)(base_x + perp_x *shaft_w / 2), (int16_t)(base_y + perp_y *shaft_w / 2), /* Right shaft bottom */
        (int16_t)(base_x - perp_x *shaft_w / 2), (int16_t)(base_y - perp_y *shaft_w / 2), /* Left shaft bottom */
        (int16_t)(head_base_x - perp_x *shaft_w / 2), (int16_t)(head_base_y - perp_y *shaft_w / 2), /* Left shaft top */
        (int16_t)(head_base_x - perp_x * head_w), (int16_t)(head_base_y - perp_y * head_w)   /* Left wing */
    };
    trvg_fast_fill_polygon(ctx, points, 7, color.r, color.g, color.b);
}

/**
 * @brief Draw sharp left arrow using TRVG fast API
 * Similar to left arrow but pointing more backwards (135 degree turn)
 */
static void draw_arrow_sharp_left(trvg_context_t *ctx, float cx, float cy,
                                  float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.18f;
    float head_w = size * 0.38f;

    /* Vertical shaft going up, then horizontal going left, arrow pointing down-left */
    float top_y = cy - half * 0.55f;          /* Top of vertical shaft */
    float corner_y = cy - half * 0.05f;       /* Where shaft bends horizontally */
    float horiz_left = cx - half * 0.3f;      /* Where horizontal part ends */
    float arrow_tip_x = cx - half * 0.85f;    /* Arrow tip X (pointing left-down) */
    float arrow_tip_y = cy + half * 0.45f;    /* Arrow tip Y */

    /* 9 points */
    int16_t points[18] =
    {
        /* Top right of vertical shaft */
        (int16_t)(cx + shaft_w / 2), (int16_t)top_y,
        /* Down to corner, outer edge */
        (int16_t)(cx + shaft_w / 2), (int16_t)corner_y,
        /* Horizontal to left, then to arrow head */
        (int16_t)horiz_left, (int16_t)(arrow_tip_y - head_w),
        /* Arrow tip */
        (int16_t)arrow_tip_x, (int16_t)arrow_tip_y,
        /* Bottom of arrow head */
        (int16_t)horiz_left, (int16_t)(arrow_tip_y + head_w),
        /* Back along inner path */
        (int16_t)(cx - shaft_w / 2), (int16_t)(corner_y + shaft_w),
        /* Up the inner edge of shaft */
        (int16_t)(cx - shaft_w / 2), (int16_t)top_y,
        /* Close */
        (int16_t)(cx + shaft_w / 2), (int16_t)top_y,
        (int16_t)(cx + shaft_w / 2), (int16_t)top_y
    };

    trvg_fast_fill_polygon(ctx, points, 7, color.r, color.g, color.b);
}

/**
 * @brief Draw sharp right arrow using TRVG fast API
 * Similar to right arrow but pointing more backwards (135 degree turn)
 */
static void draw_arrow_sharp_right(trvg_context_t *ctx, float cx, float cy,
                                   float size, trvg_color_t color)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.18f;
    float head_w = size * 0.38f;

    /* Vertical shaft going up, then horizontal going right, arrow pointing down-right */
    float top_y = cy - half * 0.55f;          /* Top of vertical shaft */
    float corner_y = cy - half * 0.05f;       /* Where shaft bends horizontally */
    float horiz_right = cx + half * 0.3f;     /* Where horizontal part ends */
    float arrow_tip_x = cx + half * 0.85f;    /* Arrow tip X (pointing right-down) */
    float arrow_tip_y = cy + half * 0.45f;    /* Arrow tip Y */

    /* 7 points */
    int16_t points[14] =
    {
        /* Top left of vertical shaft */
        (int16_t)(cx - shaft_w / 2), (int16_t)top_y,
        /* Down to corner, outer edge */
        (int16_t)(cx - shaft_w / 2), (int16_t)corner_y,
        /* Horizontal to right, then to arrow head */
        (int16_t)horiz_right, (int16_t)(arrow_tip_y - head_w),
        /* Arrow tip */
        (int16_t)arrow_tip_x, (int16_t)arrow_tip_y,
        /* Bottom of arrow head */
        (int16_t)horiz_right, (int16_t)(arrow_tip_y + head_w),
        /* Back along inner path */
        (int16_t)(cx + shaft_w / 2), (int16_t)(corner_y + shaft_w),
        /* Up the inner edge of shaft */
        (int16_t)(cx + shaft_w / 2), (int16_t)top_y
    };
    trvg_fast_fill_polygon(ctx, points, 7, color.r, color.g, color.b);
}

/**
 * @brief Draw U-turn arrow using TRVG fast API
 * Simplified version without curves
 */
static void draw_arrow_uturn(trvg_context_t *ctx, float cx, float cy,
                             float size, trvg_color_t color, int left)
{
    float half = size / 2.0f;
    float shaft_w = size * 0.14f;
    float head_w = size * 0.30f;
    float arc_radius = half * 0.40f;

    float dir = left ? -1.0f : 1.0f;

    /* Key positions */
    float up_x = cx + dir * arc_radius;      /* Rising shaft X */
    float down_x = cx - dir * arc_radius;    /* Descending shaft X (with arrow) */
    float arc_center_y = cy - half * 0.35f;  /* Y center of the arc */
    float bottom_y = cy + half * 0.65f;      /* Bottom of rising shaft */
    float arrow_tip_y = cy + half * 0.70f;   /* Arrow tip Y */
    float arrow_base_y = cy + half * 0.25f;  /* Where arrow head meets shaft */
    float arc_top_y = arc_center_y - arc_radius - shaft_w / 2;

    /* U-turn shape - simplified with straight lines at top instead of curves */
    /* 12 points */
    int16_t points[24] =
    {
        /* Start from bottom of rising shaft (outer edge) */
        (int16_t)(up_x + dir *shaft_w / 2), (int16_t)bottom_y,
        /* Go up the outer edge of rising shaft */
        (int16_t)(up_x + dir *shaft_w / 2), (int16_t)arc_center_y,
        /* Top corner outer */
        (int16_t)(up_x + dir *shaft_w / 2), (int16_t)arc_top_y,
        (int16_t)(down_x - dir *shaft_w / 2), (int16_t)arc_top_y,
        /* Down the outer edge of descending shaft to arrow head */
        (int16_t)(down_x - dir *shaft_w / 2), (int16_t)arrow_base_y,
        /* Arrow head - left wing */
        (int16_t)(down_x - dir * head_w), (int16_t)arrow_base_y,
        /* Arrow tip */
        (int16_t)down_x, (int16_t)arrow_tip_y,
        /* Arrow head - right wing */
        (int16_t)(down_x + dir * head_w), (int16_t)arrow_base_y,
        /* Back up the inner edge of descending shaft */
        (int16_t)(down_x + dir *shaft_w / 2), (int16_t)arrow_base_y,
        (int16_t)(down_x + dir *shaft_w / 2), (int16_t)(arc_top_y + shaft_w),
        /* Inner top corner */
        (int16_t)(up_x - dir *shaft_w / 2), (int16_t)(arc_top_y + shaft_w),
        /* Down the inner edge of rising shaft */
        (int16_t)(up_x - dir *shaft_w / 2), (int16_t)bottom_y
    };

    trvg_fast_fill_polygon(ctx, points, 12, color.r, color.g, color.b);
}

/**
 * @brief Draw destination/arrive marker using TRVG fast API
 * Simplified to a circle with an inner circle (no teardrop shape)
 */
static void draw_arrive_marker(trvg_context_t *ctx, float cx, float cy,
                               float size, trvg_color_t color)
{
    float half = size / 2.0f;

    /* Draw outer circle */
    int radius = (int)(half * 0.5f);
    trvg_fast_fill_circle(ctx, (int)cx, (int)cy, radius, color.r, color.g, color.b);

    /* Draw inner circle (hole in pin) with dark color */
    int inner_radius = (int)(radius * 0.4f);
    if (inner_radius > 0)
    {
        trvg_fast_fill_circle(ctx, (int)cx, (int)cy, inner_radius, 40, 40, 40);
    }
}

/**
 * @brief Draw rounded rectangle using TRVG fast API
 * Simplified to a regular rectangle (no rounded corners)
 */
static void draw_rounded_rect_trvg(trvg_context_t *ctx, float x, float y,
                                   float w, float h, float r, trvg_color_t color)
{
    (void)r;  /* Ignore radius - using simple rectangle */
    trvg_fast_fill_rect(ctx, (int)x, (int)y, (int)w, (int)h, color.r, color.g, color.b);
}

/* ============================================================================
 * Public Rendering Functions
 * ============================================================================
 */

void render_turn_arrow(renderer_t *renderer,
                       maneuver_type_t maneuver,
                       uint32_t color,
                       float cx, float cy, float size)
{
    if (!renderer)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);
    trvg_color_t trvg_color = color_to_trvg(color);

    switch (maneuver)
    {
    case MANEUVER_STRAIGHT:
        draw_arrow_straight(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_LEFT:
        draw_arrow_left(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_SHARP_LEFT:
        draw_arrow_sharp_left(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_RIGHT:
        draw_arrow_right(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_SHARP_RIGHT:
        draw_arrow_sharp_right(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_SLIGHT_LEFT:
        draw_arrow_slight_left(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_SLIGHT_RIGHT:
        draw_arrow_slight_right(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_UTURN_LEFT:
        draw_arrow_uturn(ctx, cx, cy, size, trvg_color, 1);
        break;

    case MANEUVER_UTURN_RIGHT:
        draw_arrow_uturn(ctx, cx, cy, size, trvg_color, 0);
        break;

    case MANEUVER_ARRIVE:
        draw_arrive_marker(ctx, cx, cy, size, trvg_color);
        break;

    case MANEUVER_NONE:
    default:
        draw_arrow_straight(ctx, cx, cy, size, trvg_color);
        break;
    }
}

void render_lane_guidance(renderer_t *renderer,
                          const junction_guidance_t *guidance,
                          const guidance_style_t *style,
                          float x, float y,
                          float width, float height)
{
    if (!renderer || !guidance || !style)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);

    /* Draw background panel with rounded corners */
    draw_rounded_rect_trvg(ctx, x, y, width, height,
                           style->corner_radius, color_to_trvg(style->background_color));

    /* Calculate layout */
    float padding = style->panel_padding;
    float arrow_size = style->arrow_size;

    /* Arrow position (centered in upper portion) */
    float arrow_cx = x + width / 2.0f;
    float arrow_cy = y + padding + arrow_size / 2.0f;

    /* Draw turn arrow */
    render_turn_arrow(renderer, guidance->maneuver, style->arrow_color,
                      arrow_cx, arrow_cy, arrow_size);

    /* Format distance text */
    char dist_text[32];
    guidance_format_distance(guidance->distance, dist_text, sizeof(dist_text));
    /* Draw distance text below arrow using render_text from render_api */
    float text_y = y + padding + arrow_size + padding * 0.5f;
    float text_x = x + width / 2.0f;

    /* Use render_text to draw actual text */
    render_text(renderer, text_x, text_y, dist_text,
                style->font_size, style->text_color);
}

/* ============================================================================
 * Phase 2: Multi-Lane Display Implementation
 * ============================================================================
 */

/**
 * @brief Convert maneuver type to lane direction bitmask
 */
uint8_t maneuver_to_lane_direction(maneuver_type_t maneuver)
{
    switch (maneuver)
    {
    case MANEUVER_STRAIGHT:
        return LANE_DIR_STRAIGHT;
    case MANEUVER_LEFT:
        return LANE_DIR_LEFT;
    case MANEUVER_RIGHT:
        return LANE_DIR_RIGHT;
    case MANEUVER_SLIGHT_LEFT:
        return LANE_DIR_SLIGHT_LEFT;
    case MANEUVER_SLIGHT_RIGHT:
        return LANE_DIR_SLIGHT_RIGHT;
    case MANEUVER_SHARP_LEFT:
        return LANE_DIR_SHARP_LEFT;
    case MANEUVER_SHARP_RIGHT:
        return LANE_DIR_SHARP_RIGHT;
    case MANEUVER_UTURN_LEFT:
    case MANEUVER_UTURN_RIGHT:
        return LANE_DIR_UTURN;
    case MANEUVER_RAMP_LEFT:
        return LANE_DIR_SLIGHT_LEFT | LANE_DIR_LEFT;
    case MANEUVER_RAMP_RIGHT:
        return LANE_DIR_SLIGHT_RIGHT | LANE_DIR_RIGHT;
    case MANEUVER_NONE:
    case MANEUVER_MERGE:
    case MANEUVER_ARRIVE:
    default:
        return LANE_DIR_STRAIGHT;
    }
}

/**
 * @brief Calculate recommended lane based on next maneuver
 */
uint8_t guidance_calculate_recommended_lane(const lane_config_t *lane_config,
                                            maneuver_type_t next_maneuver)
{
    if (!lane_config || lane_config->lane_count == 0)
    {
        return 255;  /* No recommendation */
    }

    /* Convert maneuver to required lane direction */
    uint8_t required_dir = maneuver_to_lane_direction(next_maneuver);

    /* Find a lane that allows this direction */
    int best_lane = -1;
    int best_score = 0;

    for (int i = 0; i < lane_config->lane_count; i++)
    {
        uint8_t lane_dirs = lane_config->lanes[i].directions;

        /* Check if this lane allows the required direction */
        if (lane_dirs & required_dir)
        {
            int score = 10;

            /* Penalty for lanes with multiple directions */
            int dir_count = 0;
            for (int b = 0; b < 8; b++)
            {
                if (lane_dirs & (1 << b))
                {
                    dir_count++;
                }
            }
            score -= dir_count - 1;

            /* For left turns, prefer leftmost lanes */
            if (required_dir & (LANE_DIR_LEFT | LANE_DIR_SLIGHT_LEFT | LANE_DIR_SHARP_LEFT | LANE_DIR_UTURN))
            {
                score += i;
            }
            /* For right turns, prefer rightmost lanes */
            else if (required_dir & (LANE_DIR_RIGHT | LANE_DIR_SLIGHT_RIGHT | LANE_DIR_SHARP_RIGHT))
            {
                score += (lane_config->lane_count - 1 - i);
            }

            if (score > best_score)
            {
                best_score = score;
                best_lane = i;
            }
        }
    }

    /* Fallback: if no lane matches the required direction, recommend based on turn direction */
    if (best_lane < 0 && lane_config->lane_count > 0)
    {
        /* For left turns, recommend leftmost lane */
        if (required_dir & (LANE_DIR_LEFT | LANE_DIR_SLIGHT_LEFT | LANE_DIR_SHARP_LEFT | LANE_DIR_UTURN))
        {
            best_lane = 0;  /* Leftmost lane */
        }
        /* For right turns, recommend rightmost lane */
        else if (required_dir & (LANE_DIR_RIGHT | LANE_DIR_SLIGHT_RIGHT | LANE_DIR_SHARP_RIGHT))
        {
            best_lane = lane_config->lane_count - 1;  /* Rightmost lane */
        }
        /* For straight, recommend middle lane */
        else if (required_dir & LANE_DIR_STRAIGHT)
        {
            best_lane = lane_config->lane_count / 2;  /* Middle lane */
        }
    }

    return (best_lane >= 0) ? (uint8_t)best_lane : 255;
}

/**
 * @brief Get extended junction guidance with lane information
 */
map_error_t guidance_get_next_junction_ext(const guidance_state_t *state,
                                           junction_guidance_ext_t *guidance,
                                           const lane_config_t *lane_config)
{
    if (!state || !guidance)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    memset(guidance, 0, sizeof(junction_guidance_ext_t));

    junction_guidance_t basic_guidance;
    map_error_t err = guidance_get_next_junction(state, &basic_guidance);
    if (err != MAP_OK)
    {
        return err;
    }

    guidance->distance = basic_guidance.distance;
    guidance->maneuver = basic_guidance.maneuver;
    guidance->turn_angle = basic_guidance.turn_angle;
    guidance->segment_index = basic_guidance.segment_index;

    if (lane_config && lane_config->lane_count > 0)
    {
        guidance->has_lane_info = 1;
        guidance->lane_info.lane_count = lane_config->lane_count;

        uint8_t recommended = guidance_calculate_recommended_lane(lane_config, guidance->maneuver);
        guidance->lane_info.recommended_lane = recommended;
        guidance->lane_info.current_lane = 255;

        for (int i = 0; i < lane_config->lane_count && i < MAX_LANES_PER_ROAD; i++)
        {
            guidance->lane_info.lanes[i].directions = lane_config->lanes[i].directions;
            guidance->lane_info.lanes[i].is_recommended = (i == recommended) ? 1 : 0;
            guidance->lane_info.lanes[i].is_current = 0;
        }
    }

    return MAP_OK;
}

/* Lane Display Style Functions */

void lane_display_style_init(lane_display_style_t *style)
{
    if (!style)
    {
        return;
    }

    style->lane_bg_color = 0x404040C0;
    style->lane_recommended_color = 0x4CAF50E8;
    style->lane_current_color = 0x2196F3E8;
    style->lane_arrow_color = 0xFFFFFFFF;
    style->lane_arrow_inactive_color = 0x808080FF;
    style->lane_separator_color = 0xFFFFFF80;

    style->lane_width = 36.0f;
    style->lane_height = 50.0f;
    style->lane_spacing = 2.0f;
    style->lane_arrow_size = 20.0f;
    style->separator_width = 2.0f;
}

void lane_display_style_init_preset(lane_display_style_t *style,
                                    guidance_style_preset_t preset)
{
    if (!style)
    {
        return;
    }

    style->lane_width = 36.0f;
    style->lane_height = 50.0f;
    style->lane_spacing = 2.0f;
    style->lane_arrow_size = 20.0f;
    style->separator_width = 2.0f;
    switch (preset)
    {
    case GUIDANCE_STYLE_GOOGLE:
        /* Modern Google Maps style: dark gray background, blue highlight */
        style->lane_bg_color = 0x4A4A4AE8;              /* Medium dark gray */
        style->lane_recommended_color = 0x4285F4E8;     /* Google Blue */
        style->lane_current_color = 0x4285F4E8;         /* Google Blue */
        style->lane_arrow_color = 0xFFFFFFFF;           /* White arrows */
        style->lane_arrow_inactive_color = 0xB0B0B0FF;  /* Light gray arrows */
        style->lane_separator_color = 0x00000040;       /* Subtle dark separator */
        break;

    case GUIDANCE_STYLE_DARK:
        style->lane_bg_color = 0x1A1A2EE8;
        style->lane_recommended_color = 0x4A90D9E8;
        style->lane_current_color = 0x4A90D9E8;
        style->lane_arrow_color = 0xE0E0E0FF;
        style->lane_arrow_inactive_color = 0x606060FF;
        style->lane_separator_color = 0x808080A0;
        break;

    case GUIDANCE_STYLE_AMAP:
        style->lane_bg_color = 0x202050E8;
        style->lane_recommended_color = 0x3385FFE8;
        style->lane_current_color = 0xFFCC00E8;
        style->lane_arrow_color = 0xFFFFFFFF;
        style->lane_arrow_inactive_color = 0x808080FF;
        style->lane_separator_color = 0xFFFFFF80;
        break;

    case GUIDANCE_STYLE_BAIDU:
        style->lane_bg_color = 0x303050E8;
        style->lane_recommended_color = 0x3385FFE8;
        style->lane_current_color = 0xFF6600E8;
        style->lane_arrow_color = 0xFFFFFFFF;
        style->lane_arrow_inactive_color = 0x808080FF;
        style->lane_separator_color = 0xFFFFFF80;
        break;

    case GUIDANCE_STYLE_DEFAULT:
    default:
        lane_display_style_init(style);
        break;
    }
}

void lane_display_style_init_from_theme(lane_display_style_t *style,
                                        const map_theme_t *map_theme)
{
    if (!style)
    {
        return;
    }
    lane_display_style_init(style);

    if (!map_theme)
    {
        return;
    }

    uint32_t bg = map_theme->background;
    uint8_t bg_r = (bg >> 24) & 0xFF;
    uint8_t bg_g = (bg >> 16) & 0xFF;
    uint8_t bg_b = (bg >> 8) & 0xFF;
    int luminance = (bg_r * 299 + bg_g * 587 + bg_b * 114) / 1000;
    int is_dark = (luminance < 128);

    if (is_dark)
    {
        style->lane_bg_color = 0x2D2D44E8;
        style->lane_recommended_color = 0x4A90D9E8;
        style->lane_arrow_color = 0xE0E0E0FF;
        style->lane_arrow_inactive_color = 0x606060FF;
    }
    else
    {
        style->lane_bg_color = 0x505050E8;
        style->lane_recommended_color = 0x34A853E8;
        style->lane_arrow_color = 0xFFFFFFFF;
        style->lane_arrow_inactive_color = 0x9E9E9EFF;
    }
    style->lane_current_color = map_theme->road_motorway;
}

/* ============================================================================
 * Lane Direction Arrow Rendering (Phase 2) - Using TRVG Fast Lane Arrow API
 * ============================================================================
 */

void render_lane_direction_arrows(renderer_t *renderer,
                                  uint8_t directions,
                                  uint32_t color,
                                  float cx, float cy, float size)
{
    if (!renderer)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    int isize = (int)size;
    int icx = (int)cx;
    int icy = (int)cy;

    /* Check for common combination scenarios, use dedicated combo arrows */
    uint8_t straight = directions & LANE_DIR_STRAIGHT;
    uint8_t left = directions & LANE_DIR_LEFT;
    uint8_t right = directions & LANE_DIR_RIGHT;
    uint8_t slight_left = directions & LANE_DIR_SLIGHT_LEFT;
    uint8_t slight_right = directions & LANE_DIR_SLIGHT_RIGHT;

    /* Straight + Right */
    if (straight && right && !left && !slight_left && !slight_right)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_STRAIGHT_RIGHT, icx, icy, isize, r, g, b);
        return;
    }

    /* Straight + Left */
    if (straight && left && !right && !slight_left && !slight_right)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_STRAIGHT_LEFT, icx, icy, isize, r, g, b);
        return;
    }

    /* Left + Right */
    if (left && right && !straight && !slight_left && !slight_right)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_LEFT_RIGHT, icx, icy, isize, r, g, b);
        return;
    }

    /* Straight + Left + Right */
    if (straight && left && right && !slight_left && !slight_right)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_STRAIGHT_LEFT_RIGHT, icx, icy, isize, r, g, b);
        return;
    }

    /* Straight + Slight Left */
    if (straight && slight_left && !left && !right && !slight_right)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_STRAIGHT_SLIGHT_LEFT, icx, icy, isize, r, g, b);
        return;
    }

    /* Straight + Slight Right */
    if (straight && slight_right && !left && !right && !slight_left)
    {
        trvg_fast_combo_arrow(ctx, TRVG_COMBO_STRAIGHT_SLIGHT_RIGHT, icx, icy, isize, r, g, b);
        return;
    }

    /* Single direction - use single arrow */
    if (directions == LANE_DIR_STRAIGHT || directions == 0)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_STRAIGHT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_LEFT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_LEFT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_RIGHT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_RIGHT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_SLIGHT_LEFT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_SLIGHT_LEFT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_SLIGHT_RIGHT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_SLIGHT_RIGHT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_SHARP_LEFT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_SHARP_LEFT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_SHARP_RIGHT)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_SHARP_RIGHT, icx, icy, isize, r, g, b);
        return;
    }
    if (directions == LANE_DIR_UTURN)
    {
        trvg_fast_lane_arrow(ctx, TRVG_ARROW_UTURN_LEFT, icx, icy, isize, r, g, b);
        return;
    }

    /* Other unhandled combinations - default to straight */
    trvg_fast_lane_arrow(ctx, TRVG_ARROW_STRAIGHT, icx, icy, isize, r, g, b);
}

void render_lane_icon(renderer_t *renderer,
                      const lane_display_info_t *lane,
                      const lane_display_style_t *style,
                      float x, float y,
                      float width, float height)
{
    if (!renderer || !lane || !style)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);

    uint32_t bg_color;
    if (lane->is_recommended)
    {
        bg_color = style->lane_recommended_color;
    }
    else if (lane->is_current)
    {
        bg_color = style->lane_current_color;
    }
    else
    {
        bg_color = style->lane_bg_color;
    }

    trvg_color_t bg = color_to_trvg(bg_color);
    trvg_fast_fill_rect(ctx, (int)x, (int)y, (int)width, (int)height, bg.r, bg.g, bg.b);

    uint32_t arrow_color = lane->is_recommended ?
                           style->lane_arrow_color : style->lane_arrow_inactive_color;

    float arrow_cx = x + width / 2.0f;
    float arrow_cy = y + height / 2.0f;
    render_lane_direction_arrows(renderer, lane->directions, arrow_color,
                                 arrow_cx, arrow_cy, style->lane_arrow_size);
}

void render_lane_guidance_panel(renderer_t *renderer,
                                const lane_guidance_info_t *lane_info,
                                const lane_display_style_t *style,
                                float x, float y)
{
    if (!renderer || !lane_info || !style)
    {
        return;
    }
    if (lane_info->lane_count == 0)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);

    float lane_x = x;
    for (int i = 0; i < lane_info->lane_count && i < MAX_LANES_PER_ROAD; i++)
    {
        render_lane_icon(renderer, &lane_info->lanes[i], style,
                         lane_x, y, style->lane_width, style->lane_height);

        if (i < lane_info->lane_count - 1)
        {
            float sep_x = lane_x + style->lane_width;
            trvg_color_t sep_color = color_to_trvg(style->lane_separator_color);
            trvg_fast_fill_rect(ctx, (int)sep_x, (int)y,
                                (int)style->lane_spacing, (int)style->lane_height,
                                sep_color.r, sep_color.g, sep_color.b);
        }

        lane_x += style->lane_width + style->lane_spacing;
    }
}

void render_full_lane_guidance(renderer_t *renderer,
                               const junction_guidance_ext_t *guidance,
                               const guidance_style_t *turn_style,
                               const lane_display_style_t *lane_style,
                               float x, float y, float width)
{
    if (!renderer || !guidance || !lane_style)
    {
        return;
    }

    /* Only render if we have lane info */
    if (!guidance->has_lane_info || guidance->lane_info.lane_count == 0)
    {
        return;
    }

    trvg_context_t *ctx = GET_TRVG(renderer);

    /* Calculate lane panel dimensions */
    float padding = turn_style ? turn_style->panel_padding : 8.0f;
    float corner_radius = turn_style ? turn_style->corner_radius : 8.0f;
    uint32_t bg_color = turn_style ? turn_style->background_color : 0x000000C0;

    float lane_total_width = guidance->lane_info.lane_count * lane_style->lane_width +
                             (guidance->lane_info.lane_count - 1) * lane_style->lane_spacing;

    /* Panel dimensions - add space for text below lanes */
    float text_height = 20.0f;  /* Height reserved for distance text */
    float panel_width = lane_total_width + padding * 2;
    float panel_height = lane_style->lane_height + padding * 2 + text_height;

    /* Use provided width if larger */
    if (width > panel_width)
    {
        panel_width = width;
    }

    /* Draw background panel with rounded corners */
    draw_rounded_rect_trvg(ctx, x, y, panel_width, panel_height,
                           corner_radius, color_to_trvg(bg_color));

    /* Center lanes horizontally within the panel */
    float lane_x = x + (panel_width - lane_total_width) / 2.0f;
    float lane_y = y + padding;

    /* Draw lane icons */
    for (int i = 0; i < guidance->lane_info.lane_count && i < MAX_LANES_PER_ROAD; i++)
    {
        render_lane_icon(renderer, &guidance->lane_info.lanes[i], lane_style,
                         lane_x, lane_y, lane_style->lane_width, lane_style->lane_height);

        /* Draw separator between lanes */
        if (i < guidance->lane_info.lane_count - 1)
        {
            float sep_x = lane_x + lane_style->lane_width;
            trvg_color_t sep_color = color_to_trvg(lane_style->lane_separator_color);
            trvg_fast_fill_rect(ctx, (int)sep_x, (int)lane_y,
                                (int)lane_style->lane_spacing, (int)lane_style->lane_height,
                                sep_color.r, sep_color.g, sep_color.b);
        }

        lane_x += lane_style->lane_width + lane_style->lane_spacing;
    }    /* Render distance and maneuver text below the lane icons - always show */
    {
        char guidance_text[64];

        /* Special handling for different maneuvers */
        if (guidance->maneuver == MANEUVER_STRAIGHT || guidance->maneuver == MANEUVER_NONE)
        {
            /* Straight ahead */
            if (guidance->distance < 1000.0f)
            {
                snprintf(guidance_text, sizeof(guidance_text), "Straight %.0fm", (double)guidance->distance);
            }
            else
            {
                snprintf(guidance_text, sizeof(guidance_text), "Straight %.1fkm",
                         (double)(guidance->distance / 1000.0f));
            }
        }
        else if (guidance->maneuver == MANEUVER_ARRIVE)
        {
            /* Arriving at destination */
            if (guidance->distance < 1000.0f)
            {
                snprintf(guidance_text, sizeof(guidance_text), "Arrive in %.0fm", (double)guidance->distance);
            }
            else
            {
                snprintf(guidance_text, sizeof(guidance_text), "Arrive in %.1fkm",
                         (double)(guidance->distance / 1000.0f));
            }
        }
        else
        {
            /* Other maneuvers (turns, etc.) */
            const char *maneuver_str = (guidance->maneuver < MANEUVER_COUNT) ?
                                       maneuver_names[guidance->maneuver] : "Unknown";

            /* Format distance: use meters if < 1000m, otherwise use km */
            if (guidance->distance < 1000.0f)
            {
                snprintf(guidance_text, sizeof(guidance_text), "%s in %.0fm",
                         maneuver_str, (double)guidance->distance);
            }
            else
            {
                snprintf(guidance_text, sizeof(guidance_text), "%s in %.1fkm",
                         maneuver_str, (double)(guidance->distance / 1000.0f));
            }
        }

        /* Draw text centered below the lane icons */
        float text_y = lane_y + lane_style->lane_height + 4.0f;
        float text_x = x + padding;
        render_text(renderer, text_x, text_y, guidance_text, 18.0f, 0xFFFFFFFF);  /* White text */
    }
}
