/**
 * @file render_api.c
 * @brief Rendering API implementation using NanoVG with AGGE backend
 */

#include "render_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef ENABLE_PNG_SAVE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#include "../graphics/trvg/trvg.h"

/* ============================================================================
 * Color Utilities
 * ============================================================================
 */

static void color_to_rgba(uint32_t color, unsigned char *r, unsigned char *g,
                          unsigned char *b, unsigned char *a)
{
    *r = (color >> 24) & 0xFF;
    *g = (color >> 16) & 0xFF;
    *b = (color >> 8) & 0xFF;
    *a = color & 0xFF;
}

/* ============================================================================
 * Software Rendering Primitives
 * ============================================================================
 */

static void set_pixel(renderer_t *r, int x, int y,
                      unsigned char cr, unsigned char cg,
                      unsigned char cb, unsigned char ca)
{
    if (x < 0 || x >= r->width || y < 0 || y >= r->height)
    {
        return;
    }

    int idx = (y * r->width + x) * 4;

    /* Alpha blending */
    float alpha = ca / 255.0f;
    float inv_alpha = 1.0f - alpha;

    r->pixels[idx + 0] = (unsigned char)(cr * alpha + r->pixels[idx + 0] * inv_alpha);
    r->pixels[idx + 1] = (unsigned char)(cg * alpha + r->pixels[idx + 1] * inv_alpha);
    r->pixels[idx + 2] = (unsigned char)(cb * alpha + r->pixels[idx + 2] * inv_alpha);
    r->pixels[idx + 3] = 255;
}

static void draw_line(renderer_t *r, float x0, float y0, float x1, float y1,
                      unsigned char cr, unsigned char cg,
                      unsigned char cb, unsigned char ca, float width)
{
    /* Bresenham's line algorithm with width */
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    if (length < 0.001f)
    {
        return;
    }

    /* Normalize */
    dx /= length;
    dy /= length;

    /* Perpendicular */
    float px = -dy;
    float py = dx;

    /* Draw thick line by drawing multiple parallel lines */
    float half_width = width / 2.0f;

    for (float w = -half_width; w <= half_width; w += 0.5f)
    {
        float sx = x0 + px * w;
        float sy = y0 + py * w;
        float ex = x1 + px * w;
        float ey = y1 + py * w;

        /* Simple line drawing */
        int steps = (int)(length + 1);
        for (int i = 0; i <= steps; i++)
        {
            float t = (float)i / steps;
            int x = (int)(sx + (ex - sx) * t);
            int y = (int)(sy + (ey - sy) * t);
            set_pixel(r, x, y, cr, cg, cb, ca);
        }
    }
}

static void draw_circle(renderer_t *r, float cx, float cy, float radius,
                        unsigned char cr, unsigned char cg,
                        unsigned char cb, unsigned char ca)
{
    /* Simple filled circle */
    int min_x = (int)(cx - radius);
    int max_x = (int)(cx + radius);
    int min_y = (int)(cy - radius);
    int max_y = (int)(cy + radius);

    float r_sq = radius * radius;

    for (int y = min_y; y <= max_y; y++)
    {
        for (int x = min_x; x <= max_x; x++)
        {
            float dx = x - cx;
            float dy = y - cy;
            if (dx * dx + dy * dy <= r_sq)
            {
                set_pixel(r, x, y, cr, cg, cb, ca);
            }
        }
    }
}

renderer_t *renderer_create(int width, int height, trvg_pixel_format_t format,
                            unsigned char *external_buffer)
{
    renderer_t *r = (renderer_t *)MAP_CALLOC(1, sizeof(renderer_t));
    if (!r)
    {
        return NULL;
    }

    r->width = width;
    r->height = height;

    int pixbyte;
    switch (format)
    {
    case TRVG_PIXEL_RGB565:
        pixbyte = 2;
        break;
    case TRVG_PIXEL_RGBA8888:
    case TRVG_PIXEL_BGRA8888:
        pixbyte = 4;
        break;
    default:
        pixbyte = 4;
        break;
    }

    if (external_buffer)
    {
        r->pixels = external_buffer;
    }
    else
    {
        r->pixels = (unsigned char *)MAP_CALLOC(width * height * pixbyte, 1);
        if (!r->pixels)
        {
            MAP_FREE(r);
            return NULL;
        }
    }

    if (!trvg_init(&r->trvg, r->pixels, width, height, width * pixbyte, format))
    {
        if (!external_buffer)
        {
            MAP_FREE(r->pixels);
        }
        MAP_FREE(r);
        return NULL;
    }

    MAP_PRINTF("TRVG renderer initialized: %dx%d, format=%d\n", width, height, format);

    r->view_min_lat = 0;
    r->view_min_lon = 0;
    r->view_max_lat = 1;
    r->view_max_lon = 1;

    return r;
}

void renderer_destroy(renderer_t *renderer)
{
    if (renderer)
    {
        trvg_reset(&renderer->trvg);
        // MAP_FREE(renderer->pixels);
        MAP_FREE(renderer);
    }
}

void renderer_set_viewport(renderer_t *renderer,
                           float min_lat, float min_lon,
                           float max_lat, float max_lon)
{
    if (!renderer)
    {
        return;
    }

    renderer->view_min_lat = min_lat;
    renderer->view_min_lon = min_lon;
    renderer->view_max_lat = max_lat;
    renderer->view_max_lon = max_lon;
}

void renderer_fit_path(renderer_t *renderer, const path_t *path, float padding_percent)
{
    if (!renderer || !path || path->count == 0)
    {
        return;
    }

    /* Find bounding box of path */
    /* cppcheck-suppress duplicateAssignExpression */
    float min_lat = path->points[0].lat;
    /* cppcheck-suppress duplicateAssignExpression */
    float max_lat = path->points[0].lat;
    /* cppcheck-suppress duplicateAssignExpression */
    float min_lon = path->points[0].lon;
    /* cppcheck-suppress duplicateAssignExpression */
    float max_lon = path->points[0].lon;

    for (uint32_t i = 1; i < path->count; i++)
    {
        if (path->points[i].lat < min_lat)
        {
            min_lat = path->points[i].lat;
        }
        if (path->points[i].lat > max_lat)
        {
            max_lat = path->points[i].lat;
        }
        if (path->points[i].lon < min_lon)
        {
            min_lon = path->points[i].lon;
        }
        if (path->points[i].lon > max_lon)
        {
            max_lon = path->points[i].lon;
        }
    }

    /* Add padding */
    float lat_range = max_lat - min_lat;
    float lon_range = max_lon - min_lon;
    float lat_padding = lat_range * padding_percent;
    float lon_padding = lon_range * padding_percent;

    /* Ensure minimum size */
    if (lat_range < 0.001f)
    {
        lat_padding = 0.005f;
    }
    if (lon_range < 0.001f)
    {
        lon_padding = 0.005f;
    }

    /* Apply padding */
    min_lat -= lat_padding;
    max_lat += lat_padding;
    min_lon -= lon_padding;
    max_lon += lon_padding;
    lat_range = max_lat - min_lat;
    lon_range = max_lon - min_lon;

    /*
     * Mercator projection correction:
     * At a given latitude, 1 degree of longitude covers less distance than 1 degree of latitude.
     * The ratio is cos(latitude). We need to adjust the viewport to maintain correct aspect ratio.
     */
    float center_lat = (min_lat + max_lat) / 2.0f;
    float lat_correction = cosf(center_lat * 3.14159265f / 180.0f);

    /* Convert geo ranges to "visual" units (longitude adjusted by latitude) */
    float geo_width = lon_range * lat_correction;  /* Adjusted longitude range */
    float geo_height = lat_range;                   /* Latitude range stays the same */

    /* Screen aspect ratio */
    float screen_aspect = (float)renderer->width / (float)renderer->height;
    /* Geographic aspect ratio */
    float geo_aspect = geo_width / geo_height;

    /* Adjust viewport to match screen aspect ratio while keeping map centered */
    if (geo_aspect < screen_aspect)
    {
        /* Map is taller than screen, expand longitude (width) */
        float target_geo_width = geo_height * screen_aspect;
        float target_lon_range = target_geo_width / lat_correction;
        float lon_expand = (target_lon_range - lon_range) / 2.0f;
        min_lon -= lon_expand;
        max_lon += lon_expand;
    }
    else
    {
        /* Map is wider than screen, expand latitude (height) */
        float target_geo_height = geo_width / screen_aspect;
        float lat_expand = (target_geo_height - lat_range) / 2.0f;
        min_lat -= lat_expand;
        max_lat += lat_expand;
    }

    renderer_set_viewport(renderer, min_lat, min_lon, max_lat, max_lon);
}

