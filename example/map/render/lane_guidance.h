/**
 * @file lane_guidance.h
 * @brief Lane guidance API for junction navigation
 *
 * Phase 1: Basic turn indicators (left, right, straight) and distance hints
 * Phase 2: Multi-lane display with recommended lane highlighting
 */

#ifndef LANE_GUIDANCE_H
#define LANE_GUIDANCE_H

#include "../core/map_types.h"
#include "render_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Maneuver Types - Turn direction indicators
 * ============================================================================
 */

/**
 * @brief Maneuver type enumeration
 */
typedef enum
{
    MANEUVER_NONE = 0,          /* No maneuver / continue straight */
    MANEUVER_STRAIGHT,          /* Go straight */
    MANEUVER_LEFT,              /* Turn left */
    MANEUVER_RIGHT,             /* Turn right */
    MANEUVER_SLIGHT_LEFT,       /* Slight left / bear left */
    MANEUVER_SLIGHT_RIGHT,      /* Slight right / bear right */
    MANEUVER_SHARP_LEFT,        /* Sharp left turn */
    MANEUVER_SHARP_RIGHT,       /* Sharp right turn */
    MANEUVER_UTURN_LEFT,        /* U-turn to left */
    MANEUVER_UTURN_RIGHT,       /* U-turn to right */
    MANEUVER_RAMP_LEFT,         /* Take ramp on left */
    MANEUVER_RAMP_RIGHT,        /* Take ramp on right */
    MANEUVER_MERGE,             /* Merge onto road */
    MANEUVER_ARRIVE,            /* Arrive at destination */
    MANEUVER_COUNT              /* Number of maneuver types */
} maneuver_type_t;

/* ============================================================================
 * Phase 2: Multi-Lane Display Structures
 * ============================================================================
 */

/**
 * @brief Lane display information for rendering
 *
 * Extended from lane_info_t with rendering-specific data
 */
typedef struct
{
    uint8_t directions;         /* Bitmask of allowed directions (lane_direction_t) */
    uint8_t is_recommended;     /* 1 if this is the recommended lane */
    uint8_t is_current;         /* 1 if vehicle is currently in this lane */
    uint8_t reserved;           /* Reserved */
} lane_display_info_t;

/**
 * @brief Multi-lane guidance information
 *
 * Contains lane configuration and recommended lane for the upcoming junction
 */
typedef struct
{
    uint8_t lane_count;                             /* Total number of lanes (0 = no lane info) */
    uint8_t recommended_lane;                       /* Index of recommended lane (0-based, 255 = none) */
    uint8_t current_lane;                           /* Index of current lane (255 = unknown) */
    uint8_t reserved;                               /* Reserved */
    lane_display_info_t lanes[MAX_LANES_PER_ROAD];  /* Lane information array */
} lane_guidance_info_t;

/**
 * @brief Extended junction guidance with lane information
 */
typedef struct
{
    /* Basic junction info (Phase 1) */
    float distance;                 /* Distance to next junction in meters */
    maneuver_type_t maneuver;       /* Type of maneuver at junction */
    float turn_angle;               /* Turn angle in degrees (-180 to 180) */
    uint32_t segment_index;         /* Current path segment index */

    /* Lane info (Phase 2) */
    lane_guidance_info_t lane_info; /* Multi-lane information */
    uint8_t has_lane_info;          /* 1 if lane_info is valid */
    uint8_t reserved[3];            /* Reserved for alignment */
} junction_guidance_ext_t;

/**
 * @brief Lane display style configuration
 */
typedef struct
{
    uint32_t lane_bg_color;             /* Lane background color (inactive) */
    uint32_t lane_recommended_color;    /* Recommended lane highlight color */
    uint32_t lane_current_color;        /* Current lane color */
    uint32_t lane_arrow_color;          /* Lane direction arrow color */
    uint32_t lane_arrow_inactive_color; /* Inactive direction arrow color */
    uint32_t lane_separator_color;      /* Lane separator line color */
    float lane_width;                   /* Width of each lane icon in pixels */
    float lane_height;                  /* Height of lane icons in pixels */
    float lane_spacing;                 /* Spacing between lanes in pixels */
    float lane_arrow_size;              /* Size of direction arrows in lanes */
    float separator_width;              /* Width of lane separator lines */
} lane_display_style_t;

/* ============================================================================
 * Guidance Structures
 * ============================================================================
 */

/**
 * @brief Junction guidance information
 */
