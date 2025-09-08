#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "components_init.h"
#include "fb_sdl.h"
#include "nanovg.h"
#include "nanovg_agge.h"


static int nanovg_example(void)
{
    printf("nanovg example test code:\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));

    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HIGHT * sizeof(uint16_t));

    NVGcontext *vg;

    vg = nvgCreateAGGE(DRV_LCD_WIDTH, DRV_LCD_HIGHT, DRV_LCD_WIDTH * (DRV_PIXEL_BITS >> 3),
                       (DRV_PIXEL_BITS >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, pixel);

    nvgBeginFrame(vg, DRV_LCD_WIDTH, DRV_LCD_HIGHT, 1);

    nvgBeginPath(vg);
    nvgRect(vg, 100, 100, 120, 30);
    nvgCircle(vg, 120, 120, 5);
    nvgPathWinding(vg, NVG_HOLE);   // Mark circle as a hole.
    nvgFillColor(vg, nvgRGBA(255, 192, 0, 255));
    nvgFill(vg);


    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT, (uint8_t *)pixel);

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);

    free(pixel);


    return 0;
}

// GUI_INIT_APP_EXPORT(nanovg_example);