void renderer_coord_to_screen(renderer_t *renderer, float lat, float lon,
                              float *x, float *y)
{
    if (!renderer)
    {
        return;
    }

    float lat_range = renderer->view_max_lat - renderer->view_min_lat;
    float lon_range = renderer->view_max_lon - renderer->view_min_lon;

    /* Normalize to 0-1 */
    float norm_lon = (lon - renderer->view_min_lon) / lon_range;
    float norm_lat = (lat - renderer->view_min_lat) / lat_range;

    /* Convert to screen coordinates (Y is inverted) */
    *x = norm_lon * renderer->width;
    *y = (1.0f - norm_lat) * renderer->height;
}

void renderer_clear(renderer_t *renderer, uint32_t color)
{
    if (!renderer || !renderer->pixels)
    {
        return;
    }

    unsigned char r, g, b, a;
    color_to_rgba(color, &r, &g, &b, &a);

    /* Use ultra-fast clear (no alpha blending needed for clear) */
    trvg_fast_clear(&renderer->trvg, r, g, b);
}

void render_path(renderer_t *renderer, const path_t *path, const render_style_t *style)
{
    if (!renderer || !path || path->count < 2 || !style)
    {
        return;
    }

    trvg_context_t *ctx = &renderer->trvg;

    unsigned char pr, pg, pb, pa;
    color_to_rgba(style->path_color, &pr, &pg, &pb, &pa);

    /* Build points array for polyline */
    int16_t *points = (int16_t *)MAP_MALLOC(path->count * 2 * sizeof(int16_t));
    if (!points)
    {
        return;
    }

    for (uint32_t i = 0; i < path->count; i++)
    {
        float x, y;
        renderer_coord_to_screen(renderer, path->points[i].lat, path->points[i].lon, &x, &y);
        points[i * 2] = (int16_t)x;
        points[i * 2 + 1] = (int16_t)y;
    }
    /* Draw path as thick polyline with round joints */
    int thickness = (int)(style->path_width + 0.5f);
    if (thickness < 1)
    {
        thickness = 1;
    }
    /* Use optimized navigation path drawing function, fixes truncation, discontinuity and sharp corner issues */
    trvg_fast_nav_path(ctx, points, path->count, thickness, pr, pg, pb);

    /* Draw start point */
    unsigned char sr, sg, sb, sa;
    color_to_rgba(style->start_color, &sr, &sg, &sb, &sa);
    float sx, sy;
    renderer_coord_to_screen(renderer, path->points[0].lat, path->points[0].lon, &sx, &sy);
    trvg_fast_fill_circle(ctx, (int)sx, (int)sy, (int)style->point_radius, sr, sg, sb);

    /* Draw end point */
    unsigned char er, eg, eb, ea;
    color_to_rgba(style->end_color, &er, &eg, &eb, &ea);
    float ex, ey;
    renderer_coord_to_screen(renderer,
                             path->points[path->count - 1].lat,
                             path->points[path->count - 1].lon, &ex, &ey);
    trvg_fast_fill_circle(ctx, (int)ex, (int)ey, (int)style->point_radius, er, eg, eb);

    MAP_FREE(points);
}



/* ============================================================================
 * Themed Map Rendering (Google Maps style)
 * ============================================================================
 */

/* Get road color from theme based on road type */
static uint32_t get_road_color(const map_theme_t *theme, uint8_t road_type)
{
    switch (road_type)
    {
    case 6:
        return theme->road_motorway;    /* ROAD_MOTORWAY */
    case 5:
        return theme->road_trunk;       /* ROAD_TRUNK */
    case 4:
        return theme->road_primary;     /* ROAD_PRIMARY */
    case 3:
        return theme->road_secondary;   /* ROAD_SECONDARY */
    case 2:
        return theme->road_tertiary;    /* ROAD_TERTIARY */
    case 1:
        return theme->road_residential; /* ROAD_RESIDENTIAL */
    case 0:
        return theme->road_service;     /* ROAD_SERVICE */
    default:
        return theme->road_residential;
    }
}

/* Get road width from theme based on road type */
static float get_road_width(const map_theme_t *theme, uint8_t road_type)
{
    switch (road_type)
    {
    case 6:
        return theme->width_motorway;
    case 5:
        return theme->width_trunk;
    case 4:
        return theme->width_primary;
    case 3:
        return theme->width_secondary;
    case 2:
        return theme->width_tertiary;
    case 1:
        return theme->width_residential;
    case 0:
        return theme->width_service;
    default:
        return theme->width_residential;
    }
}

/* Get area color from theme based on area type */
static uint32_t get_area_color(const map_theme_t *theme, uint8_t area_type)
{
    switch (area_type)
    {
    case 0:
        return theme->water;            /* AREA_WATER */
    case 1:
        return theme->park;             /* AREA_PARK */
    case 2:
        return theme->forest;           /* AREA_FOREST */
    case 3:
        return theme->grass;            /* AREA_GRASS */
    case 4:
        return theme->building;         /* AREA_BUILDING */
    case 5:
        return theme->industrial;       /* AREA_INDUSTRIAL */
    case 6:
        return theme->commercial;       /* AREA_COMMERCIAL */
    case 7:
        return theme->residential_area; /* AREA_RESIDENTIAL */
    default:
        return theme->background;
    }
}



const unsigned char *renderer_get_pixels(renderer_t *renderer, int *width, int *height)
{
    if (!renderer)
    {
        return NULL;
    }

    if (width)
    {
        *width = renderer->width;
    }
    if (height)
    {
        *height = renderer->height;
    }

    return renderer->pixels;
}

/* ============================================================================
 * PNG Export (simple implementation)
 * ============================================================================
 */

/* Simple uncompressed BMP export as fallback */
static map_error_t save_bmp(renderer_t *renderer, const char *filepath)
{
    FILE *fp = fopen(filepath, "wb");
    if (!fp)
    {
        return MAP_ERR_FILE_NOT_FOUND;
    }

    int width = renderer->width;
    int height = renderer->height;
    int row_size = ((width * 3 + 3) / 4) * 4;  /* Rows are 4-byte aligned */
    int pixel_data_size = row_size * height;
    int file_size = 54 + pixel_data_size;

    /* BMP Header */
    unsigned char header[54] =
    {
        'B', 'M',                       /* Magic */
        0, 0, 0, 0,                     /* File size (filled below) */
        0, 0, 0, 0,                     /* Reserved */
        54, 0, 0, 0,                    /* Pixel data offset */
        40, 0, 0, 0,                    /* DIB header size */
        0, 0, 0, 0,                     /* Width (filled below) */
        0, 0, 0, 0,                     /* Height (filled below) */
        1, 0,                           /* Color planes */
        24, 0,                          /* Bits per pixel */
        0, 0, 0, 0,                     /* Compression */
        0, 0, 0, 0,                     /* Image size */
        0, 0, 0, 0,                     /* X pixels per meter */
        0, 0, 0, 0,                     /* Y pixels per meter */
        0, 0, 0, 0,                     /* Colors in color table */
        0, 0, 0, 0                      /* Important colors */
    };

    /* Fill in sizes */
    header[2] = file_size & 0xFF;
    header[3] = (file_size >> 8) & 0xFF;
    header[4] = (file_size >> 16) & 0xFF;
    header[5] = (file_size >> 24) & 0xFF;

    header[18] = width & 0xFF;
    header[19] = (width >> 8) & 0xFF;
    header[20] = (width >> 16) & 0xFF;
    header[21] = (width >> 24) & 0xFF;

    header[22] = height & 0xFF;
    header[23] = (height >> 8) & 0xFF;
    header[24] = (height >> 16) & 0xFF;
    header[25] = (height >> 24) & 0xFF;

    fwrite(header, 1, 54, fp);

    /* Write pixel data (bottom to top, BGR format) */
    unsigned char *row = (unsigned char *)MAP_MALLOC(row_size);
    if (!row)
    {
        fclose(fp);
        return MAP_ERR_OUT_OF_MEMORY;
    }

    for (int y = height - 1; y >= 0; y--)
    {
        for (int x = 0; x < width; x++)
        {
            int src_idx = (y * width + x) * 4;
            row[x * 3 + 0] = renderer->pixels[src_idx + 2];  /* B */
            row[x * 3 + 1] = renderer->pixels[src_idx + 1];  /* G */
            row[x * 3 + 2] = renderer->pixels[src_idx + 0];  /* R */
        }
        /* Pad row */
        for (int x = width * 3; x < row_size; x++)
        {
            row[x] = 0;
        }
        fwrite(row, 1, row_size, fp);
    }

    MAP_FREE(row);
    fclose(fp);
    return MAP_OK;
}

