/**
 * @file gui_particle_widget.c
 * @brief Particle System Widget Adapter Implementation for HoneyGUI
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_particle_widget.h"
#include "gui_obj.h"
#include "gui_api.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "draw_img.h"
#include "lite_geometry.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/** Render context passed to particle render callback */
typedef struct particle_render_ctx
{
    gui_particle_widget_t *widget;
    struct gui_dispdev *dc;
} particle_render_ctx_t;

/** Block size for tiled rendering */
#define PARTICLE_BLOCK_SIZE 100

/** Set image data header */
static void particle_set_img_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = w;
    head->h = h;
    head->version = 0;
    head->rsvd2 = 0;
}

/*============================================================================*
 *                           Constants
 *============================================================================*/

/** Widget type identifier */
#define PARTICLE_WIDGET_TYPE GUI_VENDOR_TYPE

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Platform malloc wrapper for particle system
 */
static void *particle_platform_malloc(size_t size)
{
    return gui_malloc(size);
}

/**
 * @brief Platform free wrapper for particle system
 */
static void particle_platform_free(void *ptr)
{
    gui_free(ptr);
}

/**
 * @brief Platform get_time wrapper for particle system
 */
static uint32_t particle_platform_get_time(void)
{
    return gui_ms_get();
}

/**
 * @brief Render callback for particle system
 *
 * This callback is called for each active particle during rendering.
 * It draws the particle using simple rectangle rendering since we don't
 * have direct access to gui_img rendering in the callback context.
 */
static void particle_render_callback(particle_t *particle, void *render_ctx)
{
    particle_render_ctx_t *ctx = (particle_render_ctx_t *)render_ctx;

    if (ctx == NULL || particle == NULL)
    {
        return;
    }

    /*
     * Note: In a full implementation, this would use gui_img or direct
     * framebuffer access to render the particle. For now, we store
     * particle data for batch rendering in the draw callback.
     *
     * The actual rendering is handled in gui_particle_draw_cb where
     * we have proper access to the display context.
     */
    GUI_UNUSED(ctx);
}

/**
 * @brief Prepare callback for particle widget
 *
 * Called before drawing to update particle system state.
 */
static void gui_particle_prepare(gui_obj_t *obj)
{
    gui_particle_widget_t *widget = (gui_particle_widget_t *)obj;

    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    /* Call user update callback if set */
    if (widget->update_cb != NULL)
    {
        widget->update_cb(widget->update_cb_data);
    }

    /* Calculate delta time */
    uint32_t current_time = gui_ms_get();
    uint32_t last_time = widget->ps->last_update_time;
    float dt = 0.0f;

    if (last_time > 0 && current_time > last_time)
    {
        dt = (float)(current_time - last_time) / 1000.0f;
        /* Clamp dt to reasonable range (max 100ms) */
        if (dt > 0.1f)
        {
            dt = 0.1f;
        }
    }

    /* Update last_update_time for next frame */
    widget->ps->last_update_time = current_time;

    /* Update particle system */
    particle_system_update(widget->ps, dt);

    /* Always request redraw to keep animation running */
    gui_fb_change();
}

/**
 * @brief Draw callback for particle widget
 *
 * Renders all active particles to the display using tiled rendering.
 */
