/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file effect_light_beam.c
 * @brief Light Beam Particle Effect - Multi-beam Reflecting Lasers
 *
 * This example demonstrates:
 * 1. Multiple colored laser beams (red, green, blue) bouncing independently
 * 2. Bright saturated colors with hot-white core for laser look
 * 3. Bounce flash effect when beams hit screen edges
 * 4. ADDITIVE blending for intense glow
 */

#include "effect_light_beam.h"
#include "gui_obj.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include <math.h>

#define PARTICLE_POOL_SIZE      800
#define BEAM_COUNT              3
#define BEAM_LENGTH             25
#define BEAM_SPEED              280.0f
#define DIRECTION_CHANGE_MS     2500
#define DIRECTION_CHANGE_JITTER 1200
#define BOUNCE_FLASH_PARTICLES  12
#define BOUNCE_FLASH_SPEED      120.0f

static gui_particle_widget_t *s_beam_widget = NULL;
static int s_screen_w = 0;
static int s_screen_h = 0;

typedef struct
{
    float head_x, head_y;
    float vx, vy;
    uint32_t next_change_time;
    uint32_t color_core;
    uint32_t color_mid;
    uint32_t color_tail;
    float hist_x[64];
    float hist_y[64];
    int hist_head;
    int hist_count;
} beam_state_t;

static beam_state_t s_beams[BEAM_COUNT];

