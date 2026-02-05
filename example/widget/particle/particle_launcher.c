/**
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file particle_launcher.c
 * @brief Particle Effect Launcher - Selection menu using list widget
 */

#include "particle_launcher.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_list.h"
#include "gui_rect.h"
#include "gui_text.h"
#include "gui_api_dc.h"
#include "gui_api_os.h"

/* Effect headers */
#include "effect_firework.h"
#include "effect_trail.h"
#include "effect_touch.h"
#include "effect_snow.h"
#include "effect_bubble.h"
#include "effect_custom.h"
#include "effect_tunnel.h"
#include "effect_rocket.h"
#include "effect_vortex.h"
#include "effect_lightning.h"
#include "effect_galaxy.h"
#include "effect_fireflies.h"
#include "effect_ripple.h"
#include "effect_rain.h"

/*============================================================================*
 *                           Effect Registry
 *============================================================================*/

typedef gui_particle_widget_t *(*effect_init_func_t)(void);

typedef struct
{
    const char *name;
    const char *desc;
    effect_init_func_t init;
} effect_entry_t;

static const effect_entry_t s_effects[] =
{
    {"Firework",   "Burst particles with gravity",    effect_firework_demo_init},
    {"Trail",      "Drag to emit trailing particles", effect_trail_demo_init},
    {"Touch",      "Touch feedback burst effect",     effect_touch_demo_init},
    {"Snow",       "Snowflakes falling from top",     effect_snow_demo_init},
    {"Bubble",     "Rising bubbles with bounce",      effect_bubble_demo_init},
    {"Custom",     "Custom callback behaviors",       effect_custom_demo_init},
    {"Tunnel",     "Warp speed radial effect",        effect_tunnel_demo_init},
    {"Rocket",     "Thruster exhaust with flame",     effect_rocket_demo_init},
    {"Vortex",     "Energy spiral to center",         effect_vortex_demo_init},
    {"Lightning",  "Electric arc discharge",          effect_lightning_demo_init},
    {"Galaxy",     "Spiral galaxy rotation",          effect_galaxy_demo_init},
    {"Fireflies",  "Floating glowing dots",           effect_fireflies_demo_init},
    {"Ripple",     "Touch-triggered rings",           effect_ripple_demo_init},
    {"Rain",       "Raindrops with splash",           effect_rain_demo_init},
};

#define EFFECT_COUNT (sizeof(s_effects) / sizeof(s_effects[0]))

/*============================================================================*
 *                           Static Variables
 *============================================================================*/

static gui_win_t *s_launcher_win = NULL;
static gui_particle_widget_t *s_current_effect = NULL;
static gui_win_t *s_effect_win = NULL;
static int s_screen_w = 0;
static int s_screen_h = 0;

/*============================================================================*
 *                           Forward Declarations
 *============================================================================*/

static void launcher_show(void);
static void effect_start(int index);
static void effect_stop(void);

/*============================================================================*
 *                           Callbacks
 *============================================================================*/

static void on_effect_click(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    int index = (int)(intptr_t)e->user_data;
    effect_start(index);
}

static void on_back_click(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    effect_stop();
}

/*============================================================================*
 *                           List Note Design
 *============================================================================*/

static void note_design(gui_obj_t *obj, void *param)
{
    GUI_UNUSED(param);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int index = note->index;

    if (index < 0 || index >= (int)EFFECT_COUNT)
    {
        return;
    }

    /* Background rect with alternating colors */
    gui_color_t bg_color = (index % 2 == 0) ?
                           gui_rgba(40, 40, 50, 220) : gui_rgba(50, 50, 60, 220);
    gui_rounded_rect_t *bg = gui_rect_create(obj, "bg", 10, 2, s_screen_w - 20, 76, 8, bg_color);
    gui_rect_on_click(bg, on_effect_click, (void *)(intptr_t)index);

    /* Effect name */
    gui_text_t *name_text = gui_text_create(obj, "name", 20, 10, s_screen_w - 40, 28);
    gui_text_set(name_text, (void *)s_effects[index].name, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, strlen(s_effects[index].name), 24);

    /* Effect description */
    gui_text_t *desc_text = gui_text_create(obj, "desc", 20, 42, s_screen_w - 40, 28);
    gui_text_set(desc_text, (void *)s_effects[index].desc, GUI_FONT_SRC_BMP,
                 gui_rgba(180, 180, 180, 255), strlen(s_effects[index].desc), 24);
}

