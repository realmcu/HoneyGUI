#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>


static bool rtgui_image_rgb_mem_check(draw_img_t *image);

static bool rtgui_image_rgb_mem_load(draw_img_t *image, bool load);

static void rtgui_image_rgb_mem_unload(draw_img_t *image);

static void rtgui_image_rgb_mem_blit(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect);


struct rtgui_image_engine rtgui_image_rgb_mem_engine =
{
    "rgb_mem",
    { NULL },
    rtgui_image_rgb_mem_check,
    rtgui_image_rgb_mem_load,
    rtgui_image_rgb_mem_unload,
    rtgui_image_rgb_mem_blit,
};

static bool rtgui_image_rgb_mem_check(draw_img_t *image)
{
    return true;
}

static bool rtgui_image_rgb_mem_load(draw_img_t *image,
                                     bool load)
{
    struct gui_rgb_data_head head;
    memcpy(&head, image->data, sizeof(head));

    /* set image information */
    image->img_w = head.w;
    image->img_h = head.h;
    image->engine = &rtgui_image_rgb_mem_engine;

    //rt_kprintf("w = %d, h = %d, bit_depth =%d\n", image->img_w, image->img_h, image->bit_depth);

    return true;
}

static void rtgui_image_rgb_mem_unload(draw_img_t *image)
{

}


#include "acc_engine.h"

static void rtgui_image_rgb_mem_blit(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect)
{
    if (gui_get_acc() != NULL)
    {
        gui_get_acc()->blit(image, dc, rect);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}


void rtgui_image_rgb_mem_init(void)
{
    /* register rgb565 on image system */
    rtgui_image_register_engine(&rtgui_image_rgb_mem_engine);
}

