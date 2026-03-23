/**
 * @file render_api.h
 * @brief Rendering API using NanoVG
 */

#ifndef RENDER_API_H
#define RENDER_API_H

#include "../core/map_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TRVG (Triton Vector Graphics) is the rendering backend */
#include "../graphics/trvg/trvg.h"

/* Renderer structure */
typedef struct
{
    trvg_context_t trvg;    /* TRVG context (embedded) */
    int width;              /* Canvas width */
    int height;             /* Canvas height */
    unsigned char *pixels;  /* Pixel buffer (RGBA) */

    /* Viewport */
    float view_min_lat;
    float view_min_lon;
    float view_max_lat;
    float view_max_lon;
} renderer_t;

/* ============================================================================
 * Render Options - Control which elements to render
 * ============================================================================
 */

/**
 * @brief Render options flags - use to enable/disable specific map elements
 */
#define RENDER_OPT_ROADS        (1 << 0)   /* Render roads */
#define RENDER_OPT_WATER        (1 << 1)   /* Render water bodies (lakes, rivers) */
#define RENDER_OPT_PARKS        (1 << 2)   /* Render parks and forests */
#define RENDER_OPT_BUILDINGS    (1 << 3)   /* Render buildings */
#define RENDER_OPT_LABELS       (1 << 4)   /* Render text labels */
#define RENDER_OPT_INDUSTRIAL   (1 << 5)   /* Render industrial areas */
#define RENDER_OPT_COMMERCIAL   (1 << 6)   /* Render commercial areas */
#define RENDER_OPT_RESIDENTIAL  (1 << 7)   /* Render residential areas */
#define RENDER_OPT_GRASS        (1 << 8)   /* Render grass areas */

/* Convenience combinations */
#define RENDER_OPT_ALL_AREAS    (RENDER_OPT_WATER | RENDER_OPT_PARKS | RENDER_OPT_BUILDINGS | \
                                 RENDER_OPT_INDUSTRIAL | RENDER_OPT_COMMERCIAL | \
                                 RENDER_OPT_RESIDENTIAL | RENDER_OPT_GRASS)
#define RENDER_OPT_ALL          (RENDER_OPT_ROADS | RENDER_OPT_ALL_AREAS | RENDER_OPT_LABELS)
#define RENDER_OPT_DEFAULT      RENDER_OPT_ALL

/**
 * @brief Road type flags - use to enable/disable specific road types
 */
#define ROAD_TYPE_SERVICE       (1 << 0)   /* Service roads */
#define ROAD_TYPE_RESIDENTIAL   (1 << 1)   /* Residential roads */
#define ROAD_TYPE_TERTIARY      (1 << 2)   /* Tertiary roads */
#define ROAD_TYPE_SECONDARY     (1 << 3)   /* Secondary roads */
#define ROAD_TYPE_PRIMARY       (1 << 4)   /* Primary roads */
#define ROAD_TYPE_TRUNK         (1 << 5)   /* Trunk roads */
#define ROAD_TYPE_MOTORWAY      (1 << 6)   /* Motorways */

/* Road type convenience combinations */
#define ROAD_TYPE_ALL           (0x7F)     /* All road types */
#define ROAD_TYPE_MAJOR         (ROAD_TYPE_PRIMARY | ROAD_TYPE_TRUNK | ROAD_TYPE_MOTORWAY)
#define ROAD_TYPE_MAIN          (ROAD_TYPE_SECONDARY | ROAD_TYPE_MAJOR)
#define ROAD_TYPE_URBAN         (ROAD_TYPE_TERTIARY | ROAD_TYPE_MAIN)

/**
 * @brief Label type flags - use to enable/disable specific label types
 */