static void gui_particle_draw_cb(gui_obj_t *obj)
{
    gui_particle_widget_t *widget = (gui_particle_widget_t *)obj;

    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    gui_dispdev_t *dc = gui_get_dc();
    if (dc == NULL)
    {
        return;
    }

    /* Get particle pool for direct access */
    particle_pool_t *pool = particle_system_get_pool(widget->ps);
    if (pool == NULL)
    {
        return;
    }

    /* Check if there are active particles */
    uint16_t active_count = particle_system_get_active_count(widget->ps);
    if (active_count == 0)
    {
        return;
    }

    /* Allocate block buffer for tiled rendering */
    int bytes_per_pixel = 4;
    uint32_t block_buffer_size = (uint32_t)(PARTICLE_BLOCK_SIZE * PARTICLE_BLOCK_SIZE *
                                            bytes_per_pixel + sizeof(gui_rgb_data_head_t));
    uint8_t *block_buffer = (uint8_t *)gui_malloc(block_buffer_size);
    if (block_buffer == NULL)
    {
        return;
    }

    /* Allocate draw_img structure */
    draw_img_t *draw_img = gui_malloc(sizeof(draw_img_t));
    if (draw_img == NULL)
    {
        gui_free(block_buffer);
        return;
    }
    memset(draw_img, 0x00, sizeof(draw_img_t));
    draw_img->data = block_buffer;
    draw_img->blend_mode = IMG_SRC_OVER_MODE;
    draw_img->opacity_value = UINT8_MAX;
    draw_img->high_quality = 0;

    /* Initialize matrix */
    memcpy(&draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&draw_img->inverse, &draw_img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&draw_img->inverse);

    /* Calculate block count */
    uint16_t block_cols = (uint16_t)((obj->w + PARTICLE_BLOCK_SIZE - 1) / PARTICLE_BLOCK_SIZE);
    uint16_t block_rows = (uint16_t)((obj->h + PARTICLE_BLOCK_SIZE - 1) / PARTICLE_BLOCK_SIZE);

    /* Get particle pool capacity */
    uint16_t capacity = particle_pool_get_capacity(pool);

    /* Render tiles */
    for (uint16_t by = 0; by < block_rows; by++)
    {
        for (uint16_t bx = 0; bx < block_cols; bx++)
        {
            int tile_x = bx * PARTICLE_BLOCK_SIZE;
            int tile_y = by * PARTICLE_BLOCK_SIZE;
            int tile_w = (obj->w - tile_x < PARTICLE_BLOCK_SIZE) ? (obj->w - tile_x) : PARTICLE_BLOCK_SIZE;
            int tile_h = (obj->h - tile_y < PARTICLE_BLOCK_SIZE) ? (obj->h - tile_y) : PARTICLE_BLOCK_SIZE;
            if (tile_w <= 0 || tile_h <= 0)
            {
                continue;
            }

            /* Check if any particle intersects this tile */
            bool has_particles = false;
            for (uint16_t i = 0; i < capacity && !has_particles; i++)
            {
                particle_t *p = &pool->particles[i];
                if (!p->active)
                {
                    continue;
                }

                int16_t radius = (int16_t)(8.0f * p->scale);
                if (radius < 1) { radius = 1; }

                /* Check if particle bounding box intersects tile */
                int px = (int)p->x;
                int py = (int)p->y;
                if (px + radius >= tile_x && px - radius < tile_x + tile_w &&
                    py + radius >= tile_y && py - radius < tile_y + tile_h)
                {
                    has_particles = true;
                }
            }

            if (!has_particles)
            {
                continue;
            }

            /* Setup image header */
            gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)block_buffer;
            particle_set_img_header(img_header, (uint16_t)tile_w, (uint16_t)tile_h);
            uint8_t *pixel_data = block_buffer + sizeof(gui_rgb_data_head_t);
            memset(pixel_data, 0x00, (size_t)(tile_w * tile_h * 4));

            /* Initialize draw context */
            DrawContext draw_ctx;
            init_draw_context(&draw_ctx, pixel_data, tile_w, tile_h, PIXEL_FORMAT_ARGB8888);
            draw_ctx.clip_rect.x = 0;
            draw_ctx.clip_rect.y = 0;
            draw_ctx.clip_rect.w = (int16_t)tile_w;
            draw_ctx.clip_rect.h = (int16_t)tile_h;
            draw_ctx.enable_aa = 1;

            /* Draw particles in this tile */
            for (uint16_t i = 0; i < capacity; i++)
            {
                particle_t *p = &pool->particles[i];
                if (!p->active)
                {
                    continue;
                }

                /* Calculate particle position relative to tile */
                float local_x = p->x - (float)tile_x;
                float local_y = p->y - (float)tile_y;

                /* Calculate particle radius based on scale */
                float radius = 4.0f * p->scale;
                if (radius < 1.0f) { radius = 1.0f; }

                /* Check if particle is in this tile */
                if (local_x + radius < 0 || local_x - radius >= tile_w ||
                    local_y + radius < 0 || local_y - radius >= tile_h)
                {
                    continue;
                }

                /* Create color with opacity (ARGB8888) */
                uint8_t alpha = p->opacity;
                /* Extract RGB from particle color and combine with current opacity */
                uint32_t rgb = p->color & 0x00FFFFFF;
                uint32_t color = ((uint32_t)alpha << 24) | rgb;

                /* Draw particle as filled circle */
                fill_circle(&draw_ctx, local_x, local_y, radius, color);
            }

            /* Setup matrix for this tile */
            memcpy(&draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
            matrix_translate((float)tile_x, (float)tile_y, &draw_img->matrix);
            memcpy(&draw_img->inverse, &draw_img->matrix, sizeof(struct gui_matrix));
            matrix_inverse(&draw_img->inverse);

            /* Load and blit to display */
            draw_img_load_scale(draw_img, IMG_SRC_MEMADDR);
            draw_img_new_area(draw_img, NULL);
            gui_acc_blit_to_dc(draw_img, dc, NULL);
        }
    }

    /* Cleanup */
    if (draw_img_acc_end_cb != NULL)
    {
        draw_img_acc_end_cb(draw_img);
    }
    gui_free(block_buffer);
    gui_free(draw_img);

    /* Call render callback if set */
    particle_system_render(widget->ps);
}

