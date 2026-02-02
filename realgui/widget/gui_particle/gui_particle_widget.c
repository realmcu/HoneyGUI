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
#include <math.h>
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

    /* Initialize effect management */
    for (int i = 0; i < PARTICLE_WIDGET_MAX_EFFECTS; i++)
    {
        widget->effects[i].handle = PARTICLE_INVALID_HANDLE;
        widget->effects[i].emitter = NULL;
        widget->effects[i].behavior = PARTICLE_BEHAVIOR_NONE;
        widget->effects[i].active = 0;
    }
    widget->next_handle = 1;  /* Start from 1, 0 is invalid */

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

/* ============================================================================
 * Configuration-Driven Effect API Implementation
 * ============================================================================ */

/**
 * @brief Find a free effect slot
 */
static int find_free_effect_slot(gui_particle_widget_t *widget)
{
    for (int i = 0; i < PARTICLE_WIDGET_MAX_EFFECTS; i++)
    {
        if (!widget->effects[i].active)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Find effect entry by handle
 */
static particle_effect_entry_t *find_effect_by_handle(gui_particle_widget_t *widget,
                                                      particle_effect_handle_t handle)
{
    if (handle == PARTICLE_INVALID_HANDLE)
    {
        return NULL;
    }

    for (int i = 0; i < PARTICLE_WIDGET_MAX_EFFECTS; i++)
    {
        if (widget->effects[i].active && widget->effects[i].handle == handle)
        {
            return &widget->effects[i];
        }
    }
    return NULL;
}

particle_effect_handle_t gui_particle_widget_add_effect(
    gui_particle_widget_t *widget,
    const particle_effect_config_t *config)
{
    if (widget == NULL || widget->ps == NULL || config == NULL)
    {
        return PARTICLE_INVALID_HANDLE;
    }

    /* Validate configuration */
    if (particle_effect_config_validate(config) != PARTICLE_CONFIG_OK)
    {
        return PARTICLE_INVALID_HANDLE;
    }

    /* Find a free effect slot */
    int slot = find_free_effect_slot(widget);
    if (slot < 0)
    {
        return PARTICLE_INVALID_HANDLE;
    }

    /* Create emitter from configuration */
    particle_emitter_t *emitter = particle_emitter_create_from_config(config, &widget->config);
    if (emitter == NULL)
    {
        return PARTICLE_INVALID_HANDLE;
    }

    /* Set default image if available and no texture specified */
    if (emitter->particle_image == NULL && widget->default_image != NULL)
    {
        emitter->particle_image = widget->default_image;
    }

    /* Add emitter to particle system */
    if (!particle_system_add_emitter(widget->ps, emitter))
    {
        particle_emitter_destroy(emitter, &widget->config);
        return PARTICLE_INVALID_HANDLE;
    }

    /* Assign handle and store effect entry */
    particle_effect_handle_t handle = widget->next_handle++;
    widget->effects[slot].handle = handle;
    widget->effects[slot].emitter = emitter;
    widget->effects[slot].behavior = config->behavior.mode;
    widget->effects[slot].active = 1;

    /* Handle wrap-around (skip 0 which is invalid) */
    if (widget->next_handle == 0)
    {
        widget->next_handle = 1;
    }

    /* Set up touch event handling based on behavior mode (Requirements 14.4) */
    switch (config->behavior.mode)
    {
    case PARTICLE_BEHAVIOR_FOLLOW_TOUCH:
    case PARTICLE_BEHAVIOR_TRAIL:
    case PARTICLE_BEHAVIOR_TOUCH_FEEDBACK:
        /* Enable follow mode for touch-interactive behaviors */
        particle_emitter_enable_follow(emitter, 1);
        break;
    default:
        break;
    }

    /* If burst mode is enabled, trigger initial burst */
    if (config->emission.burst_enabled && config->emission.burst_count > 0)
    {
        particle_emitter_burst(emitter, widget->ps->pool, config->emission.burst_count);
        /* For single burst effects, disable continuous emission */
        if (config->emission.burst_interval == 0 && !config->lifecycle.loop)
        {
            particle_emitter_enable(emitter, 0);
        }
    }

    return handle;
}

void gui_particle_widget_remove_effect(gui_particle_widget_t *widget,
                                       particle_effect_handle_t handle)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    particle_effect_entry_t *entry = find_effect_by_handle(widget, handle);
    if (entry == NULL)
    {
        return;
    }

    /* Remove emitter from particle system */
    if (entry->emitter != NULL)
    {
        particle_system_remove_emitter(widget->ps, entry->emitter);
        particle_emitter_destroy(entry->emitter, &widget->config);
    }

    /* Clear effect entry */
    entry->handle = PARTICLE_INVALID_HANDLE;
    entry->emitter = NULL;
    entry->behavior = PARTICLE_BEHAVIOR_NONE;
    entry->active = 0;
}

void gui_particle_widget_trigger_burst(gui_particle_widget_t *widget,
                                       particle_effect_handle_t handle)
{
    if (widget == NULL || widget->ps == NULL)
    {
        return;
    }

    particle_effect_entry_t *entry = find_effect_by_handle(widget, handle);
    if (entry == NULL || entry->emitter == NULL)
    {
        return;
    }

    /* Trigger burst using emitter's configured burst count */
    particle_emitter_burst(entry->emitter, widget->ps->pool, entry->emitter->burst_count);
}

void gui_particle_widget_update_position(gui_particle_widget_t *widget,
                                         particle_effect_handle_t handle,
                                         float x, float y)
{
    if (widget == NULL)
    {
        return;
    }

    particle_effect_entry_t *entry = find_effect_by_handle(widget, handle);
    if (entry == NULL || entry->emitter == NULL)
    {
        return;
    }

    /* Update follow position and enable emitter */
    particle_emitter_set_follow_position(entry->emitter, x, y);
    particle_emitter_enable(entry->emitter, 1);

    /* For TRAIL mode, update trail direction based on movement */
    if (entry->behavior == PARTICLE_BEHAVIOR_TRAIL)
    {
        if (entry->emitter->has_prev_position)
        {
            /* Calculate movement direction and set emission angle opposite to it */
            float dx = x - entry->emitter->prev_follow_x;
            float dy = y - entry->emitter->prev_follow_y;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq > 1.0f)  /* Only update if moved significantly */
            {
                float angle = atan2f(dy, dx);
                /* Emit opposite to movement direction */
                float opposite_angle = angle + 3.14159265358979323846f;
                float spread = 0.5f;  /* ~30 degrees spread */
                entry->emitter->angle_min = opposite_angle - spread;
                entry->emitter->angle_max = opposite_angle + spread;
            }
        }

        /* Store current position for next update */
        entry->emitter->prev_follow_x = x;
        entry->emitter->prev_follow_y = y;
        entry->emitter->has_prev_position = 1;
    }
}

void gui_particle_widget_stop_effect(gui_particle_widget_t *widget,
                                     particle_effect_handle_t handle)
{
    if (widget == NULL)
    {
        return;
    }

    particle_effect_entry_t *entry = find_effect_by_handle(widget, handle);
    if (entry == NULL || entry->emitter == NULL)
    {
        return;
    }

    /* Disable emitter to stop particle emission */
    particle_emitter_enable(entry->emitter, 0);
}
