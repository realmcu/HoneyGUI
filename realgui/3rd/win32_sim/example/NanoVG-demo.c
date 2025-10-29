#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "components_init.h"
#include "fb_sdl.h"
#include "nanovg.h"
#include "nanovg_agge.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>


#define NANOVG_CANVAS_WIDTH   200
#define NANOVG_CANVAS_HEIGHT   100

typedef struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char align : 1;
    unsigned char resize: 2; //0-no resize;1-50%(x&y);2-70%;3-80%
    unsigned char compress: 1;
    unsigned char jpeg: 1;
    unsigned char idu: 1;
    unsigned char rsvd : 1;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
} gui_rgb_data_head_t;

static int nanovg_example(void)
{
    printf("nanovg example test code:\n");

    uint8_t *pixel = malloc(NANOVG_CANVAS_WIDTH * NANOVG_CANVAS_HEIGHT * DRV_PIXEL_BITS / 8);

    size_t size = NANOVG_CANVAS_WIDTH * NANOVG_CANVAS_HEIGHT * DRV_PIXEL_BITS / 8;

    memset(pixel, 0x0, NANOVG_CANVAS_WIDTH * NANOVG_CANVAS_HEIGHT * DRV_PIXEL_BITS / 8);

    NVGcontext *vg;

    vg = nvgCreateAGGE(NANOVG_CANVAS_WIDTH, NANOVG_CANVAS_HEIGHT,
                       NANOVG_CANVAS_WIDTH * (DRV_PIXEL_BITS >> 3),
                       (DRV_PIXEL_BITS >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, pixel);

    nvgBeginFrame(vg, NANOVG_CANVAS_WIDTH, NANOVG_CANVAS_HEIGHT, 1);

    nvgBeginPath(vg);

    nvgCircle(vg, 50, 50, 50);

    NVGpaint paint = nvgLinearGradient(vg, 50, 0, 50, 100,  nvgRGBA(255, 0, 0, 255), nvgRGBA(255, 0, 0,
                                       0));

    nvgFillPaint(vg, paint);

    nvgFill(vg);

    const char *filename = "nanovg_generate_pixel.bin";
    gui_rgb_data_head_t head;
    head.scan = 0;
    head.align = 0;
    head.resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head.compress = 0;
    head.rsvd = 0;
    head.type = 9;
    head.version = 0;
    head.rsvd2 = 0;
    head.w = NANOVG_CANVAS_WIDTH;
    head.h = NANOVG_CANVAS_HEIGHT;
    head.idu = false;
    head.jpeg = false;

    FILE *fp = fopen(filename, "wb");
    fwrite(&head, 1, sizeof(gui_rgb_data_head_t), fp);

    for (uint32_t i = 0; i < size; i += 4)
    {
        fwrite(pixel + i + 3, 1, 1, fp);
    }


    fclose(fp);


    port_direct_draw_bitmap_to_lcd(0, 0, NANOVG_CANVAS_WIDTH, NANOVG_CANVAS_HEIGHT, (uint8_t *)pixel);

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);

    free(pixel);


    return 0;
}

// GUI_INIT_APP_EXPORT(nanovg_example);