typedef struct
{
    float distance;             /* Distance to next junction in meters */
    maneuver_type_t maneuver;   /* Type of maneuver at junction */
    float turn_angle;           /* Turn angle in degrees (-180 to 180) */
    uint32_t segment_index;     /* Current path segment index */
} junction_guidance_t;

/**
 * @brief Guidance state for continuous navigation
 */
typedef struct
{
    const path_t *path;         /* Reference to current path */
    uint32_t current_segment;   /* Current segment index */
    float traveled_distance;    /* Distance traveled on current segment */
} guidance_state_t;

/* ============================================================================
 * Guidance API Functions
 * ============================================================================
 */

/**
 * @brief Initialize guidance state with a path
 *
 * @param state Guidance state to initialize
 * @param path Navigation path
 * @return MAP_OK on success
 */
map_error_t guidance_init(guidance_state_t *state, const path_t *path);

/**
 * @brief Update guidance state with current position
 *
 * @param state Guidance state
 * @param current_lat Current latitude
 * @param current_lon Current longitude
 * @return MAP_OK on success
 */
map_error_t guidance_update(guidance_state_t *state,
                            float current_lat, float current_lon);

/**
 * @brief Get next junction guidance from current position
 *
 * @param state Guidance state
 * @param guidance Output guidance information
 * @return MAP_OK on success, MAP_ERR_NO_PATH if arrived
 */
map_error_t guidance_get_next_junction(const guidance_state_t *state,
                                       junction_guidance_t *guidance);

/**
 * @brief Get extended junction guidance with lane information
 *
 * @param state Guidance state
 * @param guidance Output extended guidance information
 * @param lane_config Optional lane configuration for current road (NULL if not available)
 * @return MAP_OK on success
 */
map_error_t guidance_get_next_junction_ext(const guidance_state_t *state,
                                           junction_guidance_ext_t *guidance,
                                           const lane_config_t *lane_config);

/**
 * @brief Calculate maneuver type from turn angle
 *
 * @param angle Turn angle in degrees (-180 to 180)
 * @return Maneuver type
 */
maneuver_type_t guidance_angle_to_maneuver(float angle);

/**
 * @brief Get maneuver name string (for debugging)
 *
 * @param maneuver Maneuver type
 * @return Static string describing the maneuver
 */
const char *guidance_maneuver_name(maneuver_type_t maneuver);

/**
 * @brief Calculate turn angle between two path segments
 *
 * @param p1 First point (previous)
 * @param p2 Second point (current junction)
 * @param p3 Third point (next)
 * @return Angle in degrees (-180 to 180), positive = right turn
 */
float guidance_calculate_turn_angle(const coord_t *p1,
                                    const coord_t *p2,
                                    const coord_t *p3);

/**
 * @brief Calculate recommended lane based on next maneuver
 *
 * @param lane_config Lane configuration for current road
 * @param next_maneuver The upcoming maneuver type
 * @return Recommended lane index (0-based), or 255 if no recommendation
 */
uint8_t guidance_calculate_recommended_lane(const lane_config_t *lane_config,
                                            maneuver_type_t next_maneuver);

/**
 * @brief Convert maneuver type to lane direction bitmask
 *
 * @param maneuver Maneuver type
 * @return Corresponding lane direction flag(s)
 */
uint8_t maneuver_to_lane_direction(maneuver_type_t maneuver);

/* ============================================================================
 * Lane Guidance Rendering
 * ============================================================================
 */

/**
 * @brief Lane guidance display style
 */
typedef struct
{
    uint32_t background_color;  /* Panel background color (RGBA) */
    uint32_t arrow_color;       /* Arrow color (RGBA) */
    uint32_t text_color;        /* Text color (RGBA) */
    uint32_t highlight_color;   /* Highlight/accent color (RGBA) */
    float arrow_size;           /* Arrow icon size in pixels */
    float font_size;            /* Distance text font size */
    float panel_padding;        /* Padding inside panel */
    float corner_radius;        /* Panel corner radius */
} guidance_style_t;

/**
 * @brief Guidance style preset types
 */
typedef enum
{
    GUIDANCE_STYLE_DEFAULT = 0, /* Default dark semi-transparent style */
    GUIDANCE_STYLE_GOOGLE,      /* Google Maps style (light background) */
    GUIDANCE_STYLE_DARK,        /* Dark mode style */
    GUIDANCE_STYLE_AMAP,        /* AMap (Gaode) style */
    GUIDANCE_STYLE_BAIDU,       /* Baidu Maps style */
} guidance_style_preset_t;

/**
 * @brief Initialize guidance style with defaults
 *
 * @param style Style structure to initialize
 */
void guidance_style_init(guidance_style_t *style);

