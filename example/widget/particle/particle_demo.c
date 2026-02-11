/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_demo.c
 * @brief Demo entry functions for particle effects
 *
 * Contains all effect_xxx_demo_init() functions that are used by the
 * particle launcher. These may include image/font resources.
 */

#include "particle_demo.h"
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_api_os.h"
#include "gui_api_dc.h"
#include "def_type.h"
#include "root_image/ui_resource.h"
#include <string.h>
#include <math.h>

/* Effect create APIs */
#include "effect_snow.h"
#include "effect_rocket.h"
#include "effect_firework.h"
#include "effect_trail.h"
#include "effect_touch.h"
#include "effect_bubble.h"
#include "effect_custom.h"
#include "effect_tunnel.h"
#include "effect_vortex.h"
#include "effect_lightning.h"
#include "effect_galaxy.h"
#include "effect_fireflies.h"
#include "effect_ripple.h"
#include "effect_rain.h"
#include "effect_magic_circle.h"
#include "effect_light_beam.h"

/*============================================================================*
 *                           Snow Demo
 *============================================================================*/

gui_particle_widget_t *effect_snow_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_snow_create(root, "snow_demo",
                              0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Rocket Demo
 *============================================================================*/

/* Rocket image: nozzle offset within the image (fixed, image-relative) */
#define ROCKET_NOZZLE_OFFSET_X   75
#define ROCKET_NOZZLE_OFFSET_Y   215

gui_particle_widget_t *effect_rocket_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    /* Get rocket image dimensions */
    gui_img_t *tmp_img = gui_img_create_from_mem(root, "tmp", ROCKET_BIN, 0, 0, 0, 0);
    int img_w = gui_img_get_width(tmp_img);
    int img_h = gui_img_get_height(tmp_img);
    gui_obj_tree_free(GUI_BASE(tmp_img));

    /* Center rocket image on screen */
    int img_x = (screen_w - img_w) / 2;
    int img_y = (screen_h - img_h) / 2;

    /* Nozzle position in screen coordinates */
    int nozzle_x = img_x + ROCKET_NOZZLE_OFFSET_X;
    int nozzle_y = img_y + ROCKET_NOZZLE_OFFSET_Y;

    /* Widget: position so nozzle default (70% w, 10% h) aligns with rocket nozzle */
    int widget_w = screen_w;
    int widget_h = screen_h - nozzle_y;
    int widget_x = nozzle_x - (int)((float)widget_w * 0.7f);
    int widget_y = nozzle_y - (int)((float)widget_h * 0.1f);
    if (widget_x < 0) { widget_x = 0; }

    gui_particle_widget_t *widget = effect_rocket_create(root, "rocket_demo",
                                                         widget_x, widget_y,
                                                         widget_w, widget_h);
    if (widget == NULL)
    {
        gui_log("Rocket: Failed to create widget\n");
        return NULL;
    }

    /* Add rocket image as child (demo-only resource) */
    gui_img_t *img = gui_img_create_from_mem(GUI_BASE(widget), "img", ROCKET_BIN,
                                             img_x - widget_x,
                                             img_y - widget_y, 0, 0);
    gui_img_set_mode(img, IMG_FILTER_BLACK);

    gui_log("Rocket: Demo initialized\n");
    return widget;
}

/*============================================================================*
 *                           Firework Demo
 *============================================================================*/

