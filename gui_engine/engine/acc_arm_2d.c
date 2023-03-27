#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>



static void arm_2d_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        struct rtgui_rect *rect)
{

}


static void arm_2d_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{

}

static void arm_2d_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          struct rtgui_rect *rect)
{

}


void arm_2d_sw_bilt(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    struct gui_rgb_data_head *head = image->data;

    char img_type = head->type;

    if ((img_type == RGB888) && (dc_bytes_per_pixel == 4))
    {
        arm_2d_blit_rgb888_2_argb8888(image, dc, rect);
    }
    else if ((img_type == RGB565) && (dc_bytes_per_pixel == 4))
    {
        arm_2d_blit_rgb565_2_argb8888(image, dc, rect);
    }
    else if ((img_type == RGB565) && (dc_bytes_per_pixel == 2))
    {
        arm_2d_blit_rgb565_2_rgb565(image, dc, rect);
    }
}