/**
 * @brief End callback for particle widget
 *
 * Called after drawing to clean up temporary resources.
 */
static void gui_particle_end(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    /* No cleanup needed for particle widget */
}

/**
 * @brief Destroy callback for particle widget
 *
 * Called when widget is being destroyed to free resources.
 */
static void gui_particle_destroy(gui_obj_t *obj)
{
    gui_particle_widget_t *widget = (gui_particle_widget_t *)obj;

    if (widget == NULL)
    {
        return;
    }

    /* Destroy particle system */
    if (widget->ps != NULL)
    {
        particle_system_destroy(widget->ps);
        widget->ps = NULL;
    }

    /* Clear emitter references */
    widget->trail_emitter = NULL;
    widget->touch_emitter = NULL;
}

/**
 * @brief Main callback dispatcher for particle widget
 */
static void gui_particle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj == NULL)
    {
        return;
    }

    switch (cb_type)
    {
    case OBJ_PREPARE:
        gui_particle_prepare(obj);
        break;

    case OBJ_DRAW:
        gui_particle_draw_cb(obj);
        break;

    case OBJ_END:
        gui_particle_end(obj);
        break;

    case OBJ_DESTROY:
        gui_particle_destroy(obj);
        break;

    default:
        break;
    }
}

/**
 * @brief Constructor for particle widget
 */
static void gui_particle_widget_ctor(gui_particle_widget_t *widget,
                                     gui_obj_t *parent,
                                     const char *name,
                                     int16_t x, int16_t y,
                                     int16_t w, int16_t h,
                                     uint16_t pool_capacity)
{
    gui_obj_t *obj = (gui_obj_t *)widget;

    /* Initialize base object */
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    /* Set callbacks */
    obj->obj_cb = gui_particle_cb;
    obj->has_input_prepare_cb = false;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
    obj->type = PARTICLE_WIDGET_TYPE;

    /* Initialize platform configuration */
    widget->config.malloc_fn = particle_platform_malloc;
    widget->config.free_fn = particle_platform_free;
    widget->config.get_time_fn = particle_platform_get_time;

    /* Create particle system */
    widget->ps = particle_system_create(pool_capacity, &widget->config);

    if (widget->ps != NULL)
    {
        /* Set bounds to widget area */
        particle_system_set_bounds(widget->ps, 0, 0, (float)w, (float)h);
        particle_system_enable_bounds(widget->ps, 1);

        /* Enable auto-throttle */
        particle_system_set_auto_throttle(widget->ps, 1, 0.8f);

        /* Set render callback */
        particle_render_ctx_t *ctx = gui_malloc(sizeof(particle_render_ctx_t));
        if (ctx != NULL)
        {
            ctx->widget = widget;
            ctx->dc = NULL;
            particle_system_set_render_callback(widget->ps, particle_render_callback, ctx);
        }
    }

    /* Initialize effect state */
    widget->trail_emitter = NULL;
    widget->touch_emitter = NULL;
    widget->prev_x = 0.0f;
    widget->prev_y = 0.0f;
    widget->trail_active = 0;
    widget->touch_active = 0;
    widget->default_image = NULL;
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *gui_particle_widget_create(gui_obj_t *parent, const char *name,
                                                  int16_t x, int16_t y, int16_t w, int16_t h,
                                                  uint16_t pool_capacity)
{
    GUI_ASSERT(parent != NULL);

    /* Allocate widget */
    gui_particle_widget_t *widget = gui_malloc(sizeof(gui_particle_widget_t));
    if (widget == NULL)
    {
        return NULL;
    }

    memset(widget, 0, sizeof(gui_particle_widget_t));

    /* Construct widget */
    gui_particle_widget_ctor(widget, parent, name, x, y, w, h, pool_capacity);

    /* Initialize list nodes */
    gui_list_init(&(GET_BASE(widget)->child_list));
    if (GET_BASE(widget)->parent != NULL)
    {
        gui_list_insert_before(&(GET_BASE(widget)->parent->child_list),
                               &(GET_BASE(widget)->brother_list));
    }
    GET_BASE(widget)->create_done = true;

    return widget;
}

particle_system_t *gui_particle_widget_get_system(gui_particle_widget_t *widget)
{
    if (widget == NULL)
    {
        return NULL;
    }
    return widget->ps;
}

void gui_particle_widget_set_default_image(gui_particle_widget_t *widget, void *image)
{
    if (widget == NULL)
    {
        return;
    }
    widget->default_image = image;
}

void gui_particle_widget_add_firework(gui_particle_widget_t *widget,
                                      float x, float y, uint8_t count)
{
    gui_particle_widget_add_firework_color(widget, x, y, count, 0xFFFFFFFF);
}

void gui_particle_widget_add_firework_color(gui_particle_widget_t *widget,
                                            float x, float y, uint8_t count, uint32_t color)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    /* Create firework emitter */
    particle_emitter_t *emitter = effect_firework_create(&widget->config, x, y, count);
    if (emitter == NULL)
    {
        return;
    }

    /* Set color */
    particle_emitter_set_color(emitter, color);

    /* Set default image if available */
    if (widget->default_image != NULL)
    {
        particle_emitter_set_image(emitter, widget->default_image);
    }

    /* Add to system and trigger burst */
    if (particle_system_add_emitter(widget->ps, emitter))
    {
        particle_emitter_burst(emitter, widget->ps->pool, count);
        /* Disable continuous emission after burst */
        particle_emitter_enable(emitter, 0);
    }
    else
    {
        /* Failed to add, destroy emitter */
        particle_emitter_destroy(emitter, &widget->config);
    }
}