#define LABEL_TYPE_ROAD         (1 << 0)   /* Road names */
#define LABEL_TYPE_PLACE        (1 << 1)   /* Place names (city, district, etc.) */
#define LABEL_TYPE_POI          (1 << 2)   /* Points of interest */
#define LABEL_TYPE_WATER        (1 << 3)   /* Water body names */
#define LABEL_TYPE_PARK         (1 << 4)   /* Park names */
#define LABEL_TYPE_OTHER        (1 << 5)   /* Other labels */

/* Label type convenience combinations */
#define LABEL_TYPE_ALL          (0x3F)     /* All label types */
#define LABEL_TYPE_NAMES        (LABEL_TYPE_ROAD | LABEL_TYPE_PLACE | LABEL_TYPE_WATER | LABEL_TYPE_PARK)
#define LABEL_TYPE_GEOGRAPHIC   (LABEL_TYPE_PLACE | LABEL_TYPE_WATER | LABEL_TYPE_PARK)

/**
 * @brief Text stroke mode - controls how text outlines are rendered
 */
typedef enum
{
    TEXT_STROKE_NONE = 0,       /* No stroke/outline */
    TEXT_STROKE_4DIR,           /* 4-direction offset (up/down/left/right) - default */
    TEXT_STROKE_8DIR,           /* 8-direction offset (including diagonals) */
    TEXT_STROKE_SDF,            /* Signed Distance Field - high quality */
} text_stroke_mode_t;

/**
 * @brief Render options structure
 */
typedef struct
{
    uint32_t flags;         /* Combination of RENDER_OPT_* flags */

    /* Fine-grained road type control (bitmask for road types 0-6) */
    uint8_t road_types;     /* Bitmask: use ROAD_TYPE_* macros */
    /* Set to ROAD_TYPE_ALL to render all road types */

    /* Fine-grained label type control */
    uint8_t label_types;    /* Bitmask: use LABEL_TYPE_* macros */
    /* Set to LABEL_TYPE_ALL to render all label types */

    /* Fine-grained road label control (which road types get labels) */
    uint8_t road_label_types;  /* Bitmask: use ROAD_TYPE_* macros for road labels */
    /* Set to ROAD_TYPE_ALL to label all road types */

    /* Road segment simplification options */
    float min_road_segment_length;  /* Minimum road segment length in screen pixels */
    /* Set to 0.0 to disable filtering (default) */    /* Area polygon simplification options */
    float min_area_segment_length;  /* Minimum area polygon segment length in screen pixels */
    /* Set to 0.0 to disable filtering (default) */

    float min_area_size;            /* Minimum area bounding box size in screen pixels squared */
    /* Set to 0.0 to disable filtering (default) */

    /* Text stroke/outline options */
    text_stroke_mode_t text_stroke_mode;  /* Text outline mode: NONE, 4DIR, 8DIR, or SDF */
    /* Set to TEXT_STROKE_4DIR by default */

    uint32_t text_stroke_color;     /* Stroke color (RGBA), default is white 0xFFFFFFFF */
    /* Only used when text_stroke_mode != TEXT_STROKE_NONE */

    uint8_t text_stroke_width;      /* Stroke width in pixels (1-3), default is 1 */
    /* For 4DIR/8DIR: offset distance; For SDF: outline thickness */
} render_options_t;

/**
 * @brief Initialize render options with default values (render everything)
 *
 * @param options Pointer to options structure to initialize
 */
void render_options_init(render_options_t *options);

/**
 * @brief Render complete map with options control
 *
 * @param renderer Pointer to renderer
 * @param map Map data
 * @param theme Color theme
 * @param options Render options (NULL for default = render all)
 */
void render_map_with_options(renderer_t *renderer, const map_t *map,
                             const map_theme_t *theme, const render_options_t *options);

/**
 * @brief Create a new renderer
 *
 * @param width Canvas width in pixels
 * @param height Canvas height in pixels
 * @param format Pixel format (TRVG_PIXEL_RGBA8888, TRVG_PIXEL_RGB565, etc.)
 * @param external_buffer Optional external pixel buffer (NULL to allocate internally)
 * @return Pointer to renderer, NULL on error
 */
