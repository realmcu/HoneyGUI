
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "tp_algo.h"
#include "Board_GLCD.h"

#define DRV_LCD_WIDTH   320
#define DRV_LCD_HIGHT   240
#define DRV_PIXEL_BITS  16

uint8_t resource_root[1024] = {0};

static struct gui_touch_data raw_data = {0};
static uint8_t framebuffer[320 * 240 * 2] = {0};


void port_gui_lcd_update(struct gui_dispdev *dc)
{
    GLCD_DrawBitmap(0, 0, 320, 240, dc->frame_buf);
    return;
}

static struct gui_dispdev dc =
{
    .bit_depth = DRV_PIXEL_BITS,
    .fb_height = DRV_LCD_HIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .driver_ic_fps = 30,
    .driver_ic_active_width = DRV_LCD_WIDTH,
    .type = DC_SINGLE,

    .section = {0, 0, 0, 0},
    .section_count = 0,
    .adaption = false,
    .scale_x = 1,
    .scale_y = 1,
    .disp_buf_1 = NULL,
    .disp_buf_2 = NULL,
    .frame_buf = framebuffer,

    .lcd_update = port_gui_lcd_update,
};





void gui_port_dc_init(void)
{
    gui_dc_info_register(&dc);
}



struct gui_touch_data *port_touchpad_get_data()
{
    return &raw_data;
}


static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .tp_height = 0,
    .tp_witdh = 0,
    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 300,
    .quick_slide_time_ms = 50,
};


void gui_port_indev_init(void)
{
    gui_indev_info_register(&indev);
}