void gui_particle_widget_add_trail(gui_particle_widget_t *widget)
{
    gui_particle_widget_add_trail_color(widget, 0xFFFFFFFF);
}

void gui_particle_widget_add_trail_color(gui_particle_widget_t *widget, uint32_t color)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    /* Only add one trail emitter */
    if (widget->trail_emitter != NULL)
    {
        return;
    }

    /* Create trail emitter */
    widget->trail_emitter = effect_trail_create(&widget->config);
    if (widget->trail_emitter == NULL)
    {
        return;
    }

    /* Set color */
    particle_emitter_set_color(widget->trail_emitter, color);

    /* Set default image if available */
    if (widget->default_image != NULL)
    {
        particle_emitter_set_image(widget->trail_emitter, widget->default_image);
    }

    /* Add to system */
    if (!particle_system_add_emitter(widget->ps, widget->trail_emitter))
    {
        particle_emitter_destroy(widget->trail_emitter, &widget->config);
        widget->trail_emitter = NULL;
    }
}

void gui_particle_widget_update_trail(gui_particle_widget_t *widget, float x, float y)
{
    if (widget == NULL || widget->trail_emitter == NULL)
    {
        return;
    }

    /* Always update follow position first */
    particle_emitter_set_follow_position(widget->trail_emitter, x, y);
    particle_emitter_enable(widget->trail_emitter, 1);

    /* Update trail with current and previous positions */
    if (widget->trail_active)
    {
        effect_trail_update(widget->trail_emitter, x, y, widget->prev_x, widget->prev_y);
    }

    /* Store current position for next update */
    widget->prev_x = x;
    widget->prev_y = y;
    widget->trail_active = 1;
}

void gui_particle_widget_stop_trail(gui_particle_widget_t *widget)
{
    if (widget == NULL || widget->trail_emitter == NULL)
    {
        return;
    }

    /* Disable emitter and reset tracking state */
    particle_emitter_enable(widget->trail_emitter, 0);
    widget->trail_active = 0;
}

void gui_particle_widget_add_touch(gui_particle_widget_t *widget)
{
    gui_particle_widget_add_touch_color(widget, 0xFFFFFFFF);
}

void gui_particle_widget_add_touch_color(gui_particle_widget_t *widget, uint32_t color)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    /* Only add one touch emitter */
    if (widget->touch_emitter != NULL)
    {
        return;
    }

    /* Create touch emitter */
    widget->touch_emitter = effect_touch_create(&widget->config);
    if (widget->touch_emitter == NULL)
    {
        return;
    }

    /* Set color */
    particle_emitter_set_color(widget->touch_emitter, color);

    /* Set default image if available */
    if (widget->default_image != NULL)
    {
        particle_emitter_set_image(widget->touch_emitter, widget->default_image);
    }

    /* Add to system */
    if (!particle_system_add_emitter(widget->ps, widget->touch_emitter))
    {
        particle_emitter_destroy(widget->touch_emitter, &widget->config);
        widget->touch_emitter = NULL;
    }
}

