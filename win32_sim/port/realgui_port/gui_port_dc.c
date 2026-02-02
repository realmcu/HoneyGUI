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



void port_gui_lcd_update(struct gui_dispdev *dc)
{
    uint32_t x = dc->section.x1;
    uint32_t y = dc->section.y1;
    uint32_t w = dc->section.x2 - dc->section.x1 + 1;
    uint32_t h = dc->section.y2 - dc->section.y1 + 1;

    sdl_driver_update_window(dc->frame_buf, x, y, w, h);

}

static struct gui_dispdev dc =
{
#ifdef USE_DC_PFB
    .fb_height = LCD_SECTION_HEIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .type = DC_RAMLESS,
#else
    .fb_height = DRV_LCD_HEIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .type = DC_SINGLE,
#endif
    .section = {0, 0, 0, 0},
    .section_count = 0,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HEIGHT,
    .bit_depth = DRV_PIXEL_BITS,
    .lcd_update = port_gui_lcd_update,

};


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
    /* Example: enable canvas mode with circular screen */
    /* corner_radius: 0=rect, -1=circle, >0=rounded corner radius */

    int panel_w = DRV_LCD_WIDTH + 100, panel_h = 150;

    sdl_driver_set_canvas(DRV_LCD_WIDTH + 100, DRV_LCD_HEIGHT + 100 + panel_h, NULL);
    sdl_driver_set_screen(50, 50, DRV_LCD_CORNER_RADIUS);

    /* Control panel with 4 evenly distributed buttons */

    sdl_panel_set_control_panel(0, DRV_LCD_HEIGHT + 100, panel_w, panel_h);

    /* Auto-calculate button layout */
    int btn_size = panel_h * 0.4;  // 40% of panel height
    int btn_y = (panel_h - btn_size) / 2;  // Vertical center
    int slot_w = panel_w / 4;  // Each button gets 1/4 of panel width

    sdl_panel_add_button(slot_w * 0 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         on_home_press, on_home_release);

    sdl_panel_add_button(slot_w * 1 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         on_back_press, on_back_release);

    sdl_panel_add_button(slot_w * 2 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         on_menu_press, on_menu_release);

    sdl_panel_add_button(slot_w * 3 + (slot_w - btn_size) / 2, btn_y, btn_size, btn_size, -1,
                         on_power_press, on_power_release);

    sdl_driver_init(DRV_LCD_WIDTH, DRV_LCD_HEIGHT, DRV_PIXEL_BITS);
    gui_dc_info_register(&dc);
#ifdef USE_DC_PFB
    dc.disp_buf_1 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
    dc.disp_buf_2 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#else
    dc.frame_buf = malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#endif

}





