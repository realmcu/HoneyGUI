#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include <gui_matrix.h>
#include <rtl_PPEV2.h>
#include <rtl_imdc.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gdma.h>
#include <dma_channel.h>
#include "trace.h"
#include "rtl876x_gpio.h"
#include "os_mem.h"
#include "rtl876x_pinmux.h"
#include "math.h"
#include "fmc_api_ext.h"
#include "os_sync.h"
#include "hal_imdc.h"


extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect);

#define PPEV2_ACC_MIN_OPA       3
#define PPEV2_TESS_LENGTH       92

#include "section.h"
#define CACHE_BUF_SIZE           (42 * 1024)
SHM_DATA_SECTION static uint8_t cache_buf1[CACHE_BUF_SIZE];
static uint8_t *cache_buf2;
static uint8_t *cache_buf = cache_buf1;
static uint32_t last_cache_size = 0;
static void change_cache_buf(uint32_t cache_size)
{
    if (cache_buf == cache_buf1)
    {
        cache_buf2 = cache_buf1 + (CACHE_BUF_SIZE - cache_size - 4);
        cache_buf = cache_buf2;
    }
    else if (cache_buf == cache_buf2)
    {
        cache_buf = cache_buf1;
    }
#if F_APP_GUI_USE_PSRAM
    else if ((uint32_t)cache_buf > PSRAM_GUI_HEAP_ADDR)
    {
        gui_free(cache_buf);
        cache_buf = cache_buf1;
    }
#endif
    else
    {
        cache_buf = cache_buf1;
    }
}
static void restore_cache_buf(void)
{
#if F_APP_GUI_USE_PSRAM
    if ((uint32_t)cache_buf > PSRAM_GUI_HEAP_ADDR)
    {
        gui_free(cache_buf);
    }
#endif
    cache_buf = cache_buf1;
}

#if F_APP_GUI_USE_PSRAM
static void cache_on_psram(uint32_t size)
{
    if ((uint32_t)cache_buf > PSRAM_GUI_HEAP_ADDR)
    {
        gui_free(cache_buf);
    }
    cache_buf = gui_malloc(size);
}
#endif

static bool memcpy_by_dma(ppe_rect_t *p_rect, ppe_buffer_t *source)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(source->format);
    if (p_rect->w * p_rect->h * pixel_size > CACHE_BUF_SIZE)
    {
        return false;
    }
    hal_imdc_dma_info info;
    info.length = p_rect->w * pixel_size;
    info.dst_stride = p_rect->w * pixel_size;
    info.src_stride = source->width * pixel_size;
    info.height = p_rect->h;
    uint32_t start_address = source->address + (p_rect->x + p_rect->y * source->width) * pixel_size;
    hal_dma_copy(&info, (uint8_t *)start_address, cache_buf);
    source->address = (uint32_t)cache_buf;
    return true;
}




static bool memcpy_by_imdc(ppe_rect_t *p_rect, ppe_buffer_t *source)
{
#if !F_APP_GUI_USE_PSRAM
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(source->format);
    if (p_rect->w * p_rect->h * pixel_size > CACHE_BUF_SIZE)
    {
        return false;
    }
#endif
    hal_imdc_decompress_info info;
    info.start_column = p_rect->x;
    info.end_column = p_rect->x + p_rect->w - 1;
    info.start_line = p_rect->y;
    info.end_line = p_rect->y + p_rect->h - 1;
    info.raw_data_address = source->address;
    if (hal_imdc_decompress(&info, cache_buf))
    {
        source->address = (uint32_t)cache_buf;
        return true;
    }
    else
    {
        return false;
    }
}



void bare_blit_by_dma(ppe_buffer_t *target, ppe_buffer_t *source, ppe_rect_t *src_rect,
                      ppe_rect_t *dst_trans)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(target->format);
    uint32_t dst_start_address = target->address + (dst_trans->x + dst_trans->y * target->width) *
                                 pixel_size;
    uint32_t src_start_address = source->address + (src_rect->x + src_rect->y * source->width) *
                                 pixel_size;
    hal_imdc_dma_info info;
    info.length = src_rect->w * pixel_size;
    info.src_stride = source->width * pixel_size;
    info.dst_stride = target->width * pixel_size;
    info.height = src_rect->h;
    hal_dma_copy(&info, (uint8_t *)src_start_address, (uint8_t *)dst_start_address);
}