#ifdef ENABLE_PNG_SAVE
map_error_t render_save_png(renderer_t *renderer, const char *filepath)
{
    if (!renderer || !filepath)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    MAP_PRINTF("Saving image to: %s\n", filepath);

    /* Save as PNG using stb_image_write */
    if (stbi_write_png(filepath, renderer->width, renderer->height, 4, renderer->pixels,
                       renderer->width * 4))
    {
        return MAP_OK;
    }

    return MAP_ERR_INVALID_PARAM; // Using existing error code as fallback
}
#else
map_error_t render_save_png(renderer_t *renderer, const char *filepath)
{
    if (!renderer || !filepath)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    return MAP_OK;
}
#endif /* ENABLE_PNG_SAVE */
unsigned char *render_get_pixel_buffer(renderer_t *renderer, int *width, int *height)
{
    if (!renderer)
    {
        return NULL;
    }

    if (width)
    {
        *width = renderer->width;
    }
    if (height)
    {
        *height = renderer->height;
    }

    return renderer->pixels;
}

/* ============================================================================
 * Font Management for MCU Direct Memory Access
 * ============================================================================
 */

#include "../3rd/stb/stb_truetype.h"

/* Static font state - shared across all render_text calls */
static unsigned char *s_font_buffer =
    NULL;       /* Font buffer (allocated when loaded from file) */
static const unsigned char *s_font_data = NULL;   /* Font data pointer (direct memory or buffer) */
static stbtt_fontinfo s_font;
static int s_font_loaded = 0;
static int s_font_from_memory = 0;  /* 1 if font is from direct memory (no need to free) */

/**
 * @brief Set font data from memory address (for MCU direct memory access)
 */
bool render_set_font_from_memory(const unsigned char *font_data, size_t font_size)
{
    (void)font_size;  /* font_size is optional, stbtt can work without it */

    if (!font_data)
    {
        MAP_PRINTF("ERROR: render_set_font_from_memory - NULL font_data\n");
        return false;
    }

    /* If we had a previously allocated font buffer, free it */
    if (s_font_buffer && !s_font_from_memory)
    {
        MAP_FREE(s_font_buffer);
        s_font_buffer = NULL;
    }

    /* Set font data from memory directly (no allocation needed) */
    s_font_data = font_data;
    s_font_from_memory = 1;

    if (stbtt_InitFont(&s_font, s_font_data, 0))
    {
        s_font_loaded = 1;
        MAP_PRINTF("Font initialized from memory address: %p\n", (void *)font_data);
        return true;
    }
    else
    {
        MAP_PRINTF("ERROR: Failed to initialize font from memory!\n");
        s_font_loaded = 0;
        s_font_data = NULL;
        s_font_from_memory = 0;
        return false;
    }
}

/**
 * @brief Check if font is already loaded
 */
bool render_is_font_loaded(void)
{
    return s_font_loaded != 0;
}
void render_text(renderer_t *renderer, float x, float y, const char *text, float size,
                 uint32_t color)
{
    if (!renderer || !text)
    {
        return;
    }

    if (!s_font_loaded)
    {
        MAP_PRINTF("WARNING: render_text called but font not loaded. Call render_set_font_from_memory() first.\n");
        return;
    }

    if (!s_font_loaded)
    {
        MAP_PRINTF("WARNING: render_text called but font not loaded, text='%s'\n", text);
        return;
    }

    unsigned char r, g, b, a;
    color_to_rgba(color, &r, &g, &b, &a);

    float scale = stbtt_ScaleForPixelHeight(&s_font, size);
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&s_font, &ascent, &descent, &line_gap);
    float baseline = y + ascent * scale;

    trvg_pixel_format_t format = renderer->trvg.format;
    bool is_rgb565 = (format == TRVG_PIXEL_RGB565);

    while (*text)
    {
        int advance, lsb;
        int x0, y0, x1, y1;
        int code = (unsigned char) * text;

        if ((code & 0x80) != 0)
        {
            if ((code & 0xE0) == 0xE0)
            {
                code = ((code & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
                text += 2;
            }
        }

        stbtt_GetCodepointHMetrics(&s_font, code, &advance, &lsb);
        stbtt_GetCodepointBitmapBox(&s_font, code, scale, scale, &x0, &y0, &x1, &y1);

        int out_w = x1 - x0;
        int out_h = y1 - y0;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&s_font, 0, scale, code, &out_w, &out_h, 0, 0);

        if (bitmap)
        {
            if (is_rgb565)
            {
                uint16_t *pixels16 = (uint16_t *)renderer->pixels;
                uint16_t fg_r5 = (r >> 3) & 0x1F;
                uint16_t fg_g6 = (g >> 2) & 0x3F;
                uint16_t fg_b5 = (b >> 3) & 0x1F;

                for (int j = 0; j < out_h; j++)
                {
                    for (int i = 0; i < out_w; i++)
                    {
                        int px = (int)x + i + (int)(lsb * scale);
                        int py = (int)baseline + y0 + j;
                        if (px >= 0 && px < renderer->width && py >= 0 && py < renderer->height)
                        {
                            unsigned char glyph_alpha = bitmap[j * out_w + i];
                            if (glyph_alpha == 0)
                            {
                                continue;
                            }

                            int idx = py * renderer->width + px;

                            if (glyph_alpha == 255 && a == 255)
                            {
                                pixels16[idx] = (fg_r5 << 11) | (fg_g6 << 5) | fg_b5;
                            }
                            else
                            {
                                uint16_t alpha = ((uint16_t)glyph_alpha * a) >> 8;
                                uint16_t inv_alpha = 255 - alpha;

                                uint16_t bg_pixel = pixels16[idx];
                                uint16_t bg_r5 = (bg_pixel >> 11) & 0x1F;
                                uint16_t bg_g6 = (bg_pixel >> 5) & 0x3F;
                                uint16_t bg_b5 = bg_pixel & 0x1F;

                                uint16_t out_r5 = (fg_r5 * alpha + bg_r5 * inv_alpha) >> 8;
                                uint16_t out_g6 = (fg_g6 * alpha + bg_g6 * inv_alpha) >> 8;
                                uint16_t out_b5 = (fg_b5 * alpha + bg_b5 * inv_alpha) >> 8;

                                pixels16[idx] = (out_r5 << 11) | (out_g6 << 5) | out_b5;
                            }
                        }
                    }
                }
            }
            else
            {
                for (int j = 0; j < out_h; j++)
                {
                    for (int i = 0; i < out_w; i++)
                    {
                        int px = (int)x + i + (int)(lsb * scale);
                        int py = (int)baseline + y0 + j;
                        if (px >= 0 && px < renderer->width && py >= 0 && py < renderer->height)
                        {
                            float alpha = (bitmap[j * out_w + i] / 255.0f) * (a / 255.0f);
                            int idx = (py * renderer->width + px) * 4;
                            renderer->pixels[idx + 0] = (unsigned char)(r * alpha + renderer->pixels[idx + 0] * (1.0f - alpha));
                            renderer->pixels[idx + 1] = (unsigned char)(g * alpha + renderer->pixels[idx + 1] * (1.0f - alpha));
                            renderer->pixels[idx + 2] = (unsigned char)(b * alpha + renderer->pixels[idx + 2] * (1.0f - alpha));
                            renderer->pixels[idx + 3] = 255;
                        }
                    }
                }
            }
            stbtt_FreeBitmap(bitmap, NULL);
        }
        x += advance * scale;
        text++;
    }
}