renderer_t *renderer_create(int width, int height, trvg_pixel_format_t format,
                            unsigned char *external_buffer);

/**
 * @brief Destroy renderer and free resources
 *
 * @param renderer Pointer to renderer
 */
void renderer_destroy(renderer_t *renderer);

/**
 * @brief Set viewport to show specific map region
 *
 * @param renderer Pointer to renderer
 * @param min_lat Minimum latitude
 * @param min_lon Minimum longitude
 * @param max_lat Maximum latitude
 * @param max_lon Maximum longitude
 */
void renderer_set_viewport(renderer_t *renderer,
                           float min_lat, float min_lon,
                           float max_lat, float max_lon);

/**
 * @brief Set viewport to fit a path with padding
 *
 * @param renderer Pointer to renderer
 * @param path Path to fit
 * @param padding_percent Padding as percentage of size (e.g., 0.1 for 10%)
 */
void renderer_fit_path(renderer_t *renderer, const path_t *path, float padding_percent);

/**
 * @brief Clear canvas with background color
 *
 * @param renderer Pointer to renderer
 * @param color Background color (RGBA)
 */
void renderer_clear(renderer_t *renderer, uint32_t color);

/**
 * @brief Render a path on the canvas
 *
 * @param renderer Pointer to renderer
 * @param path Path to render
 * @param style Render style
 */
void render_path(renderer_t *renderer, const path_t *path, const render_style_t *style);

/**
 * @brief Draw text on the canvas
 *
 * @param renderer Pointer to renderer
 * @param x X coordinate
 * @param y Y coordinate
 * @param text Text string to draw
 * @param size Font size
 * @param color Text color (RGBA)
 */
void render_text(renderer_t *renderer, float x, float y, const char *text, float size,
                 uint32_t color);

/**
 * @brief Set font data from memory address (for MCU direct memory access)
 *
 * This function allows setting font data directly from a memory address,
 * which is useful for MCU environments where the font is stored in flash
 * or a specific memory region.
 *
 * @param font_data Pointer to font data in memory
 * @param font_size Size of font data in bytes (optional, can be 0 if unknown)
 * @return true if font was successfully initialized, false otherwise
 */
bool render_set_font_from_memory(const unsigned char *font_data, size_t font_size);

/**
 * @brief Check if font is already loaded
 *
 * @return true if font is loaded and ready for use
 */
bool render_is_font_loaded(void);


/**
 * @brief Save canvas to PNG file
 *
 * @param renderer Pointer to renderer
 * @param filepath Output file path
 * @return MAP_OK on success
 */
map_error_t render_save_png(renderer_t *renderer, const char *filepath);

/**
 * @brief Get pixel buffer pointer (when PNG save is disabled)
 *
 * This function is a replacement for render_save_png when ENABLE_PNG_SAVE is not defined.
 * Instead of saving to file, it returns a pointer to the pixel buffer.
 *
 * @param renderer Pointer to renderer
 * @param width Output: buffer width (can be NULL)
 * @param height Output: buffer height (can be NULL)
 * @return Pointer to RGBA pixel data, NULL on error
 */
unsigned char *render_get_pixel_buffer(renderer_t *renderer, int *width, int *height);

/**
 * @brief Get pixel buffer
 *
 * @param renderer Pointer to renderer
 * @param width Output: buffer width
 * @param height Output: buffer height
 * @return Pointer to RGBA pixel data
 */
const unsigned char *renderer_get_pixels(renderer_t *renderer, int *width, int *height);

/**
 * @brief Convert latitude/longitude to screen coordinates
 *
 * @param renderer Pointer to renderer
 * @param lat Latitude
 * @param lon Longitude
 * @param x Output: screen X coordinate
 * @param y Output: screen Y coordinate
 */
void renderer_coord_to_screen(renderer_t *renderer, float lat, float lon, float *x, float *y);

#ifdef __cplusplus
}
#endif

#endif /* RENDER_API_H */