/**
 * @brief Initialize guidance style with a preset theme
 *
 * @param style Style structure to initialize
 * @param preset Style preset type (GUIDANCE_STYLE_GOOGLE, etc.)
 */
void guidance_style_init_preset(guidance_style_t *style, guidance_style_preset_t preset);

/**
 * @brief Initialize guidance style to match map theme
 *
 * @param style Style structure to initialize
 * @param map_theme Map theme to match
 */
void guidance_style_init_from_theme(guidance_style_t *style, const map_theme_t *map_theme);

/**
 * @brief Initialize lane display style with defaults
 *
 * @param style Style structure to initialize
 */
void lane_display_style_init(lane_display_style_t *style);

/**
 * @brief Initialize lane display style with a preset theme
 *
 * @param style Style structure to initialize
 * @param preset Style preset type
 */
void lane_display_style_init_preset(lane_display_style_t *style,
                                    guidance_style_preset_t preset);

/**
 * @brief Initialize lane display style from map theme
 *
 * @param style Style structure to initialize
 * @param map_theme Map theme to match
 */
void lane_display_style_init_from_theme(lane_display_style_t *style,
                                        const map_theme_t *map_theme);

/**
 * @brief Render lane guidance panel
 *
 * Draws a panel with turn arrow and distance indicator.
 * Typically placed at top-left corner of map view.
 *
 * @param renderer Renderer context
 * @param guidance Junction guidance data
 * @param style Display style
 * @param x Panel X position
 * @param y Panel Y position
 * @param width Panel width
 * @param height Panel height
 */
void render_lane_guidance(renderer_t *renderer,
                          const junction_guidance_t *guidance,
                          const guidance_style_t *style,
                          float x, float y,
                          float width, float height);

/**
 * @brief Render turn arrow icon
 *
 * @param renderer Renderer context
 * @param maneuver Maneuver type to draw
 * @param color Arrow color (RGBA)
 * @param cx Center X position
 * @param cy Center Y position
 * @param size Arrow size in pixels
 */
void render_turn_arrow(renderer_t *renderer,
                       maneuver_type_t maneuver,
                       uint32_t color,
                       float cx, float cy, float size);

/**
 * @brief Render multi-lane guidance panel
 *
 * Draws lane indicators with direction arrows and highlights.
 * Typically placed below the turn arrow panel.
 *
 * @param renderer Renderer context
 * @param lane_info Lane guidance information
 * @param style Display style
 * @param x Panel X position
 * @param y Panel Y position
 */
void render_lane_guidance_panel(renderer_t *renderer,
                                const lane_guidance_info_t *lane_info,
                                const lane_display_style_t *style,
                                float x, float y);

/**
 * @brief Render a single lane icon with direction arrows
 *
 * @param renderer Renderer context
 * @param lane Lane information
 * @param style Display style
 * @param x Lane icon X position (left edge)
 * @param y Lane icon Y position (top edge)
 * @param width Lane icon width
 * @param height Lane icon height
 */
void render_lane_icon(renderer_t *renderer,
                      const lane_display_info_t *lane,
                      const lane_display_style_t *style,
                      float x, float y,
                      float width, float height);

/**
 * @brief Render lane direction arrow(s) within a lane icon
 *
 * @param renderer Renderer context
 * @param directions Bitmask of lane directions
 * @param color Arrow color
 * @param cx Center X position
 * @param cy Center Y position
 * @param size Arrow size
 */
void render_lane_direction_arrows(renderer_t *renderer,
                                  uint8_t directions,
                                  uint32_t color,
                                  float cx, float cy, float size);

/**
 * @brief Render combined turn and lane guidance
 *
 * Renders both the turn arrow panel and lane indicators in one call.
 * Layout: Turn arrow on top, lane indicators below.
 *
 * @param renderer Renderer context
 * @param guidance Extended junction guidance
 * @param turn_style Turn arrow panel style
 * @param lane_style Lane display style
 * @param x Panel X position
 * @param y Panel Y position
 * @param width Panel width
 */
void render_full_lane_guidance(renderer_t *renderer,
                               const junction_guidance_ext_t *guidance,
                               const guidance_style_t *turn_style,
                               const lane_display_style_t *lane_style,
                               float x, float y, float width);

/**
 * @brief Format distance for display
 *
 * Converts distance in meters to human-readable string.
 * Examples: "50 m", "200 m", "1.2 km", "5 km"
 *
 * @param distance Distance in meters
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 */
void guidance_format_distance(float distance, char *buffer, int buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* LANE_GUIDANCE_H */