/**
 * @brief Render text with stroke/outline based on the specified mode
 *
 * @param renderer Pointer to renderer
 * @param x X position
 * @param y Y position
 * @param text Text to render
 * @param size Font size
 * @param color Text fill color (RGBA)
 * @param stroke_mode Stroke mode (NONE, 4DIR, 8DIR, SDF)
 * @param stroke_color Stroke color (RGBA)
 * @param stroke_width Stroke width in pixels
 */
static void render_text_with_stroke(renderer_t *renderer, float x, float y,
                                    const char *text, float size, uint32_t color,
                                    text_stroke_mode_t stroke_mode, uint32_t stroke_color,
                                    uint8_t stroke_width)
{
    if (!renderer || !text)
    {
        return;
    }

    int sw = (stroke_width > 0) ? stroke_width : 1;

    switch (stroke_mode)
    {
    case TEXT_STROKE_NONE:
        /* No stroke, just render the text */
        render_text(renderer, x, y, text, size, color);
        break;

    case TEXT_STROKE_4DIR:
        /* 4-direction offset stroke (up/down/left/right) */
        for (int i = 1; i <= sw; i++)
        {
            render_text(renderer, x - i, y, text, size, stroke_color);
            render_text(renderer, x + i, y, text, size, stroke_color);
            render_text(renderer, x, y - i, text, size, stroke_color);
            render_text(renderer, x, y + i, text, size, stroke_color);
        }
        render_text(renderer, x, y, text, size, color);
        break;

    case TEXT_STROKE_8DIR:
        /* 8-direction offset stroke (including diagonals) */
        for (int i = 1; i <= sw; i++)
        {
            /* Cardinal directions */
            render_text(renderer, x - i, y, text, size, stroke_color);
            render_text(renderer, x + i, y, text, size, stroke_color);
            render_text(renderer, x, y - i, text, size, stroke_color);
            render_text(renderer, x, y + i, text, size, stroke_color);
            /* Diagonal directions */
            render_text(renderer, x - i, y - i, text, size, stroke_color);
            render_text(renderer, x + i, y - i, text, size, stroke_color);
            render_text(renderer, x - i, y + i, text, size, stroke_color);
            render_text(renderer, x + i, y + i, text, size, stroke_color);
        }
        render_text(renderer, x, y, text, size, color);
        break;

    case TEXT_STROKE_SDF:
        /* SDF (Signed Distance Field) rendering
         * Note: This is a simplified implementation using stbtt_GetCodepointSDF.
         * For now, fall back to 4DIR mode as full SDF requires significant refactoring.
         * TODO: Implement proper SDF text rendering with caching.
         */
        for (int i = 1; i <= sw; i++)
        {
            render_text(renderer, x - i, y, text, size, stroke_color);
            render_text(renderer, x + i, y, text, size, stroke_color);
            render_text(renderer, x, y - i, text, size, stroke_color);
            render_text(renderer, x, y + i, text, size, stroke_color);
        }
        render_text(renderer, x, y, text, size, color);
        break;

    default:
        /* Unknown mode, render without stroke */
        render_text(renderer, x, y, text, size, color);
        break;
    }
}

/* ============================================================================
 * Label Rendering
 * ============================================================================
 */

/* Get label font size based on label type and priority */
static float get_label_font_size(uint8_t label_type, uint8_t priority, uint8_t road_type)
{
    float base_size = 12.0f;

    switch (label_type)
    {
    case LABEL_ROAD:
        /* Road label size based on road type */
        switch (road_type)
        {
        case ROAD_MOTORWAY:
            base_size = 14.0f;
            break;
        case ROAD_TRUNK:
            base_size = 13.0f;
            break;
        case ROAD_PRIMARY:
            base_size = 12.0f;
            break;
        case ROAD_SECONDARY:
            base_size = 11.0f;
            break;
        case ROAD_TERTIARY:
            base_size = 10.0f;
            break;
        default:
            base_size = 9.0f;
            break;
        }
        break;
    case LABEL_PLACE:
        /* Place names - much larger for cities and districts */
        /* priority 10=city, 8=town/county, 7=district/municipality, 6=village/borough, 5=suburb */
        if (priority >= 10)
        {
            base_size = 24.0f;  /* City - very large */
        }
        else if (priority >= 8)
        {
            base_size = 18.0f;  /* Town/County */
        }
        else if (priority >= 7)
        {
            base_size = 16.0f;  /* District/Municipality */
        }
        else if (priority >= 6)
        {
            base_size = 14.0f;  /* Village/Borough */
        }
        else if (priority >= 5)
        {
            base_size = 13.0f;  /* Suburb */
        }
        else
        {
            base_size = 12.0f;  /* Other places */
        }
        break;
    case LABEL_WATER:
        base_size = 12.0f;
        break;
    case LABEL_PARK:
        base_size = 11.0f;
        break;
    case LABEL_POI:
        base_size = 10.0f;
        break;
    default:
        base_size = 10.0f;
        break;
    }

    return base_size * 22.0f / 12.0f;
}

/* Get label color based on label type */
static uint32_t get_label_color(uint8_t label_type, const map_theme_t *theme)
{
    (void)theme;  /* May use theme colors in the future */

    switch (label_type)
    {
    case LABEL_ROAD:
        return 0x333333FF;  /* Dark gray for road names */
    case LABEL_PLACE:
        return 0x000000FF;  /* Black for place names */
    case LABEL_WATER:
        return 0x0066AAFF;  /* Blue for water names */
    case LABEL_PARK:
        return 0x228B22FF;  /* Forest green for park names */
    case LABEL_POI:
        return 0x8B4513FF;  /* Saddle brown for POI */
    default:
        return 0x444444FF;  /* Default gray */
    }
}
#ifndef MAX_ROAD_LABELS
#define MAX_ROAD_LABELS 6
#define MAX_WATER_LABELS 4
#define MAX_PARK_LABELS 4
#define MAX_POI_LABELS 4
#define MAX_PLACE_LABELS 4
#define MAX_TOTAL_CANDIDATES 500
#define MAX_ROAD_NAMES 8
#define ROAD_NAME_LIMIT_OPT 8  /* Increased to collect more roads before filtering */
#endif

/* NOTE: use compile-time constants for local arrays (MSVC doesn't allow VLA). */
#define ROAD_LABEL_LIMIT 6
#define ROAD_NAME_LIMIT 8
#define ROAD_PRIO_MAX 16



/* ============================================================================
 * Render Options API
 * ============================================================================
 */

void render_options_init(render_options_t *options)
{
    if (!options)
    {
        return;
    }
    options->flags = RENDER_OPT_ALL;
    options->road_types = ROAD_TYPE_ALL;       /* All road types */
    options->label_types = LABEL_TYPE_ALL;     /* All label types */
    options->road_label_types = ROAD_TYPE_ALL; /* Labels for all road types */
    options->min_road_segment_length = 0.0f;   /* No road filtering by default */
    options->min_area_segment_length = 0.0f;   /* No area segment filtering by default */
    options->min_area_size = 0.0f;             /* No area size filtering by default */
    options->text_stroke_mode = TEXT_STROKE_NONE;  /* 4-direction stroke by default */
    options->text_stroke_color = 0xFFFFFFFF;   /* White stroke */
    options->text_stroke_width = 1;            /* 1 pixel stroke width */
}

/* Helper: Check if an area type should be rendered based on options */
static int should_render_area_type(uint8_t area_type, uint32_t flags)
{
    switch (area_type)
    {
    case 0:
        return (flags & RENDER_OPT_WATER) != 0;        /* AREA_WATER */
    case 1:
        return (flags & RENDER_OPT_PARKS) != 0;        /* AREA_PARK */
    case 2:
        return (flags & RENDER_OPT_PARKS) != 0;        /* AREA_FOREST */
    case 3:
        return (flags & RENDER_OPT_GRASS) != 0;        /* AREA_GRASS */
    case 4:
        return (flags & RENDER_OPT_BUILDINGS) != 0;    /* AREA_BUILDING */
    case 5:
        return (flags & RENDER_OPT_INDUSTRIAL) != 0;   /* AREA_INDUSTRIAL */
    case 6:
        return (flags & RENDER_OPT_COMMERCIAL) != 0;   /* AREA_COMMERCIAL */
    case 7:
        return (flags & RENDER_OPT_RESIDENTIAL) != 0;  /* AREA_RESIDENTIAL */
    default:
        return 1;  /* Unknown types rendered by default */
    }
}

