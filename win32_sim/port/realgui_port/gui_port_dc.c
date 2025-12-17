#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "unistd.h"
#include "sdl_driver.h"

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



void gui_port_dc_init(void)
{
    /* Example: enable canvas mode with circular screen */
    /* corner_radius: 0=rect, -1=circle, >0=rounded corner radius */
    sdl_driver_set_canvas(DRV_LCD_WIDTH + 100, DRV_LCD_HEIGHT + 100, 50, 50, DRV_LCD_CORNER_RADIUS,
                          NULL);

    sdl_driver_init(DRV_LCD_WIDTH, DRV_LCD_HEIGHT, DRV_PIXEL_BITS);
    gui_dc_info_register(&dc);
#ifdef USE_DC_PFB
    dc.disp_buf_1 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
    dc.disp_buf_2 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#else
    dc.frame_buf = malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#endif

}