gui_particle_widget_t *effect_firework_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_firework_create(root, "firework_demo",
                                  0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Trail Demo
 *============================================================================*/

gui_particle_widget_t *effect_trail_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_trail_create(root, "trail_demo",
                               0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Touch Demo
 *============================================================================*/

gui_particle_widget_t *effect_touch_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_touch_create(root, "touch_demo",
                               0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Bubble Demo
 *============================================================================*/

gui_particle_widget_t *effect_bubble_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_bubble_create(root, "bubble_demo",
                                0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Custom Demo
 *============================================================================*/

gui_particle_widget_t *effect_custom_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_custom_create(root, "custom_demo",
                                0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Tunnel Demo
 *============================================================================*/

gui_particle_widget_t *effect_tunnel_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_tunnel_create(root, "tunnel_demo",
                                0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Vortex Demo
 *============================================================================*/

gui_particle_widget_t *effect_vortex_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_vortex_create(root, "vortex_demo",
                                0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Lightning Demo
 *============================================================================*/

gui_particle_widget_t *effect_lightning_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_lightning_create(root, "lightning_demo",
                                   0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Galaxy Demo (with image + text)
 *============================================================================*/

#define GALAXY_WIDGET_MARGIN 0.10f
#define TEXT_VALUE_MIN       60
#define TEXT_VALUE_MAX       89
#define TEXT_UPDATE_INTERVAL 1000

static gui_text_t *s_galaxy_text = NULL;
static int s_text_value = TEXT_VALUE_MIN;
static char s_text_buffer[16];

static void galaxy_text_timer_cb(void *user_data)
{
    GUI_UNUSED(user_data);
    s_text_value++;
    if (s_text_value > TEXT_VALUE_MAX)
    {
        s_text_value = TEXT_VALUE_MIN;
    }
    snprintf(s_text_buffer, sizeof(s_text_buffer), "%d%%", s_text_value);
    gui_text_content_set(s_galaxy_text, s_text_buffer, strlen(s_text_buffer));
}

gui_particle_widget_t *effect_galaxy_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;

    int widget_y = (int)(screen_h * GALAXY_WIDGET_MARGIN);
    int widget_h = screen_h - widget_y * 2;

    gui_particle_widget_t *widget = effect_galaxy_create(root, "galaxy_demo",
                                                         0, widget_y,
                                                         screen_w, widget_h);
    if (widget == NULL)
    {
        gui_log("Galaxy: Failed to create widget\n");
        return NULL;
    }

    /* Image position in widget local coordinates */
    gui_img_t *img = gui_img_create_from_mem(GUI_BASE(widget), "img", GALAXY_CIRCLE_BIN, 0, 0,
                                             0, 0);
    uint16_t img_w = gui_img_get_width(img);
    uint16_t img_h = gui_img_get_height(img);
    gui_img_set_pos(img, (screen_w - img_w) / 2,
                    (screen_h - img_h) / 2 - widget_y);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);

    s_galaxy_text = gui_text_create(GUI_BASE(widget), "text", 0, 0, 0, 0);
    s_text_value = TEXT_VALUE_MIN;
    snprintf(s_text_buffer, sizeof(s_text_buffer), "%d%%", s_text_value);
    gui_text_set(s_galaxy_text, s_text_buffer, GUI_FONT_SRC_TTF, APP_COLOR_SILVER,
                 strlen(s_text_buffer), 90);
    gui_text_type_set(s_galaxy_text, NOTOSANS_REGULAR_VECTOR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(s_galaxy_text, MID_CENTER);
    gui_obj_create_timer((gui_obj_t *)s_galaxy_text, TEXT_UPDATE_INTERVAL, true,
                         galaxy_text_timer_cb);

    gui_log("Galaxy: Demo initialized\n");
    return widget;
}

/*============================================================================*
 *                           Fireflies Demo
 *============================================================================*/

gui_particle_widget_t *effect_fireflies_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_fireflies_create(root, "fireflies_demo",
                                   0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Ripple Demo
 *============================================================================*/

gui_particle_widget_t *effect_ripple_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_ripple_create(root, "ripple_demo",
                                0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Rain Demo
 *============================================================================*/

gui_particle_widget_t *effect_rain_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_rain_create(root, "rain_demo",
                              0, 0, dc->screen_width, dc->screen_height);
}

/*============================================================================*
 *                           Magic Circle Demo (with images)
 *============================================================================*/

#define OUTER_ROTATE_SPEED    1.0f
#define INNER_ROTATE_SPEED    1.5f

static gui_img_t *s_img_outer = NULL;
static gui_img_t *s_img_inner = NULL;
static float s_outer_angle = 0.0f;
static float s_inner_angle = 0.0f;

static void mc_demo_widget_update(void *user_data)
{
    GUI_UNUSED(user_data);

    s_outer_angle += OUTER_ROTATE_SPEED;
    if (s_outer_angle >= 360.0f)
    {
        s_outer_angle -= 360.0f;
    }

    s_inner_angle -= INNER_ROTATE_SPEED;
    if (s_inner_angle <= -360.0f)
    {
        s_inner_angle += 360.0f;
    }

    if (s_img_outer != NULL)
    {
        gui_img_rotation(s_img_outer, s_outer_angle);
    }
    if (s_img_inner != NULL)
    {
        gui_img_rotation(s_img_inner, s_inner_angle);
    }
}

gui_particle_widget_t *effect_magic_circle_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();
    int screen_w = dc->screen_width;
    int screen_h = dc->screen_height;
    float center_x = (float)(screen_w / 2);
    float center_y = (float)(screen_h / 2);

    gui_particle_widget_t *widget = effect_magic_circle_create(root, "magic_circle_demo",
                                                               0, 0, screen_w, screen_h);
    if (widget == NULL)
    {
        gui_log("MagicCircle: Failed to create widget\n");
        return NULL;
    }

    /* Create outer circle image */
    s_img_outer = gui_img_create_from_mem(GUI_BASE(widget), "mc_outer",
                                          MAGIC_OUTER_BIN, 0, 0, 0, 0);
    if (s_img_outer != NULL)
    {
        uint16_t ow = gui_img_get_width(s_img_outer);
        uint16_t oh = gui_img_get_height(s_img_outer);
        gui_img_set_focus(s_img_outer, (float)ow / 2.0f, (float)oh / 2.0f);
        gui_img_translate(s_img_outer, center_x, center_y);
        gui_img_set_mode(s_img_outer, IMG_SRC_OVER_MODE);
        gui_img_set_quality(s_img_outer, true);
    }

    /* Create inner circle image */
    s_img_inner = gui_img_create_from_mem(GUI_BASE(widget), "mc_inner",
                                          MAGIC_INNER_BIN, 0, 0, 0, 0);
    if (s_img_inner != NULL)
    {
        uint16_t iw = gui_img_get_width(s_img_inner);
        uint16_t ih = gui_img_get_height(s_img_inner);
        gui_img_set_focus(s_img_inner, (float)iw / 2.0f, (float)ih / 2.0f);
        gui_img_translate(s_img_inner, center_x, center_y);
        gui_img_set_mode(s_img_inner, IMG_SRC_OVER_MODE);
        gui_img_set_quality(s_img_inner, true);
    }

    /* Register frame update callback for image rotation */
    gui_particle_widget_set_update_cb(widget, mc_demo_widget_update, NULL);

    s_outer_angle = 0.0f;
    s_inner_angle = 0.0f;

    gui_log("MagicCircle: Demo initialized\n");
    return widget;
}

/*============================================================================*
 *                           Light Beam Demo
 *============================================================================*/

gui_particle_widget_t *effect_light_beam_demo_init(void)
{
    gui_obj_t *root = gui_obj_get_root();
    gui_dispdev_t *dc = gui_get_dc();

    return effect_light_beam_create(root, "light_beam_demo",
                                    0, 0, dc->screen_width, dc->screen_height);
}