/* Render areas with options filter */
static void render_areas_with_options(renderer_t *renderer, const map_t *map,
                                      const map_theme_t *theme, const render_options_t *options)
{
    if (!renderer || !map || !map->loaded)
    {
        return;
    }
    if (!map->areas || !map->area_points || map->header.area_count == 0)
    {
        return;
    }

    trvg_context_t *ctx = &renderer->trvg;
    uint32_t flags = options ? options->flags : RENDER_OPT_ALL;

    /* Small area filtering parameters */
    float min_area_size = options ? options->min_area_size : 0.0f;

    /* Small segment filtering parameters for area polygons */
    float min_segment_len = options ? options->min_area_segment_length : 0.0f;
    float min_segment_len_sq = min_segment_len * min_segment_len;

    /* Get screen bounds for clipping with some margin */
    const float margin = 50.0f;
    const float screen_min_x = -margin;
    const float screen_min_y = -margin;
    const float screen_max_x = (float)renderer->width + margin;
    const float screen_max_y = (float)renderer->height + margin;

    /* Check if area spatial index is available (v7+) */
    int use_area_spatial_index = (map->area_cell_offsets != NULL && map->area_cell_indices != NULL &&
                                  map->header.grid_cols > 0 && map->header.grid_rows > 0);

    /* Get viewport bounds in geo coordinates */
    float view_min_lat = renderer->view_min_lat;
    float view_max_lat = renderer->view_max_lat;
    float view_min_lon = renderer->view_min_lon;
    float view_max_lon = renderer->view_max_lon;

    /* Temporary area visited flags for spatial index (to avoid duplicates) */
    uint8_t *area_visited = NULL;
    uint32_t areas_checked = 0;
    uint32_t areas_rendered = 0;
    uint32_t cells_checked = 0;

    if (use_area_spatial_index)
    {
        area_visited = (uint8_t *)MAP_CALLOC((map->header.area_count + 7) / 8, 1);
    }

    /* Macro to render a single area by index */
#define RENDER_AREA(area_idx) do { \
        uint32_t area_i = (area_idx); \
        map_area_t *area = &map->areas[area_i]; \
        if (area->point_count < 3) continue; \
        if (!should_render_area_type(area->area_type, flags)) continue; \
        \
        uint32_t color = get_area_color(theme, area->area_type); \
        unsigned char cr, cg, cb, ca; \
        color_to_rgba(color, &cr, &cg, &cb, &ca); \
        \
        static int16_t poly_points[2048]; \
        const int MAX_POLY_POINTS = 1024; \
        int poly_count = 0; \
        float last_x = 0, last_y = 0; \
        \
        float area_min_x = 1e30f, area_min_y = 1e30f; \
        float area_max_x = -1e30f, area_max_y = -1e30f; \
        \
        int sample_step = 1; \
        if (area->point_count > MAX_POLY_POINTS) { \
            sample_step = (area->point_count + MAX_POLY_POINTS - 1) / MAX_POLY_POINTS; \
        } \
        \
        for (uint32_t j = 0; j < area->point_count; j++) { \
            coord_t *pt = &map->area_points[area->point_offset + j]; \
            float x, y; \
            renderer_coord_to_screen(renderer, pt->lat, pt->lon, &x, &y); \
            if (x < area_min_x) area_min_x = x; \
            if (x > area_max_x) area_max_x = x; \
            if (y < area_min_y) area_min_y = y; \
            if (y > area_max_y) area_max_y = y; \
        } \
        \
        if (area_max_x < screen_min_x || area_min_x > screen_max_x || \
            area_max_y < screen_min_y || area_min_y > screen_max_y) continue; \
        \
        if (min_area_size > 0.0f) { \
            float area_width = area_max_x - area_min_x; \
            float area_height = area_max_y - area_min_y; \
            float area_size = area_width * area_height; \
            if (area_size < min_area_size) continue; \
        } \
        \
        for (uint32_t j = 0; j < area->point_count && poly_count < MAX_POLY_POINTS - 1; j += sample_step) { \
            coord_t *pt = &map->area_points[area->point_offset + j]; \
            float x, y; \
            renderer_coord_to_screen(renderer, pt->lat, pt->lon, &x, &y); \
            if (min_segment_len > 0.0f && poly_count > 0) { \
                float dx = x - last_x; \
                float dy = y - last_y; \
                float dist_sq = dx * dx + dy * dy; \
                if (dist_sq < min_segment_len_sq) continue; \
            } \
            poly_points[poly_count * 2] = (int16_t)x; \
            poly_points[poly_count * 2 + 1] = (int16_t)y; \
            last_x = x; \
            last_y = y; \
            poly_count++; \
        } \
        \
        if (poly_count < 3) continue; \
        areas_rendered++; \
        trvg_fast_fill_polygon(ctx, poly_points, poly_count, cr, cg, cb); \
    } while(0)

    if (use_area_spatial_index)
    {
        /* Use spatial index for fast area lookup */
        float lat_range = map->header.max_lat - map->header.min_lat;
        float lon_range = map->header.max_lon - map->header.min_lon;
        float cell_lat = lat_range / map->header.grid_rows;
        float cell_lon = lon_range / map->header.grid_cols;

        /* Calculate which grid cells overlap with viewport */
        int col_start = (int)((view_min_lon - map->header.min_lon) / cell_lon);
        int col_end = (int)((view_max_lon - map->header.min_lon) / cell_lon);
        int row_start = (int)((view_min_lat - map->header.min_lat) / cell_lat);
        int row_end = (int)((view_max_lat - map->header.min_lat) / cell_lat);

        /* Clamp to valid range */
        if (col_start < 0)
        {
            col_start = 0;
        }
        if (col_end >= map->header.grid_cols)
        {
            col_end = map->header.grid_cols - 1;
        }
        if (row_start < 0)
        {
            row_start = 0;
        }
        if (row_end >= map->header.grid_rows)
        {
            row_end = map->header.grid_rows - 1;
        }

        cells_checked = (row_end - row_start + 1) * (col_end - col_start + 1);

        /* Iterate only cells in viewport */
        for (int row = row_start; row <= row_end; row++)
        {
            for (int col = col_start; col <= col_end; col++)
            {
                int cell_idx = row * map->header.grid_cols + col;
                uint32_t start_offset = map->area_cell_offsets[cell_idx];
                uint32_t end_offset = map->area_cell_offsets[cell_idx + 1];

                for (uint32_t ai = start_offset; ai < end_offset; ai++)
                {
                    uint32_t area_idx = map->area_cell_indices[ai];
                    areas_checked++;

                    /* Check if already visited (avoid duplicates from overlapping cells) */
                    uint32_t byte_idx = area_idx / 8;
                    uint8_t bit_mask = 1 << (area_idx % 8);
                    if (area_visited[byte_idx] & bit_mask)
                    {
                        continue;
                    }
                    area_visited[byte_idx] |= bit_mask;

                    RENDER_AREA(area_idx);
                }
            }
        }
        if (area_visited)
        {
            MAP_FREE(area_visited);
        }

        MAP_PRINTF(">>> AREA SPATIAL INDEX ACTIVE: checked %u/%u areas (%.2f%%), rendered %u, cells %u/%u <<<\n",
                   areas_checked, map->header.area_count,
                   (double)(100.0f * areas_checked / map->header.area_count),
                   areas_rendered, cells_checked,
                   (uint32_t)(map->header.grid_cols * map->header.grid_rows));
    }
    else
    {
        /* Fallback: iterate all areas (legacy mode for v1-v6 maps) */
        for (uint32_t i = 0; i < map->header.area_count; i++)
        {
            areas_checked++;
            RENDER_AREA(i);
        }
        MAP_PRINTF(">>> NO AREA SPATIAL INDEX: checked %u areas, rendered %u <<<\n",
                   areas_checked, areas_rendered);
    }

#undef RENDER_AREA
}


