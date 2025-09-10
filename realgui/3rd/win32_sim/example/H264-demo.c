#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "components_init.h"
#include "fb_sdl.h"
#include "gui_h264bsd.h"

#include "earth_240x240_header.txt"


static int h264bsd_example(void)
{
    printf("h264bsd example test code:\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));
    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));

    void *handle = gui_h264bsd_create_decoder((uint8_t *)_acearth_240x240_header + 24,
                                              sizeof(_acearth_240x240_header));

    gui_h264bsd_get_frame(handle, pixel, DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));

    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT, (uint8_t *)pixel);




    return 0;
}

// GUI_INIT_APP_EXPORT(h264bsd_example);