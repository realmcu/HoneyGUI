#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>


static bool gui_image_rgb_mem_check(draw_img_t *image);

static bool gui_image_rgb_mem_load(draw_img_t *image, bool load);

static void gui_image_rgb_mem_unload(draw_img_t *image);

static void gui_image_rgb_mem_blit(draw_img_t *image, struct gui_dispdev *dc,
                                   gui_rect_t *rect);


struct gui_image_engine gui_image_rgb_mem_engine =
{
    "rgb_mem",
    { NULL },
    gui_image_rgb_mem_check,
    gui_image_rgb_mem_load,
    gui_image_rgb_mem_unload,
    gui_image_rgb_mem_blit,
};

static bool gui_image_rgb_mem_check(draw_img_t *image)
{
    return true;
}

static bool gui_image_rgb_mem_load(draw_img_t *image,
                                   bool load)
{
    struct gui_rgb_data_head head;
    memcpy(&head, image->data, sizeof(head));

    /* set image information */
    image->img_w = head.w;
    image->img_h = head.h;
    image->engine = &gui_image_rgb_mem_engine;

    //rt_kprintf("w = %d, h = %d, bit_depth =%d\n", image->img_w, image->img_h, image->bit_depth);

    return true;
}

static void gui_image_rgb_mem_unload(draw_img_t *image)
{

}


#include "acc_engine.h"

static void gui_image_rgb_mem_blit(draw_img_t *image, struct gui_dispdev *dc,
                                   gui_rect_t *rect)
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


void gui_image_rgb_mem_init(void)
{
    /* register rgb565 on image system */
    gui_image_register_engine(&gui_image_rgb_mem_engine);
}