/* Render roads with options filter */
static void render_roads_with_options(renderer_t *renderer, const map_t *map,
                                      const map_theme_t *theme, const render_options_t *options)
{
    if (!renderer || !map || !map->loaded)
    {
        return;
    }

    trvg_context_t *ctx = &renderer->trvg;
    uint8_t road_type_mask = options ? options->road_types : 0x7F;
    float min_segment_len = options ? options->min_road_segment_length : 0.0f;
    float min_segment_len_sq = min_segment_len * min_segment_len;

    /* Get screen bounds for clipping with some margin */
    const float margin = 50.0f;
    const float screen_min_x = -margin;
    const float screen_min_y = -margin;
    const float screen_max_x = (float)renderer->width + margin;
    const float screen_max_y = (float)renderer->height + margin;

    /* Check if spatial index is available (v6+) */
    int use_spatial_index = (map->cell_offsets != NULL && map->cell_edges != NULL &&
                             map->header.grid_cols > 0 && map->header.grid_rows > 0);

    /* Get viewport bounds in geo coordinates (already stored in renderer) */
    float view_min_lat = renderer->view_min_lat;
    float view_max_lat = renderer->view_max_lat;
    float view_min_lon = renderer->view_min_lon;
    float view_max_lon = renderer->view_max_lon;

    /* Temporary edge visited flags for spatial index (to avoid duplicates) */
    uint8_t *edge_visited = NULL;
    uint32_t edges_checked = 0;
    uint32_t edges_rendered = 0;
    uint32_t cells_checked = 0;

    if (use_spatial_index)
    {
        edge_visited = (uint8_t *)MAP_CALLOC((map->header.edge_count + 7) / 8, 1);
    }

    //MAP_PRINTF("road%d\n", map->header.edge_count);

    /* Draw roads from lowest to highest priority (service -> motorway) */
    for (int level = 0; level <= 6; level++)
    {
        /* Check if this road type should be rendered */
        if (!(road_type_mask & (1 << level)))
        {
            continue;
        }

        uint32_t color = get_road_color(theme, level);
        float width = get_road_width(theme, level);
        unsigned char r, g, b, a;
        color_to_rgba(color, &r, &g, &b, &a);

        if (use_spatial_index)
        {
            /* Use spatial index for fast edge lookup */
            float lat_range = map->header.max_lat - map->header.min_lat;
            float lon_range = map->header.max_lon - map->header.min_lon;
            float cell_lat = lat_range / map->header.grid_rows;
            float cell_lon = lon_range / map->header.grid_cols;

            /* Calculate which grid cells overlap with viewport */
            int col_start = (int)((view_min_lon - map->header.min_lon) / cell_lon);
            int col_end = (int)((view_max_lon - map->header.min_lon) / cell_lon);
            int row_start = (int)((view_min_lat - map->header.min_lat) / cell_lat);
            int row_end = (int)((view_max_lat - map->header.min_lat) / cell_lat);

            /* Clamp to valid range */
            if (col_start < 0)
            {
                col_start = 0;
            }
            if (col_end >= map->header.grid_cols)
            {
                col_end = map->header.grid_cols - 1;
            }
            if (row_start < 0)
            {
                row_start = 0;
            }
            if (row_end >= map->header.grid_rows)
            {
                row_end = map->header.grid_rows - 1;
            }

            cells_checked += (row_end - row_start + 1) * (col_end - col_start + 1);

            /* Iterate only cells in viewport */
            for (int row = row_start; row <= row_end; row++)
            {
                for (int col = col_start; col <= col_end; col++)
                {
                    int cell_idx = row * map->header.grid_cols + col;
                    uint32_t start_offset = map->cell_offsets[cell_idx];
                    uint32_t end_offset = map->cell_offsets[cell_idx + 1];

                    for (uint32_t ei = start_offset; ei < end_offset; ei++)
                    {
                        uint32_t i = map->cell_edges[ei];
                        edges_checked++;

                        /* Check if already visited (avoid duplicates from overlapping cells) */
                        uint32_t byte_idx = i / 8;
                        uint8_t bit_mask = 1 << (i % 8);
                        if (edge_visited[byte_idx] & bit_mask)
                        {
                            continue;
                        }
                        edge_visited[byte_idx] |= bit_mask;

                        if (map->edges[i].road_type != level)
                        {
                            continue;
                        }

                        uint32_t from = map->edges[i].from_node;
                        uint32_t to = map->edges[i].to_node;

                        /* Only draw one direction to avoid overdraw */
                        if (from > to)
                        {
                            continue;
                        }

                        float x0, y0, x1, y1;
                        renderer_coord_to_screen(renderer,
                                                 map->nodes[from].lat, map->nodes[from].lon, &x0, &y0);
                        renderer_coord_to_screen(renderer,
                                                 map->nodes[to].lat, map->nodes[to].lon, &x1, &y1);

                        /* Skip edges completely outside the screen */
                        float edge_min_x = (x0 < x1) ? x0 : x1;
                        float edge_max_x = (x0 > x1) ? x0 : x1;
                        float edge_min_y = (y0 < y1) ? y0 : y1;
                        float edge_max_y = (y0 > y1) ? y0 : y1;
                        if (edge_max_x < screen_min_x || edge_min_x > screen_max_x ||
                            edge_max_y < screen_min_y || edge_min_y > screen_max_y)
                        {
                            continue;
                        }

                        /* Small segment filtering */
                        if (min_segment_len > 0.0f)
                        {
                            float dx = x1 - x0;
                            float dy = y1 - y0;
                            float len_sq = dx * dx + dy * dy;
                            if (len_sq < min_segment_len_sq)
                            {
                                continue;
                            }
                        }

                        int thickness = (int)(width + 0.5f);
                        if (thickness < 1)
                        {
                            thickness = 1;
                        }
                        edges_rendered++;
                        //MAP_PRINTF("road i:%d\n", i);
                        trvg_fast_thick_line(ctx, (int)x0, (int)y0, (int)x1, (int)y1,
                                             thickness, r, g, b);
                    }
                }
            }

            /* Clear visited flags for next level */
            if (edge_visited)
            {
                memset(edge_visited, 0, (map->header.edge_count + 7) / 8);
            }
        }
        else
        {
            /* Fallback: iterate all edges (legacy mode for v1-v5 maps) */
            for (uint32_t i = 0; i < map->header.edge_count; i++)
            {
                edges_checked++;
                if (map->edges[i].road_type != level)
                {
                    continue;
                }

                uint32_t from = map->edges[i].from_node;
                uint32_t to = map->edges[i].to_node;

                /* Only draw one direction to avoid overdraw */
                if (from > to)
                {
                    continue;
                }

                float x0, y0, x1, y1;
                renderer_coord_to_screen(renderer,
                                         map->nodes[from].lat, map->nodes[from].lon, &x0, &y0);
                renderer_coord_to_screen(renderer,
                                         map->nodes[to].lat, map->nodes[to].lon, &x1, &y1);

                /* Skip edges completely outside the screen */
                float edge_min_x = (x0 < x1) ? x0 : x1;
                float edge_max_x = (x0 > x1) ? x0 : x1;
                float edge_min_y = (y0 < y1) ? y0 : y1;
                float edge_max_y = (y0 > y1) ? y0 : y1;
                if (edge_max_x < screen_min_x || edge_min_x > screen_max_x ||
                    edge_max_y < screen_min_y || edge_min_y > screen_max_y)
                {
                    continue;
                }

                /* Small segment filtering */
                if (min_segment_len > 0.0f)
                {
                    float dx = x1 - x0;
                    float dy = y1 - y0;
                    float len_sq = dx * dx + dy * dy;
                    if (len_sq < min_segment_len_sq)
                    {
                        continue;
                    }
                }

                int thickness = (int)(width + 0.5f);
                if (thickness < 1)
                {
                    thickness = 1;
                }
                edges_rendered++;
                //MAP_PRINTF("road i:%d\n", i);
                trvg_fast_thick_line(ctx, (int)x0, (int)y0, (int)x1, (int)y1,
                                     thickness, r, g, b);
            }
        }
    }

    /* Print spatial index statistics */
    if (use_spatial_index)
    {
        uint32_t total_cells = (uint32_t)map->header.grid_cols * (uint32_t)map->header.grid_rows;
        MAP_PRINTF(">>> SPATIAL INDEX ACTIVE: checked %u/%u edges (%.2f%%), rendered %u, cells %u/%u <<<\n",
                   edges_checked, map->header.edge_count,
                   (double)(100.0f * edges_checked / map->header.edge_count),
                   edges_rendered, cells_checked, total_cells);
    }
    else
    {
        MAP_PRINTF(">>> NO SPATIAL INDEX: checked %u edges (legacy mode), rendered %u <<<\n",
                   edges_checked, edges_rendered);
    }

    if (edge_visited)
    {
        MAP_FREE(edge_visited);
    }
}