void gui_particle_widget_touch_tap(gui_particle_widget_t *widget, float x, float y)
{
    if (widget == NULL || widget->touch_emitter == NULL || widget->ps == NULL)
    {
        return;
    }

    effect_touch_tap(widget->touch_emitter, widget->ps->pool, x, y);
    widget->touch_active = 1;
}

void gui_particle_widget_touch_drag(gui_particle_widget_t *widget, float x, float y)
{
    if (widget == NULL || widget->touch_emitter == NULL)
    {
        return;
    }

    effect_touch_drag(widget->touch_emitter, x, y);
}

void gui_particle_widget_touch_release(gui_particle_widget_t *widget, float x, float y)
{
    if (widget == NULL || widget->touch_emitter == NULL || widget->ps == NULL)
    {
        return;
    }

    effect_touch_release(widget->touch_emitter, widget->ps->pool, x, y);
    widget->touch_active = 0;
}

void gui_particle_widget_add_snow(gui_particle_widget_t *widget, float intensity)
{
    gui_particle_widget_add_snow_color(widget, intensity, 0xFFFFFFFF);
}

void gui_particle_widget_add_snow_color(gui_particle_widget_t *widget, float intensity,
                                        uint32_t color)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    gui_obj_t *obj = (gui_obj_t *)widget;

    /* Create snow emitter */
    particle_emitter_t *emitter = effect_snow_create(&widget->config, (float)obj->w);
    if (emitter == NULL)
    {
        return;
    }

    /* Set intensity */
    effect_snow_set_intensity(emitter, intensity);

    /* Set color */
    particle_emitter_set_color(emitter, color);

    /* Set default image if available */
    if (widget->default_image != NULL)
    {
        particle_emitter_set_image(emitter, widget->default_image);
    }

    /* Add to system */
    if (!particle_system_add_emitter(widget->ps, emitter))
    {
        particle_emitter_destroy(emitter, &widget->config);
    }
}

void gui_particle_widget_set_snow_intensity(gui_particle_widget_t *widget, float intensity)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    /* Find snow emitter and update intensity */
    /* Note: In a full implementation, we would track the snow emitter reference */
    for (uint8_t i = 0; i < widget->ps->emitter_count; i++)
    {
        particle_emitter_t *emitter = widget->ps->emitters[i];
        if (emitter != NULL)
        {
            /* Check if this looks like a snow emitter (emits from top) */
            if (emitter->shape.type == EMITTER_SHAPE_LINE &&
                emitter->shape.line.y1 < 10.0f && emitter->shape.line.y2 < 10.0f)
            {
                effect_snow_set_intensity(emitter, intensity);
                break;
            }
        }
    }
}

void gui_particle_widget_add_bubble(gui_particle_widget_t *widget)
{
    gui_particle_widget_add_bubble_color(widget, 0xFFFFFFFF);
}

void gui_particle_widget_add_bubble_color(gui_particle_widget_t *widget, uint32_t color)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    gui_obj_t *obj = (gui_obj_t *)widget;

    /* Create bubble emitter */
    particle_emitter_t *emitter = effect_bubble_create(&widget->config,
                                                       (float)obj->w, (float)obj->h);
    if (emitter == NULL)
    {
        return;
    }

    /* Set color */
    particle_emitter_set_color(emitter, color);

    /* Set default image if available */
    if (widget->default_image != NULL)
    {
        particle_emitter_set_image(emitter, widget->default_image);
    }

    /* Configure boundary reflection for bubbles */
    particle_system_set_bound_behavior(widget->ps, BOUNDARY_BEHAVIOR_REFLECT);

    /* Add to system */
    if (!particle_system_add_emitter(widget->ps, emitter))
    {
        particle_emitter_destroy(emitter, &widget->config);
    }
}

void gui_particle_widget_clear(gui_particle_widget_t *widget)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    particle_system_clear(widget->ps);
}

uint16_t gui_particle_widget_get_active_count(gui_particle_widget_t *widget)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return 0;
    }

    return particle_system_get_active_count(widget->ps);
}

void gui_particle_widget_set_update_cb(gui_particle_widget_t *widget,
                                       gui_particle_update_cb_t cb,
                                       void *user_data)
{
    if (widget == NULL)
    {
        return;
    }

    widget->update_cb = cb;
    widget->update_cb_data = user_data;
}