/*============================================================================*
 *                           Launcher Functions
 *============================================================================*/

static void launcher_show(void)
{
    if (s_launcher_win != NULL)
    {
        gui_obj_hidden((gui_obj_t *)s_launcher_win, false);
        return;
    }

    gui_obj_t *root = gui_obj_get_root();

    /* Create launcher window */
    s_launcher_win = gui_win_create(root, "launcher_win", 0, 0, s_screen_w, s_screen_h);

    /* Background */
    gui_rect_create(s_launcher_win, "launcher_bg", 0, 0, s_screen_w, s_screen_h, 0,
                    gui_rgba(20, 20, 30, 255));

    /* Title */
    gui_text_t *title = gui_text_create(s_launcher_win, "title", 0, 15, s_screen_w, 20);
    gui_text_set(title, (void *)"Particle Effects", GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, 16, 16);
    gui_text_mode_set(title, CENTER);

    /* Create list - with bar to avoid crash */
    int note_height = 80;
    int space = 0;
    gui_list_t *list = gui_list_create(s_launcher_win, "effect_list",
                                       0, 40, s_screen_w, s_screen_h - 40,
                                       note_height, space, VERTICAL,
                                       note_design, NULL, true);
    gui_list_set_note_num(list, EFFECT_COUNT);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_out_scope(list, 50);
}

static void effect_start(int index)
{
    if (index < 0 || index >= (int)EFFECT_COUNT)
    {
        return;
    }

    /* Hide launcher */
    if (s_launcher_win != NULL)
    {
        gui_obj_hidden((gui_obj_t *)s_launcher_win, true);
    }

    gui_obj_t *root = gui_obj_get_root();

    /* Create effect window */
    s_effect_win = gui_win_create(root, "effect_win", 0, 0, s_screen_w, s_screen_h);

    /* Black background */
    gui_rect_create(s_effect_win, "effect_bg", 0, 0, s_screen_w, s_screen_h, 0,
                    gui_rgba(0, 0, 0, 255));

    /* Start effect */
    s_current_effect = s_effects[index].init();

    /* Back button - centered at top for round screen */
    int btn_w = 80;
    int btn_x = (s_screen_w - btn_w) / 2;
    gui_rounded_rect_t *back_btn = gui_rect_create(s_effect_win, "back_btn",
                                                   btn_x, 10, btn_w, 35, 8, gui_rgba(80, 80, 100, 200));
    gui_rect_on_click(back_btn, on_back_click, NULL);

    gui_text_t *back_text = gui_text_create(s_effect_win, "back_text", btn_x, 18, btn_w, 20);
    gui_text_set(back_text, (void *)"Back", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 4, 16);
    gui_text_mode_set(back_text, CENTER);
}

static void effect_stop(void)
{
    /* Clean up effect widget */
    if (s_current_effect != NULL)
    {
        gui_obj_tree_free((gui_obj_t *)s_current_effect);
        s_current_effect = NULL;
    }

    /* Clean up effect window */
    if (s_effect_win != NULL)
    {
        gui_obj_tree_free((gui_obj_t *)s_effect_win);
        s_effect_win = NULL;
    }

    /* Show launcher */
    launcher_show();
}

/*============================================================================*
 *                           Public API
 *============================================================================*/

int particle_launcher_init(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    s_screen_w = dc->screen_width;
    s_screen_h = dc->screen_height;

    launcher_show();

    gui_set_keep_active_time(0xFFFFFFFF);
    return 0;
}