/* Helper: Check if a label type should be rendered based on options */
static int should_render_label_type(uint8_t label_type, uint8_t label_types_mask)
{
    switch (label_type)
    {
    case LABEL_ROAD:
        return (label_types_mask & LABEL_TYPE_ROAD) != 0;
    case LABEL_PLACE:
        return (label_types_mask & LABEL_TYPE_PLACE) != 0;
    case LABEL_POI:
        return (label_types_mask & LABEL_TYPE_POI) != 0;
    case LABEL_WATER:
        return (label_types_mask & LABEL_TYPE_WATER) != 0;
    case LABEL_PARK:
        return (label_types_mask & LABEL_TYPE_PARK) != 0;
    default:
        return (label_types_mask & LABEL_TYPE_OTHER) != 0;
    }
}

/* Helper: Check if a road type should have labels based on options */
static int should_label_road_type(uint8_t road_type, uint8_t road_label_mask)
{
    if (road_type > 6)
    {
        return 0;
    }
    return (road_label_mask & (1 << road_type)) != 0;
}

/* Render labels with options filter */
static void render_labels_with_options(renderer_t *renderer, const map_t *map,
                                       const map_theme_t *theme, const render_options_t *options)
{
    if (!renderer || !map || !map->loaded)
    {
        return;
    }
    if (!map->labels || !map->label_text || map->header.label_count == 0)
    {
        return;
    }    /* Get options or use defaults */
    uint8_t label_types = options ? options->label_types : LABEL_TYPE_ALL;
    uint8_t road_label_types = options ? options->road_label_types : ROAD_TYPE_ALL;
    text_stroke_mode_t stroke_mode = options ? options->text_stroke_mode : TEXT_STROKE_4DIR;
    uint32_t stroke_color = options ? options->text_stroke_color : 0xFFFFFFFF;
    uint8_t stroke_width = options ? options->text_stroke_width : 1;
    // MAP_PRINTF("Rendering labels with options (label_types=0x%02X, road_label_types=0x%02X)...\n",
    //            label_types, road_label_types);

    /* Configuration: max labels per type - uses macros defined earlier in this file */

    int road_label_count = 0;
    int water_label_count = 0;
    int park_label_count = 0;
    int poi_label_count = 0;
    int place_label_count = 0;
    int rendered_count = 0;

    /* Create temporary arrays for sorting by priority */
    typedef struct
    {
        uint32_t index;
        uint8_t priority;
        float sx, sy;
        uint16_t area_hectare;  /* Area in hectares (10000 sqm) for water/park sorting */
    } label_candidate_opt_t;

    static label_candidate_opt_t road_candidates[MAX_TOTAL_CANDIDATES];
    static label_candidate_opt_t water_candidates[MAX_TOTAL_CANDIDATES];
    static label_candidate_opt_t park_candidates[MAX_TOTAL_CANDIDATES];
    static label_candidate_opt_t poi_candidates[MAX_TOTAL_CANDIDATES];
    static label_candidate_opt_t place_candidates[MAX_TOTAL_CANDIDATES];

    int road_candidate_count = 0;
    int water_candidate_count = 0;
    int park_candidate_count = 0;
    int poi_candidate_count = 0;
    int place_candidate_count = 0;

    /* First pass: collect all visible labels by type */
    for (uint32_t i = 0; i < map->header.label_count; i++)
    {
        map_label_t *label = &map->labels[i];

        /* Check if this label type should be rendered */
        if (!should_render_label_type(label->label_type, label_types))
        {
            continue;
        }

        /* For road labels, also check road type filter */
        if (label->label_type == LABEL_ROAD)
        {
            if (!should_label_road_type(label->road_type, road_label_types))
            {
                continue;
            }
        }

        /* Get screen position */
        float sx, sy;
        renderer_coord_to_screen(renderer, label->lat, label->lon, &sx, &sy);

        /* Skip if outside screen */
        if (sx < -100 || sx > renderer->width + 100 ||
            sy < -50 || sy > renderer->height + 50)
        {
            continue;
        }

        /* Add to appropriate candidate list */
        label_candidate_opt_t candidate = {i, label->priority, sx, sy, label->area_hectare};

        switch (label->label_type)
        {
        case LABEL_PLACE:
            if (place_candidate_count < MAX_TOTAL_CANDIDATES)
            {
                place_candidates[place_candidate_count++] = candidate;
            }
            break;
        case LABEL_ROAD:
            if (road_candidate_count < MAX_TOTAL_CANDIDATES)
            {
                road_candidates[road_candidate_count++] = candidate;
            }
            break;
        case LABEL_WATER:
            if (label->priority >= 5 && water_candidate_count < MAX_TOTAL_CANDIDATES)
            {
                water_candidates[water_candidate_count++] = candidate;
            }
            break;
        case LABEL_PARK:
            if (label->priority >= 4 && park_candidate_count < MAX_TOTAL_CANDIDATES)
            {
                park_candidates[park_candidate_count++] = candidate;
            }
            break;
        case LABEL_POI:
            if (label->priority >= 4 && poi_candidate_count < MAX_TOTAL_CANDIDATES)
            {
                poi_candidates[poi_candidate_count++] = candidate;
            }
            break;
        }
    }

    /* Sort each candidate list by priority (descending) */
#define SORT_CANDIDATES_OPT(arr, count) \
    for (int i = 0; i < (count) - 1; i++) { \
        for (int j = 0; j < (count) - 1 - i; j++) { \
            if ((arr)[j].priority < (arr)[j+1].priority) { \
                label_candidate_opt_t tmp = (arr)[j]; \
                (arr)[j] = (arr)[j+1]; \
                (arr)[j+1] = tmp; \
            } \
        } \
    }

    SORT_CANDIDATES_OPT(place_candidates, place_candidate_count);

    /* Sort road candidates by road_type (highest first: motorway=6 > trunk=5 > ... > service=0)
     * This ensures high-priority roads (motorway/trunk) are processed first when grouping,
     * so they won't be dropped due to ROAD_NAME_LIMIT_OPT */
    for (int i = 0; i < road_candidate_count - 1; i++)
    {
        for (int j = 0; j < road_candidate_count - 1 - i; j++)
        {
            map_label_t *label_j = &map->labels[road_candidates[j].index];
            map_label_t *label_j1 = &map->labels[road_candidates[j + 1].index];
            if (label_j->road_type < label_j1->road_type)
            {
                label_candidate_opt_t tmp = road_candidates[j];
                road_candidates[j] = road_candidates[j + 1];
                road_candidates[j + 1] = tmp;
            }
        }
    }

    /* Sort water candidates: purely by area (descending) - larger water bodies first */
    for (int i = 0; i < water_candidate_count - 1; i++)
    {
        for (int j = 0; j < water_candidate_count - 1 - i; j++)
        {
            /* Sort by area only (larger first) */
            if (water_candidates[j].area_hectare < water_candidates[j + 1].area_hectare)
            {
                label_candidate_opt_t tmp = water_candidates[j];
                water_candidates[j] = water_candidates[j + 1];
                water_candidates[j + 1] = tmp;
            }
        }
    }

    // /* Debug: print water candidates after sorting */
    // if (water_candidate_count > 0)
    // {
    //     MAP_PRINTF("[WATER] Found %d water candidates (sorted by area):\n", water_candidate_count);
    //     for (int i = 0; i < water_candidate_count && i < 10; i++)
    //     {
    //         map_label_t *label = &map->labels[water_candidates[i].index];
    //         const char *name = map->label_text + label->text_offset;
    //         MAP_PRINTF("  [%d] %s: priority=%d, area=%d hectares\n",
    //                    i, name, water_candidates[i].priority, water_candidates[i].area_hectare);
    //     }
    // }

    /* Sort park candidates: purely by area (descending) - larger parks/forests first */
    for (int i = 0; i < park_candidate_count - 1; i++)
    {
        for (int j = 0; j < park_candidate_count - 1 - i; j++)
        {
            /* Sort by area only (larger first) */
            if (park_candidates[j].area_hectare < park_candidates[j + 1].area_hectare)
            {
                label_candidate_opt_t tmp = park_candidates[j];
                park_candidates[j] = park_candidates[j + 1];
                park_candidates[j + 1] = tmp;
            }
        }
    }

    // /* Debug: print park candidates after sorting */
    // if (park_candidate_count > 0)
    // {
    //     MAP_PRINTF("[PARK] Found %d park candidates (sorted by area):\n", park_candidate_count);
    //     for (int i = 0; i < park_candidate_count && i < 10; i++)
    //     {
    //         map_label_t *label = &map->labels[park_candidates[i].index];
    //         const char *name = map->label_text + label->text_offset;
    //         MAP_PRINTF("  [%d] %s: priority=%d, area=%d hectares\n",
    //                    i, name, park_candidates[i].priority, park_candidates[i].area_hectare);
    //     }
    // }

    /* park_candidates already sorted by area above, skip priority sort */
    SORT_CANDIDATES_OPT(poi_candidates,
                        poi_candidate_count);    /* Helper macro to render a single label with configurable stroke */
#define DO_RENDER_LABEL_OPT(cand, label) { \
        const char *text = map->label_text + (label)->text_offset; \
        float font_size = get_label_font_size((label)->label_type, (label)->priority, (label)->road_type); \
        uint32_t color = get_label_color((label)->label_type, theme); \
        float text_width_estimate = font_size * (label)->text_length * 0.5f; \
        float draw_x = (cand)->sx - text_width_estimate / 2.0f; \
        float draw_y = (cand)->sy - font_size / 2.0f; \
        render_text_with_stroke(renderer, draw_x, draw_y, text, font_size, color, \
                                stroke_mode, stroke_color, stroke_width); \
        rendered_count++; \
    }

    /* Render place labels */
    if (label_types & LABEL_TYPE_PLACE)
    {
        for (int c = 0; c < place_candidate_count && place_label_count < MAX_PLACE_LABELS; c++)
        {
            label_candidate_opt_t *cand = &place_candidates[c];
            map_label_t *label = &map->labels[cand->index];
            DO_RENDER_LABEL_OPT(cand, label);
            place_label_count++;
        }
    }

    /* Render water labels */
    if (label_types & LABEL_TYPE_WATER)
    {
        for (int c = 0; c < water_candidate_count && water_label_count < MAX_WATER_LABELS; c++)
        {
            label_candidate_opt_t *cand = &water_candidates[c];
            map_label_t *label = &map->labels[cand->index];
            DO_RENDER_LABEL_OPT(cand, label);
            water_label_count++;
        }
    }

    /* Render park labels */
    if (label_types & LABEL_TYPE_PARK)
    {
        for (int c = 0; c < park_candidate_count && park_label_count < MAX_PARK_LABELS; c++)
        {
            label_candidate_opt_t *cand = &park_candidates[c];
            map_label_t *label = &map->labels[cand->index];
            DO_RENDER_LABEL_OPT(cand, label);
            park_label_count++;
        }
    }

    /* Render POI labels */
    if (label_types & LABEL_TYPE_POI)
    {
        for (int c = 0; c < poi_candidate_count && poi_label_count < MAX_POI_LABELS; c++)
        {
            label_candidate_opt_t *cand = &poi_candidates[c];
            map_label_t *label = &map->labels[cand->index];
            DO_RENDER_LABEL_OPT(cand, label);
            poi_label_count++;
        }
    }

    /* Render road labels */
    if (label_types & LABEL_TYPE_ROAD)
    {
        /* Group by road name and render one label per unique road */
        typedef struct
        {
            uint32_t text_offset;
            const char *road_name;
            label_candidate_opt_t best_position;
            uint8_t road_type;
        } road_group_opt_t;

        road_group_opt_t road_groups[ROAD_NAME_LIMIT_OPT];
        int road_group_count = 0;

        for (int c = 0; c < road_candidate_count; c++)
        {
            label_candidate_opt_t *cand = &road_candidates[c];
            map_label_t *label = &map->labels[cand->index];
            const char *road_name = map->label_text + label->text_offset;

            /* Find existing group */
            int group_idx = -1;
            for (int g = 0; g < road_group_count; g++)
            {
                if (strcmp(road_groups[g].road_name, road_name) == 0)
                {
                    group_idx = g;
                    break;
                }
            }

            if (group_idx < 0)
            {
                if (road_group_count < ROAD_NAME_LIMIT_OPT)
                {
                    group_idx = road_group_count++;
                    road_groups[group_idx].text_offset = label->text_offset;
                    road_groups[group_idx].road_name = road_name;
                    road_groups[group_idx].best_position = *cand;
                    road_groups[group_idx].road_type = label->road_type;
                }
            }
            else
            {
                /* Update if more central */
                float cx = renderer->width * 0.5f;
                float cy = renderer->height * 0.5f;
                float d_new = (cand->sx - cx) * (cand->sx - cx) + (cand->sy - cy) * (cand->sy - cy);
                float d_old = (road_groups[group_idx].best_position.sx - cx) *
                              (road_groups[group_idx].best_position.sx - cx) +
                              (road_groups[group_idx].best_position.sy - cy) *
                              (road_groups[group_idx].best_position.sy - cy);
                if (d_new < d_old)
                {
                    road_groups[group_idx].best_position = *cand;
                }
            }
        }

        /* Sort by road type (highest first) */
        for (int i = 0; i < road_group_count - 1; i++)
        {
            for (int j = 0; j < road_group_count - 1 - i; j++)
            {
                if (road_groups[j].road_type < road_groups[j + 1].road_type)
                {
                    road_group_opt_t tmp = road_groups[j];
                    road_groups[j] = road_groups[j + 1];
                    road_groups[j + 1] = tmp;
                }
            }
        }

        /* Render one label per road */
        for (int g = 0; g < road_group_count && road_label_count < MAX_ROAD_LABELS; g++)
        {
            road_group_opt_t *group = &road_groups[g];
            label_candidate_opt_t *cand = &group->best_position;
            map_label_t *label = &map->labels[cand->index];
            DO_RENDER_LABEL_OPT(cand, label);
            road_label_count++;
        }
    }

