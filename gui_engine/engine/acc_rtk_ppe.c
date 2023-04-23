#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include <rtl876x_ppe.h>
#include <drv_lcd.h>

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))

extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect);

void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    ppe_buffer_t source, target;
    memset(&source, 0, sizeof(ppe_buffer_t));
    memset(&target, 0, sizeof(ppe_buffer_t));
    struct gui_rgb_data_head *head = image->data;

    if (dc->bit_depth == 16)
    {
        target.format = PPE_BGR565;
    }
    else if (dc->bit_depth == 32)
    {
        target.format = PPE_ABGR8888;
    }
    else if (dc->bit_depth == 24)
    {
        target.format = PPE_BGR888;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;

    switch (head->type)
    {
    case RGB565:
        source.format = PPE_BGR565;
        break;
    case RGB888:
        source.format = PPE_BGR888;
        break;
    case RGBA8888:
        source.format = PPE_ABGR8888;
        break;
    default:
        break;
    }
    source.width = image->img_w;
    source.height = image->img_h;
    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.memory = (void *)source.address;
    source.global_alpha_en = true;
    source.global_alpha = image->opacity_value << 24;
    if (image->blend_mode == IMG_MAGIC_MATRIX)
    {
        if ((image->matrix->m[0][1] == 0) && (image->matrix->m[1][0] == 0))
        {
            if ((image->matrix->m[0][0] != 1) || (image->matrix->m[1][1] != 1) && 1)
            {
                float scale_x = image->matrix->m[0][0], scale_y = image->matrix->m[1][1];
                if ((image->img_w == (int)(image->img_w * scale_x)) &&
                    (image->img_h == (int)(image->img_h * scale_y)))
                {
                    PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                    PPE_blend(&source, &target, &trans);
                    return;
                }
                ppe_buffer_t scaled_img;
                PPE_translate_t trans;
                PPE_rect_t range;
                PPE_rect_t scale_rect;
                memset(&scaled_img, 0, sizeof(ppe_buffer_t));
                scaled_img.width = image->img_w * scale_x;
                scaled_img.height = image->img_h * scale_y > (dc->section.y2 - dc->section.y1) ?
                                    (dc->section.y2 - dc->section.y1) : image->img_h * scale_y;
                scaled_img.height += ceil(scale_y);
                uint32_t modified_height = image->img_h * scale_y;
                switch (head->type)
                {
                case RGB565:
                    scaled_img.format = PPE_BGR565;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(scale_y)) * 2);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(
                                                                               scale_y) * 2 + 1) * 2);
                    }
                    break;
                case RGB888:
                    scaled_img.format = PPE_BGR888;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(scale_y)) * 3);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(
                                                                               scale_y) + 1) * 3);
                    }
                    break;
                case RGBA8888:
                    scaled_img.format = PPE_BGRA8888;
                    if (dc->type == DC_SINGLE)
                    {
                        //scaled_img.memory = tlsf_malloc(tlsf, scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(scale_y)) * 4);
                    }
                    else if (dc->type == DC_RAMLESS)
                    {
                        scaled_img.memory = gui_malloc(scaled_img.width * (dc->section.y2 - dc->section.y1 + (int)ceil(
                                                                               scale_y) + 1) * 4);
                    }
                    break;
                default:
                    return;
                }
                trans.x = (int)image->matrix->m[0][2];
                trans.y = 0;
                range.left = 0;
                range.right = scaled_img.width - 1;
                range.top = 0;
                range.bottom = scaled_img.height - 1;
                scaled_img.global_alpha_en = true;
                scaled_img.global_alpha = image->opacity_value << 24;
                if ((dc->section.y1 <= rect->y1) && (dc->section.y2 > rect->y1))
                {
                    scale_rect.top = 0;
                    if ((rect->y1 + modified_height) <= dc->section.y2)
                    {
                        scale_rect.bottom = source.height - 1;
                    }
                    else
                    {
                        scale_rect.bottom = (dc->section.y2 - rect->y1) / scale_y;
                    }
                    trans.y = (int)image->matrix->m[1][2] - dc->section.y1;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                    PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
                    if (err == PPE_SUCCESS)
                    {
                        PPE_blend(&scaled_img, &target, &trans);
                    }
                }
                else if ((dc->section.y2 < (rect->y1 + modified_height)) && (dc->section.y1 > rect->y1))
                {
                    scale_rect.top = (dc->section.y1 - rect->y1) / scale_y;
                    scale_rect.bottom = (dc->section.y2 - rect->y1) / scale_y;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                    PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
                    if (err == PPE_SUCCESS)
                    {
                        PPE_blend(&scaled_img, &target, &trans);
                    }
                }
                else if ((dc->section.y2 >= (rect->y1 + modified_height)) && (dc->section.y1 > rect->y1)
                         && (dc->section.y1 < (rect->y1 + modified_height)))
                {
                    scale_rect.top = (dc->section.y1 - rect->y1) / scale_y;
                    scale_rect.bottom = source.height - 1;
                    scaled_img.height = rect->y1 + modified_height - dc->section.y1;
                    scale_rect.left = 0;
                    scale_rect.right = source.width - 1;
                    PPE_ERR err = PPE_Scale_Rect(&source, &scaled_img, scale_x, scale_y, &scale_rect);
                    if (err == PPE_SUCCESS)
                    {
                        PPE_blend(&scaled_img, &target, &trans);
                    }
                }
                if (dc->type == DC_SINGLE)
                {
                    //tlsf_free(tlsf, scaled_img.memory);
                }
                else if (dc->type == DC_RAMLESS)
                {
                    gui_free(scaled_img.memory);
                }
            }
            else
            {
                source.width = image->img_w;
                source.height = image->img_h;
                PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
                PPE_blend(&source, &target, &trans);
            }
        }
        else
        {
            sw_acc_blit(image, dc, rect);
        }
    }
    else
    {
        if (image->blend_mode == IMG_FILTER_BLACK)
        {
            source.color_key_en = true;
            source.color_key_value = 0x00000000;
        }

        PPE_translate_t trans = {.x = rect->x1 - dc->section.x1, .y = rect->y1 - dc->section.y1};
        PPE_blend(&source, &target, &trans);
    }
}
