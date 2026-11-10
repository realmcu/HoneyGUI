/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "unistd.h"
#include "sdl_driver.h"
#include "sdl_driver_panel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DRV_LCD_CORNER_RADIUS //rounded corner radius
#define DRV_LCD_CORNER_RADIUS 0
#endif

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH   480
#endif
#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT   480
#endif

#ifndef DRV_PIXEL_BITS
#define DRV_PIXEL_BITS   16
#endif

#define USE_DC_PFB

#ifdef USE_DC_PFB
#define LCD_SECTION_HEIGHT 20
#endif

/*============================================================================*
 *                     Screen geometry from project.json
 *============================================================================*/

/* Assets directory given on the command line ("gui <dir>"); NULL if none. */
extern const char *asset_dir;

/* A Designer project describes its screen in project.json, beside ui/ under
 * the directory given on the command line. Reading it here lets one build run
 * any resolution without recompiling, like web_sim_configure() in the browser
 * simulator. Without the file the compiled-in DRV_LCD_* values apply. */
static int  screen_w = DRV_LCD_WIDTH;
static int  screen_h = DRV_LCD_HEIGHT;
static int  screen_bits = DRV_PIXEL_BITS;
static int  screen_radius = DRV_LCD_CORNER_RADIUS;

/* Value of a top-level "key" in buf, or NULL. The keys of interest appear once
 * at the top level, so the first match is the one wanted. */
static const char *screen_json_value(const char *buf, const char *key)
{
    char pattern[32];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(buf, pattern);
    if (!p)
    {
        return NULL;
    }
    p = strchr(p + strlen(pattern), ':');
    if (!p)
    {
        return NULL;
    }
    do
    {
        p++;
    }
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n');
    return p;
}

static void screen_load_project_json(void)
{
    char path[1024], *text;
    long size;

    if (!asset_dir || !asset_dir[0] ||
        snprintf(path, sizeof(path), "%s/project.json", asset_dir) >= (int)sizeof(path))
    {
        return;
    }
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        return;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (size <= 0 || size > 64 * 1024)
    {
        fclose(fp);
        return;
    }
    text = malloc((size_t)size + 1);
    if (!text)
    {
        fclose(fp);
        return;
    }
    size = (long)fread(text, 1, (size_t)size, fp);
    fclose(fp);
    text[size] = '\0';

    /* "resolution": "410X502" */
    const char *p = screen_json_value(text, "resolution");
    char sep = 0;
    if (p && sscanf(p, "\"%d%c%d\"", &screen_w, &sep, &screen_h) == 3 &&
        (sep == 'x' || sep == 'X') && screen_w > 0 && screen_h > 0)
    {
        /* "pixelMode": "RGB565" | "RGB888" | "ARGB8888" */
        const char *mode = screen_json_value(text, "pixelMode");
        if (mode && strncmp(mode, "\"RGB888\"", 8) == 0)        { screen_bits = 24; }
        else if (mode && strncmp(mode, "\"ARGB8888\"", 10) == 0) { screen_bits = 32; }
        else                                                     { screen_bits = 16; }

        /* "cornerRadius": -1 for a round screen, as in the browser simulator */
        const char *radius = screen_json_value(text, "cornerRadius");
        screen_radius = radius ? atoi(radius) : DRV_LCD_CORNER_RADIUS;
        if (screen_radius < 0)
        {
            screen_radius = (screen_w < screen_h ? screen_w : screen_h) / 2;
        }
        gui_log("screen: %dx%d, %d bits, corner radius %d (from project.json)\n",
                screen_w, screen_h, screen_bits, screen_radius);
    }
    free(text);
}

void port_gui_lcd_update(struct gui_dispdev *dc)
{
    uint32_t x = dc->section.x1;
    uint32_t y = dc->section.y1;
    uint32_t w = dc->section.x2 - dc->section.x1 + 1;
    uint32_t h = dc->section.y2 - dc->section.y1 + 1;

    sdl_driver_update_window(dc->frame_buf, x, y, w, h);

}

/* gui_dispdev example define start */
/* The screen fields are written by gui_port_dc_init() before gui_dc_info_register(),
 * so a project.json on the command line can override the DRV_LCD_* defaults. */