#undef ROAD_NAME_LIMIT_OPT
#undef SORT_CANDIDATES_OPT
#undef DO_RENDER_LABEL_OPT

    // MAP_PRINTF("Rendered %d labels (places=%d, roads=%d, water=%d, parks=%d, POIs=%d)\n",
    //            rendered_count, place_label_count, road_label_count, water_label_count,
    //            park_label_count, poi_label_count);
}

void render_map_with_options(renderer_t *renderer, const map_t *map,
                             const map_theme_t *theme, const render_options_t *options)
{
    if (!renderer || !map || !map->loaded || !theme)
    {
        return;
    }

    /* Use default options if NULL */
    render_options_t default_opts;
    if (!options)
    {
        render_options_init(&default_opts);
        options = &default_opts;
    }
    MAP_PRINTF("map1\n");
    /* 1. Clear with background color */
    renderer_clear(renderer, theme->background);
    MAP_PRINTF("map2\n");
    /* 2. Draw areas (water, parks, buildings, etc.) if any area option is enabled */
    if (options->flags & RENDER_OPT_ALL_AREAS)
    {
        render_areas_with_options(renderer, map, theme, options);
    }
    MAP_PRINTF("map3\n");
    /* 3. Draw roads if enabled */
    if (options->flags & RENDER_OPT_ROADS)
    {
        render_roads_with_options(renderer, map, theme, options);
    }
    MAP_PRINTF("map4\n");
    /* 4. Render labels if enabled (uses stb_truetype directly) */
    if ((options->flags & RENDER_OPT_LABELS) &&
        (map->header.features & MAP_FEATURE_LABELS))
    {
        render_labels_with_options(renderer, map, theme, options);
    }
    MAP_PRINTF("map5\n");
}