void bare_blit_by_imdc(ppe_buffer_t *target, ppe_buffer_t *source, ppe_rect_t *src_rect,
                       ppe_rect_t *dst_trans)
{
    uint8_t pixel_size = PPEV2_Get_Pixel_Size(target->format);
    uint32_t dst_start_address = target->address + (dst_trans->x + dst_trans->y * target->width) *
                                 pixel_size;
    hal_imdc_decompress_info info;
    info.start_line = src_rect->y;
    info.end_line = src_rect->y + src_rect->h - 1;
    info.start_column = src_rect->x;
    info.end_column = src_rect->x + src_rect->w - 1;
    info.length = src_rect->w * pixel_size;
    info.dst_stride = target->width * pixel_size;
    info.raw_data_address = source->address;
    hal_imdc_decompress_rect(&info, (uint8_t *)dst_start_address);
}

static float get_x(float *line, float y)
{
    float x;
    if (line[0] == 0)
    {
        x = 0;
    }
    else if (line[1] == 0)
    {
        x = -line[2];
    }
    else
    {
        x = (-line[2] - line[1] * y) / line[0];
    }
    return x;
}

void acc_get_intersect_area(draw_img_t *image, ppe_rect_t *new_rect, ppe_rect_t *dst,
                            struct gui_dispdev *dc)
{
    memcpy(new_rect, dst, sizeof(ppe_rect_t));
    float left = dst->x * 1.0f, right = (dst->x + dst->w - 1) * 1.0f;
    float top = dst->y * 1.0f;
    float bottom = (dst->y + dst->h - 1) * 1.0f;
    float x_min = dc->fb_width, x_max = -1;
    float *lines = (float *)image->acc_user;
    for (int i = 0; i < 4; i++)
    {
        float x = get_x(&lines[i * 3], top);
        if (x < x_min)
        {
            x_min = x;
        }
        if (x > x_max)
        {
            x_max = x;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        float x = get_x(&lines[i * 3], bottom);
        if (x < x_min)
        {
            x_min = x;
        }
        if (x > x_max)
        {
            x_max = x;
        }
    }
    if (x_min > left)
    {
        new_rect->x = floor(x_min);
    }
    if (x_max < right)
    {
        new_rect->w = ceil(x_max - new_rect->x) + 1;
    }
    else
    {
        new_rect->w = right - new_rect->x + 1;
    }
}

bool acc_get_src_rect_area(ppe_rect_t *src_rect, gui_rect_t *rect)
{
    int32_t src_hor_end = src_rect->x + src_rect->w - 1;
    int32_t src_ver_end = src_rect->y + src_rect->h - 1;
    if (src_rect->x > rect->x2 || src_rect->y > rect->y2 || src_hor_end < rect->x1 ||
        src_ver_end < rect->y1)
    {
        return false;
    }
    if (src_rect->x < rect->x1)
    {
        src_rect->x = rect->x1;
    }
    if (src_rect->y < rect->y1)
    {
        src_rect->y = rect->y1;
    }
    if (src_hor_end > rect->x2)
    {
        src_hor_end = rect->x2;
    }
    if (src_ver_end > rect->y2)
    {
        src_ver_end = rect->y2;
    }
    int new_w = src_hor_end - src_rect->x + 1;
    int new_h = src_ver_end - src_rect->y + 1;
    if (new_w <= 0 || new_h <= 0)
    {
        return false;
    }
    else
    {
        src_rect->w = new_w;
        src_rect->h = new_h;
        return true;
    }
}

void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct gui_rect *rect)
{
    int32_t x_max = (image->img_target_w + image->img_target_x - 1);
    int32_t y_max = (image->img_target_h + image->img_target_y - 1);
    int32_t x_min = image->img_target_x;
    int32_t y_min = image->img_target_y;
    if (dc->section.y2 < y_min || dc->section.y1 > y_max || dc->section.x2 < x_min ||
        dc->section.x1 > x_max)
    {
        return;
    }
    if (image->opacity_value <= PPEV2_ACC_MIN_OPA)
    {
        return;
    }

    ppe_buffer_t target, source;
    memset(&target, 0, sizeof(ppe_buffer_t));
    memset(&source, 0, sizeof(ppe_buffer_t));
    PPEV2_BLEND_MODE mode = PPEV2_BYPASS_MODE;

    switch (dc->bit_depth)
    {
    case 16:
        target.format = PPEV2_RGB565;
        break;
    case 24:
        target.format = PPEV2_RGB888;
        break;
    case 32:
        target.format = PPEV2_ARGB8888;
        break;
    default:
        return;
    }
    target.address = (uint32_t)dc->frame_buf;
    target.width = dc->fb_width;
    target.height = dc->fb_height;
    target.const_color = 0;
    target.win_x_min = 0;
    target.win_x_max = target.width;
    target.win_y_min = 0;
    target.win_y_max = target.height;

    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
    source.opacity = image->opacity_value;
    source.width = image->img_w;
    source.height = image->img_h;
    struct gui_rgb_data_head *head = image->data;
    switch (head->type)
    {
    case RGB565:
        source.format = PPEV2_RGB565;
        break;
    case RGB888:
        source.format = PPEV2_RGB888;
        break;
    case ARGB8888:
        source.format = PPEV2_ARGB8888;
        break;
    case ARGB8565:
        source.format = PPEV2_ARGB8565;
        break;
    case XRGB8888:
        source.format = PPEV2_XRGB8888;
        break;
    default:
        return;
    }

    bool shape_transform = true;
    if (image->blend_mode == IMG_RECT)
    {
        mode = PPEV2_CONST_MASK_MODE;
    }
    else if ((image->matrix.m[2][2] == 1 && image->matrix.m[0][1] == 0 && \
              image->matrix.m[1][0] == 0 && image->matrix.m[2][0] == 0 && \
              image->matrix.m[2][1] == 0) || image->blend_mode == IMG_COVER_MODE)
    {
        shape_transform = false;
        if ((image->matrix.m[0][0] == 1 && image->matrix.m[1][1] == 1) || image->blend_mode == IMG_RECT)
        {
            if ((image->blend_mode == IMG_BYPASS_MODE && source.format == target.format) ||
                image->blend_mode == IMG_RECT || image->blend_mode == IMG_COVER_MODE)
            {

                if ((x_max < dc->section.x1) || (y_max < dc->section.y1)
                    || (x_min >= dc->section.x1 + dc->fb_width) || (y_min >= dc->section.y1 + dc->fb_height))
                {
                    return;
                }
                ppe_rect_t dst_rect = {.x = x_min - dc->section.x1, .y = y_min - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
                if (dst_rect.x < 0)
                {
                    dst_rect.x = 0;
                }
                if (dst_rect.y < 0)
                {
                    dst_rect.y = 0;
                }
                if (x_max >= dc->section.x1 + dc->fb_width)
                {
                    dst_rect.w = dc->fb_width - dst_rect.x;
                }
                else
                {
                    dst_rect.w = x_max - dc->section.x1 - dst_rect.x + 1;
                }
                if (y_max >= dc->section.y1 + dc->fb_height)
                {
                    dst_rect.h = dc->fb_height - dst_rect.y;
                }
                else
                {
                    dst_rect.h = y_max - dc->section.y1 - dst_rect.y + 1;
                }
                dst_rect.x += dc->section.x1;
                dst_rect.y += dc->section.y1;
                ppe_rect_t src_rect;
                ppe_matrix_t inv_matrix;
                if (image->blend_mode == IMG_BYPASS_MODE || image->blend_mode == IMG_COVER_MODE)
                {
                    memcpy(&inv_matrix, &image->inverse, sizeof(float) * 9);
                    source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
                    source.width = image->img_w;
                    source.height = image->img_h;
                    bool ret = ppe_get_area(&src_rect, &dst_rect, &inv_matrix, &source);
                    if (src_rect.w % 2)
                    {
                        src_rect.w -= 1;
                        if (src_rect.w == 0)
                        {
                            return;
                        }
                    }
                    if (!ret)
                    {
                        //DBG_DIRECT("MAT err! addr %x, section %x", image->data, dc->section_count);
                        return;
                    }
                }
                dst_rect.x -= dc->section.x1;
                dst_rect.y -= dc->section.y1;
                if (image->blend_mode == IMG_BYPASS_MODE || image->blend_mode == IMG_COVER_MODE)
                {
                    if (head->compress)
                    {
                        uint32_t s;
                        s = os_lock();
                        bare_blit_by_imdc(&target, &source, &src_rect, &dst_rect);
                        os_unlock(s);
                    }
                    else
                    {
                        uint32_t s;
                        s = os_lock();
                        bare_blit_by_dma(&target, &source, &src_rect, &dst_rect);
                        os_unlock(s);
                    }
                }
                else if (image->blend_mode == IMG_RECT)
                {
                    gui_rect_file_head_t *rect_header = (gui_rect_file_head_t *)image->data;
                    gui_color_t color;
                    color.color.argb_full = rect_header->color.color.argb_full;
                    color.color.rgba.a = rect_header->color.color.rgba.a * (image->opacity_value * 1.0f / 255);

                    uint8_t tmp = color.color.rgba.b;
                    color.color.rgba.b = color.color.rgba.r;
                    color.color.rgba.r = tmp;
                    PPEV2_Mask(&target, color.color.argb_full, &dst_rect);
                }
                return;
            }
            else
            {
                mode = PPEV2_SRC_OVER_MODE;
            }
        }
        else if (image->blend_mode == IMG_BYPASS_MODE)
        {
            mode = PPEV2_BYPASS_MODE;
        }
        else if (image->blend_mode == IMG_RECT)
        {
            mode = PPEV2_CONST_MASK_MODE;
        }
        else if (image->blend_mode == IMG_FILTER_BLACK)
        {
            mode = PPEV2_SRC_OVER_MODE;
        }
        else
        {
            if (source.format >= PPEV2_BGR888 && source.format <= PPEV2_RGB565 && image->opacity_value == 0xFF)
            {
                mode = PPEV2_BYPASS_MODE;
            }
            else
            {
                mode = PPEV2_SRC_OVER_MODE;
            }
        }
    }
    else if (image->blend_mode == IMG_COVER_MODE)
    {
        mode = PPEV2_BYPASS_MODE;
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE || image->blend_mode == IMG_FILTER_BLACK)
    {
        mode = PPEV2_SRC_OVER_MODE;
    }

    if (image->blend_mode == IMG_FILTER_BLACK)
    {
        source.color_key_enable = PPEV2_COLOR_KEY_INSIDE;
        source.color_key_min = 0;
        source.color_key_max = 0x010101;
    }
    else if (image->blend_mode == IMG_BYPASS_MODE)
    {
        source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE)
    {
        source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
    }
    else if (image->blend_mode == IMG_RECT)
    {
        source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
    }
    else if (image->blend_mode == IMG_COVER_MODE)
    {
        source.color_key_enable = PPEV2_COLOR_KEY_DISABLE;
    }
    else
    {
        return;
    }
    ppe_rect_t constraint, old_rect;
    int32_t tessalation_len = PPEV2_TESS_LENGTH;
    uint32_t block_num = dc->fb_width / tessalation_len;
    if (dc->fb_width % tessalation_len)
    {
        block_num++;
    }
    if (image->blend_mode == IMG_RECT)
    {
        tessalation_len = dc->fb_width;
        block_num = 1;
    }

    if (shape_transform && image->acc_user != NULL)
    {
        ppe_rect_t section_rect = {.x = x_min - dc->section.x1, .y = y_min - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
        if (section_rect.x < 0)
        {
            section_rect.x = 0;
        }
        if (section_rect.y < 0)
        {
            section_rect.y = 0;
        }
        if (x_max >= dc->section.x1 + dc->fb_width)
        {
            section_rect.w = dc->section.x1 + dc->fb_width - section_rect.x;
        }
        else
        {
            section_rect.w = x_max - dc->section.x1 - section_rect.x + 1;
        }
        if (y_max >= dc->section.y1 + dc->fb_height)
        {
            section_rect.h = dc->fb_height - section_rect.y;
        }
        else
        {
            section_rect.h = y_max - dc->section.y1 - section_rect.y + 1;
        }
        section_rect.y += dc->section.y1;
        acc_get_intersect_area(image, &constraint, &section_rect, dc);
    }
    else
    {
        constraint = (ppe_rect_t) {.x = x_min - dc->section.x1, .y = y_min - dc->section.y1, .w = dc->fb_width, .h = dc->fb_height};
        if (constraint.x < 0)
        {
            constraint.x = 0;
        }
        if (constraint.y < 0)
        {
            constraint.y = 0;
        }
        if (x_max >= dc->section.x1 + dc->fb_width)
        {
            constraint.w = dc->section.x1 + dc->fb_width - constraint.x;
        }
        else
        {
            constraint.w = x_max - dc->section.x1 - constraint.x + 1;
        }
        if (y_max >= dc->section.y1 + dc->fb_height)
        {
            constraint.h = dc->fb_height - constraint.y;
        }
        else
        {
            constraint.h = y_max - dc->section.y1 - constraint.y + 1;
        }
        constraint.y += dc->section.y1;
    }
    bool ret = false;
    if ((image->matrix.m[2][0] == 0 && image->matrix.m[2][1] == 0))
    {
        ret = ppe_get_area(&old_rect, &constraint, (ppe_matrix_t *)&image->inverse, &source);
        if (ret)
        {
            if (old_rect.w * old_rect.h * PPEV2_Get_Pixel_Size(source.format) < CACHE_BUF_SIZE)
            {
                block_num = 1;
                tessalation_len = dc->fb_width;
            }
        }
        else
        {
            return;
        }
    }
    for (int i = 0; i < block_num; i++)
    {
        int32_t section_x1 = tessalation_len * i;
        int32_t section_y1 = dc->section.y1;
        if (tessalation_len * (i + 1) > dc->fb_width)
        {
            tessalation_len = dc->fb_width % tessalation_len;
        }
        if ((constraint.x + constraint.w <= section_x1) || (constraint.y + constraint.h <= section_y1)
            || (constraint.x >= section_x1 + tessalation_len) || (constraint.y >= section_y1 + dc->fb_height))
        {
            continue;
        }
        source.win_x_min = 0;
        source.win_x_max = target.width;
        source.win_y_min = 0;
        source.win_y_max = target.height;
        source.address = (uint32_t)image->data + sizeof(struct gui_rgb_data_head);
        source.width = image->img_w;
        source.height = image->img_h;
        float x_ref = 0, y_ref = 0;
        ppe_rect_t ppe_rect = {0};
        memcpy(&ppe_rect, &constraint, sizeof(ppe_rect_t));
        if (block_num != 1)
        {
            if (ppe_rect.x - section_x1 < 0)
            {
                ppe_rect.x = section_x1;
            }
            if (constraint.x + constraint.w > section_x1 + tessalation_len)
            {
                ppe_rect.w = section_x1 + tessalation_len - ppe_rect.x;
            }
            else
            {
                ppe_rect.w = constraint.x + constraint.w - ppe_rect.x;
            }
        }
        ppe_matrix_t inverse;
        memcpy(&inverse, &image->inverse, sizeof(float) * 9);
        ppe_matrix_t pre_trans;
        if (mode != PPEV2_CONST_MASK_MODE)
        {
            if (block_num != 1)
            {
                ret = ppe_get_area(&old_rect, &ppe_rect, &inverse, &source);
                if (!ret)
                {
                    if (i == block_num - 1)
                    {
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            if (rect != NULL)
            {
                bool res = acc_get_src_rect_area(&old_rect, rect);
                if (!res)
                {
                    if (i == block_num - 1)
                    {
                        return;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            uint32_t new_cache_size = old_rect.w * old_rect.h * PPEV2_Get_Pixel_Size(source.format) + 4;
            if (new_cache_size < (CACHE_BUF_SIZE - last_cache_size - 4))
            {
                change_cache_buf(new_cache_size);
                last_cache_size = new_cache_size;
            }
            else if (new_cache_size < CACHE_BUF_SIZE)
            {
                PPEV2_Finish();
                restore_cache_buf();
                last_cache_size = new_cache_size;
            }
#if F_APP_GUI_USE_PSRAM
            else
            {
                if (head->compress)
                {
                    PPEV2_Finish();
                    cache_on_psram(new_cache_size);
                    last_cache_size = 0;
                }
                else
                {
                    restore_cache_buf();
                }
            }
#else
            else
            {
                if (head->compress)
                {
                    GUI_ASSERT(new_cache_size < CACHE_BUF_SIZE);
                    return;
                }
            }
#endif

            if (head->compress)
            {
                uint32_t s = os_lock();
                ret = memcpy_by_imdc(&old_rect, &source);
                os_unlock(s);
            }
            else
            {
                uint32_t s = os_lock();
                ret = memcpy_by_dma(&old_rect, &source);
                os_unlock(s);
            }
            ppe_rect.y -= section_y1;
            if (ret)
            {
                ppe_get_identity(&pre_trans);
                pre_trans.m[0][2] = old_rect.x * -1.0f;
                pre_trans.m[1][2] = old_rect.y * -1.0f;
                ppe_mat_multiply(&pre_trans, &inverse);
                y_ref = section_y1;
                ppe_translate(0, y_ref, &pre_trans);
                memcpy(&inverse, &pre_trans, sizeof(float) * 9);
                source.width = old_rect.w;
                source.height = old_rect.h;
            }
            else
            {
                last_cache_size = 0;
                restore_cache_buf();
                if (head->compress)
                {
                    return;
                }
                if (dc->type == DC_RAMLESS)
                {
                    x_ref = 0;
                    y_ref = section_y1;
                }
                ppe_translate(x_ref, y_ref, &inverse);
            }
        }
        else
        {
            if (rect != NULL)
            {
                ppe_get_identity(&pre_trans);
                pre_trans.m[0][2] = rect->x1 * -1.0f;
                pre_trans.m[1][2] = rect->y1 * -1.0f;
                ppe_mat_multiply(&pre_trans, &inverse);
                source.width = rect->x2 - rect->x1 + 1;
                source.height = rect->y2 - rect->y1 + 1;
            }
            ppe_translate(0, dc->section.y1, &inverse);
            gui_rect_file_head_t *rect_header = (gui_rect_file_head_t *)image->data;
            gui_color_t color = {.color.argb_full = rect_header->color.color.argb_full};

            uint8_t tmp = color.color.rgba.b;
            color.color.rgba.b = color.color.rgba.r;
            color.color.rgba.r = tmp;

            color.color.rgba.a = rect_header->color.color.rgba.a * (image->opacity_value * 1.0f / 255);
            source.const_color = color.color.argb_full;
            ppe_rect.y -= dc->section.y1;
        }
        source.high_quality = image->high_quality;
        if ((image->matrix.m[0][0] == 1 && image->matrix.m[1][1] == 1 && \
             image->matrix.m[2][2] == 1 && image->matrix.m[0][1] == 0 && \
             image->matrix.m[1][0] == 0 && image->matrix.m[2][0] == 0 && \
             image->matrix.m[2][1] == 0))
        {
            source.high_quality = false;
        }
        PPEV2_Finish();
        PPEV2_err err = PPEV2_Blit_Inverse(&target, &source, &inverse, &ppe_rect, mode);
        if (err != PPEV2_SUCCESS)
        {
            DBG_DIRECT("PPE err %d", err);
//                sw_acc_blit(image, dc, rect);
        }
    }
    PPEV2_Finish();
}

bool hw_acc_imdc_decode(uint8_t *image, gui_rect_t *rect, uint8_t *output)
{
    if (image == NULL || rect == NULL || output == NULL)
    {
        GUI_ASSERT(image != NULL && rect != NULL && output != NULL);
        return false;
    }
    struct gui_rgb_data_head output_header;
    struct gui_rgb_data_head *head = (struct gui_rgb_data_head *)image;
    memset(&output_header, 0, sizeof(struct gui_rgb_data_head));
    const IMDC_file_header *header = (IMDC_file_header *)((uint32_t)image + sizeof(
                                                              struct gui_rgb_data_head));
    switch (head->type)
    {
    case RGB565:
        output_header.type = PPEV2_RGB565;
        break;
    case RGB888:
        output_header.type = PPEV2_RGB888;
        break;
    case ARGB8888:
        output_header.type = PPEV2_ARGB8888;
        break;
    case ARGB8565:
        output_header.type = PPEV2_ARGB8565;
        break;
    default:
        return false;
    }
    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
    hal_imdc_decompress_info info;
    info.raw_data_address = (uint32_t)header;
    info.start_column = rect->x1;
    info.end_column = rect->x2;
    info.start_line = rect->y1;
    info.end_line = rect->y2;
    output_header.w = info.end_column - info.start_column + 1;
    output_header.h = info.end_line - info.start_line + 1;
    if (hal_imdc_decompress(&info, output + 8))
    {
        memcpy(output, &output_header, sizeof(struct gui_rgb_data_head));
        return true;
    }
    else
    {
        return false;
    }
}

void hw_acc_prepare_cb(draw_img_t *img, gui_rect_t *rect)
{
    if (img->matrix.m[2][2] != 1 || img->matrix.m[0][1] != 0 || \
        img->matrix.m[1][0] != 0 || img->matrix.m[2][0] != 0 || \
        img->matrix.m[2][1] != 0)
    {
        gui_point_t pox = {0.0f};
        float point[4][2];
        float *line = gui_malloc(12 * sizeof(float));
        float x1 = 0;
        float y1 = 0;
        float x2 = 0;
        float y2 = 0;

        if (rect == NULL)
        {
            x1 = 0;
            y1 = 0;
            x2 = img->img_w - 1;
            y2 = img->img_h - 1;
        }
        else
        {
            x1 = _UI_MAX(0, rect->x1);
            y1 = _UI_MAX(0, rect->y1);
            x2 = _UI_MIN(img->img_w - 1, rect->x2);
            y2 = _UI_MIN(img->img_h - 1, rect->y2);
        }

        pox.p[0] = x1;
        pox.p[1] = y1;
        pox.p[2] = 1.0f;
        matrix_multiply_point(&img->matrix, &pox);
        point[0][0] = pox.p[0];
        point[0][1] = pox.p[1];

        pox.p[0] = x2;
        pox.p[1] = y1;
        pox.p[2] = 1.0f;
        matrix_multiply_point(&img->matrix, &pox);
        point[1][0] = pox.p[0];
        point[1][1] = pox.p[1];

        pox.p[0] = x2;
        pox.p[1] = y2;
        pox.p[2] = 1.0f;
        matrix_multiply_point(&img->matrix, &pox);
        point[2][0] = pox.p[0];
        point[2][1] = pox.p[1];

        pox.p[0] = x1;
        pox.p[1] = y2;
        pox.p[2] = 1.0f;
        matrix_multiply_point(&img->matrix, &pox);

        point[3][0] = pox.p[0];
        point[3][1] = pox.p[1];

        if (point[0][0] == point[1][0])
        {
            line[0] = 1;
            line[1] = 0;
            line[2] = -point[0][0];
        }
        else if (point[0][1] == point[1][1])
        {
            line[0] = 0;
            line[1] = 1;
            line[2] = -point[0][1];
        }
        else
        {
            line[1] = -1;
            line[0] = (point[1][1] - point[0][1]) / (point[1][0] - point[0][0]);
            line[2] = point[1][1] - line[0] * point[1][0];
        }

        if (point[0][0] == point[3][0])
        {
            line[3] = 1;
            line[4] = 0;
            line[5] = -point[0][0];
        }
        else if (point[0][1] == point[3][1])
        {
            line[3] = 0;
            line[4] = 1;
            line[5] = -point[0][1];
        }
        else
        {
            line[4] = -1;
            line[3] = (point[3][1] - point[0][1]) / (point[3][0] - point[0][0]);
            line[5] = point[3][1] - line[3] * point[3][0];
        }

        if (point[2][0] == point[1][0])
        {
            line[6] = 1;
            line[7] = 0;
            line[8] = -point[2][0];
        }
        else if (point[2][1] == point[1][1])
        {
            line[6] = 0;
            line[7] = 1;
            line[8] = -point[2][1];
        }
        else
        {
            line[7] = -1;
            line[6] = (point[1][1] - point[2][1]) / (point[1][0] - point[2][0]);
            line[8] = point[1][1] - line[6] * point[1][0];
        }

        if (point[2][0] == point[3][0])
        {
            line[9] = 1;
            line[10] = 0;
            line[11] = -point[2][0];
        }
        else if (point[2][1] == point[3][1])
        {
            line[9] = 0;
            line[10] = 1;
            line[11] = -point[2][1];
        }
        else
        {
            line[10] = -1;
            line[9] = (point[3][1] - point[2][1]) / (point[3][0] - point[2][0]);
            line[11] = point[3][1] - line[9] * point[3][0];
        }
        img->acc_user = line;
    }
}

void hw_acc_end_cb(draw_img_t *image)
{
    if (image->acc_user != NULL)
    {
        gui_free(image->acc_user);
        image->acc_user = NULL;
    }
    return;
}

void hw_acc_init(void)
{
    PPEV2_CLK_ENABLE(ENABLE);
    uint8_t dma_channel_1, dma_channel_2;
    hal_dma_channel_init(&dma_channel_1, &dma_channel_2);
    extern void (* gui_image_acc_prepare_cb)(struct draw_img * image, gui_rect_t *rect);
    extern void (* gui_image_acc_end_cb)(struct draw_img * image);
    gui_image_acc_prepare_cb = hw_acc_prepare_cb;
    gui_image_acc_end_cb = hw_acc_end_cb;
}