static struct gui_dispdev dc =
{
    .section = {0, 0, 0, 0},
    .section_count = 0,
    .lcd_update = port_gui_lcd_update,

};
/* gui_dispdev example define end */


/* Button state variables for keyboard and panel buttons */
extern bool home_state;
extern bool back_state;
extern bool menu_state;
extern bool power_state;
extern uint32_t home_timestamp_ms_press;
extern uint32_t home_timestamp_ms_release;
extern uint32_t home_timestamp_ms_pressing;
extern uint32_t back_timestamp_ms_press;
extern uint32_t back_timestamp_ms_release;
extern uint32_t back_timestamp_ms_pressing;
extern uint32_t menu_timestamp_ms_press;
extern uint32_t menu_timestamp_ms_release;
extern uint32_t menu_timestamp_ms_pressing;
extern uint32_t power_timestamp_ms_press;
extern uint32_t power_timestamp_ms_release;
extern uint32_t power_timestamp_ms_pressing;

static void on_home_press(void)
{
    home_state = true;
    home_timestamp_ms_press = gui_ms_get();
    gui_log("Home button pressed\n");
}

static void on_home_release(void)
{
    home_state = false;
    home_timestamp_ms_release = gui_ms_get();
    gui_log("Home button released\n");
}

static void on_back_press(void)
{
    back_state = true;
    back_timestamp_ms_press = gui_ms_get();
    gui_log("Back button pressed\n");
}

static void on_back_release(void)
{
    back_state = false;
    back_timestamp_ms_release = gui_ms_get();
    gui_log("Back button released\n");
}

static void on_menu_press(void)
{
    menu_state = true;
    menu_timestamp_ms_press = gui_ms_get();
    gui_log("Menu button pressed\n");
}

static void on_menu_release(void)
{
    menu_state = false;
    menu_timestamp_ms_release = gui_ms_get();
    gui_log("Menu button released\n");
}

static void on_power_press(void)
{
    power_state = true;
    power_timestamp_ms_press = gui_ms_get();
    gui_log("Power button pressed\n");
}

static void on_power_release(void)
{
    power_state = false;
    power_timestamp_ms_release = gui_ms_get();
    gui_log("Power button released\n");
}

void gui_port_dc_init(void)
{
    screen_load_project_json();

#ifdef USE_DC_PFB
    dc.fb_height = LCD_SECTION_HEIGHT;
    dc.fb_width = screen_w;
    dc.type = DC_RAMLESS;
#else
    dc.fb_height = screen_h;
    dc.fb_width = screen_w;
    dc.type = DC_SINGLE;
#endif
    dc.screen_width = screen_w;
    dc.screen_height = screen_h;
    dc.bit_depth = screen_bits;

    int panel_w = screen_w + 100, panel_h = 150;

    sdl_driver_set_canvas(screen_w + 100, screen_h + 100 + panel_h, NULL);
    sdl_driver_set_screen(50, 50, screen_radius);

    /* Control panel with 4 evenly distributed buttons */

    sdl_panel_set_control_panel(0, screen_h + 100, panel_w, panel_h);

    /* Auto-calculate button layout */
    int btn_size = panel_h * 0.4;  // 40% of panel height
    int btn_y = (panel_h - btn_size) / 2;  // Vertical center
    int slot_w = panel_w / 4;  // Each button gets 1/4 of panel width

    sdl_panel_add_button(slot_w * 0 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         "port/realgui_port/home.bmp",
                         on_home_press, on_home_release);

    sdl_panel_add_button(slot_w * 1 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         "port/realgui_port/back.bmp",
                         on_back_press, on_back_release);

    sdl_panel_add_button(slot_w * 2 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         "port/realgui_port/menu.bmp",
                         on_menu_press, on_menu_release);

    sdl_panel_add_button(slot_w * 3 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         "port/realgui_port/power.bmp",
                         on_power_press, on_power_release);

    sdl_driver_init(screen_w, screen_h, screen_bits);
    gui_dc_info_register(&dc);
#ifdef USE_DC_PFB
    dc.disp_buf_1 =  malloc(dc.fb_height * dc.fb_width * screen_bits / 8);
    dc.disp_buf_2 =  malloc(dc.fb_height * dc.fb_width * screen_bits / 8);
#else
    dc.frame_buf = malloc(dc.fb_height * dc.fb_width * screen_bits / 8);
#endif

}