static uint32_t beam_rand(void)
{
    static uint32_t seed = 98765;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

static float beam_rand_float(float min, float max)
{
    return min + (float)(beam_rand() % 1000) / 1000.0f * (max - min);
}

static void beam_history_push(beam_state_t *b, float x, float y)
{
    b->hist_x[b->hist_head] = x;
    b->hist_y[b->hist_head] = y;
    b->hist_head = (b->hist_head + 1) % 64;
    if (b->hist_count < 64)
    {
        b->hist_count++;
    }
}

static int beam_history_get(beam_state_t *b, int age, float *x, float *y)
{
    if (age >= b->hist_count)
    {
        return 0;
    }
    int idx = (b->hist_head - 1 - age + 64) % 64;
    *x = b->hist_x[idx];
    *y = b->hist_y[idx];
    return 1;
}

static int reflect_beam(beam_state_t *b)
{
    int bounced = 0;

    if (b->head_x <= 0.0f)
    {
        b->head_x = 0.0f;
        b->vx = -b->vx;
        bounced = 1;
    }
    else if (b->head_x >= (float)s_screen_w)
    {
        b->head_x = (float)s_screen_w;
        b->vx = -b->vx;
        bounced = 1;
    }

    if (b->head_y <= 0.0f)
    {
        b->head_y = 0.0f;
        b->vy = -b->vy;
        bounced = 1;
    }
    else if (b->head_y >= (float)s_screen_h)
    {
        b->head_y = (float)s_screen_h;
        b->vy = -b->vy;
        bounced = 1;
    }

    return bounced;
}

static void maybe_change_direction(beam_state_t *b, uint32_t now)
{
    if (now < b->next_change_time)
    {
        return;
    }

    float angle = atan2f(b->vy, b->vx);
    float delta = beam_rand_float(-M_PI_F * 0.5f, M_PI_F * 0.5f);
    angle += delta;

    b->vx = cosf(angle) * BEAM_SPEED;
    b->vy = sinf(angle) * BEAM_SPEED;

    b->next_change_time = now + DIRECTION_CHANGE_MS +
                          beam_rand() % DIRECTION_CHANGE_JITTER;
}

static void emit_bounce_flash(beam_state_t *b)
{
    if (s_beam_widget == NULL || s_beam_widget->ps == NULL ||
        s_beam_widget->ps->pool == NULL)
    {
        return;
    }

    particle_pool_t *pool = s_beam_widget->ps->pool;

    for (int i = 0; i < BOUNCE_FLASH_PARTICLES; i++)
    {
        particle_t *p = particle_pool_alloc(pool);
        if (p == NULL)
        {
            break;
        }

        float angle = beam_rand_float(0.0f, 2.0f * M_PI_F);
        float speed = beam_rand_float(BOUNCE_FLASH_SPEED * 0.3f, BOUNCE_FLASH_SPEED);

        p->x = b->head_x;
        p->y = b->head_y;
        p->user_data = NULL;
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed;
        p->ax = 0.0f;
        p->ay = 0.0f;
        p->color = 0xFFFFFFFF;
        float flash_scale = beam_rand_float(1.5f, 3.5f);
        p->scale_start = flash_scale;
        p->scale_end = 0.0f;
        p->opacity_start = 255;
        p->opacity_end = 0;
        p->life = (uint16_t)(beam_rand() % 200 + 200);
        p->max_life = p->life;
        p->active = 1;
        p->rotation = 0.0f;
        p->rotation_speed = 0.0f;
    }
}

static void emit_beam_trail(beam_state_t *b)
{
    if (s_beam_widget == NULL || s_beam_widget->ps == NULL ||
        s_beam_widget->ps->pool == NULL)
    {
        return;
    }

    particle_pool_t *pool = s_beam_widget->ps->pool;

    /* Recycle previous trail particles for this beam */
    for (uint16_t idx = 0; idx < pool->capacity; idx++)
    {
        particle_t *old = &pool->particles[idx];
        if (old->active && old->user_data == (void *)b)
        {
            particle_pool_free(pool, old);
        }
    }

    for (int i = 0; i < BEAM_LENGTH; i++)
    {
        float px, py;
        int age = i * 2;
        if (!beam_history_get(b, age, &px, &py))
        {
            break;
        }

        particle_t *p = particle_pool_alloc(pool);
        if (p == NULL)
        {
            break;
        }

        p->user_data = (void *)b;
        p->x = px;
        p->y = py;
        p->vx = 0.0f;
        p->vy = 0.0f;
        p->ax = 0.0f;
        p->ay = 0.0f;

        float t = (float)i / (float)(BEAM_LENGTH - 1);

        uint32_t c;
        if (t < 0.5f)
        {
            float u = t * 2.0f;
            uint8_t r0 = (b->color_core >> 16) & 0xFF;
            uint8_t g0 = (b->color_core >> 8) & 0xFF;
            uint8_t b0 = b->color_core & 0xFF;
            uint8_t r1 = (b->color_mid >> 16) & 0xFF;
            uint8_t g1 = (b->color_mid >> 8) & 0xFF;
            uint8_t b1 = b->color_mid & 0xFF;
            uint8_t r = (uint8_t)(r0 + (r1 - r0) * u);
            uint8_t g = (uint8_t)(g0 + (g1 - g0) * u);
            uint8_t bl = (uint8_t)(b0 + (b1 - b0) * u);
            c = 0xFF000000 | (r << 16) | (g << 8) | bl;
        }
        else
        {
            float u = (t - 0.5f) * 2.0f;
            uint8_t r0 = (b->color_mid >> 16) & 0xFF;
            uint8_t g0 = (b->color_mid >> 8) & 0xFF;
            uint8_t b0 = b->color_mid & 0xFF;
            uint8_t r1 = (b->color_tail >> 16) & 0xFF;
            uint8_t g1 = (b->color_tail >> 8) & 0xFF;
            uint8_t b1 = b->color_tail & 0xFF;
            uint8_t r = (uint8_t)(r0 + (r1 - r0) * u);
            uint8_t g = (uint8_t)(g0 + (g1 - g0) * u);
            uint8_t bl = (uint8_t)(b0 + (b1 - b0) * u);
            c = 0xFF000000 | (r << 16) | (g << 8) | bl;
        }

        p->color = c;

        float sc = 1.5f * (1.0f - t * 0.2f);
        uint8_t op = (uint8_t)(255.0f * (1.0f - t * 0.2f));
        p->scale_start = sc;
        p->scale_end = sc;
        p->opacity_start = op;
        p->opacity_end = op;

        p->life = 500;
        p->max_life = 500;
        p->active = 1;
        p->rotation = 0.0f;
        p->rotation_speed = 0.0f;
    }
}

static void beam_update_cb(void *user_data)
{
    GUI_UNUSED(user_data);

    if (s_beam_widget == NULL)
    {
        return;
    }

    uint32_t now = gui_ms_get();
    static uint32_t s_last_time = 0;
    if (s_last_time == 0)
    {
        s_last_time = now;
    }

    float dt = (float)(now - s_last_time) / 1000.0f;
    s_last_time = now;

    if (dt > 0.05f)
    {
        dt = 0.05f;
    }

    for (int i = 0; i < BEAM_COUNT; i++)
    {
        beam_state_t *b = &s_beams[i];

        maybe_change_direction(b, now);

        b->head_x += b->vx * dt;
        b->head_y += b->vy * dt;

        int bounced = reflect_beam(b);
        if (bounced)
        {
            emit_bounce_flash(b);
        }

        beam_history_push(b, b->head_x, b->head_y);
        emit_beam_trail(b);
    }
}

static void init_beam(beam_state_t *b, uint32_t core, uint32_t mid, uint32_t tail,
                      float start_x, float start_y)
{
    b->head_x = start_x;
    b->head_y = start_y;
    b->color_core = core;
    b->color_mid = mid;
    b->color_tail = tail;
    b->hist_head = 0;
    b->hist_count = 0;

    float angle = beam_rand_float(0.0f, 2.0f * M_PI_F);
    b->vx = cosf(angle) * BEAM_SPEED;
    b->vy = sinf(angle) * BEAM_SPEED;

    b->next_change_time = gui_ms_get() + DIRECTION_CHANGE_MS +
                          beam_rand() % DIRECTION_CHANGE_JITTER;

    for (int i = 0; i < 64; i++)
    {
        beam_history_push(b, start_x, start_y);
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_particle_widget_t *effect_light_beam_create(gui_obj_t *parent, const char *name,
                                                int16_t x, int16_t y, int16_t w, int16_t h)
{
    s_screen_w = w;
    s_screen_h = h;

    float cx = (float)(w / 2);
    float cy = (float)(h / 2);

    /* Red laser */
    init_beam(&s_beams[0],
              0xFFFF0000, 0xFFDD0000, 0xFFBB0000,
              cx - 30.0f, cy);

    /* Green laser */
    init_beam(&s_beams[1],
              0xFF00FF00, 0xFF00DD00, 0xFF00BB00,
              cx, cy - 30.0f);

    /* Blue laser */
    init_beam(&s_beams[2],
              0xFF0000FF, 0xFF0020FF, 0xFF0010DD,
              cx + 30.0f, cy);

    s_beam_widget = gui_particle_widget_create(parent, name,
                                               x, y, w, h,
                                               PARTICLE_POOL_SIZE);
    if (s_beam_widget == NULL)
    {
        return NULL;
    }

    gui_particle_widget_set_update_cb(s_beam_widget, beam_update_cb, NULL);

    return s_beam_widget;
}
