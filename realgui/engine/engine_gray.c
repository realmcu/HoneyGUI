/**
 * @file engine_gray.c
 * @brief Implementation of functions for handling 16-bit grayscale images
 *        and their interaction with a graphical user interface display device.
 *
 * This file provides the necessary functionality to process and manipulate
 * 16-bit grayscale images, including rendering them on a GUI display device.
 * It is part of the HoneyGUI project.
 *
 * @author Howie Wang
 * @date [Insert Date Here]
 */

#include <string.h>
#include <math.h>
#include "engine_gray.h"

void engine_gray16_blit_to_dc(engine_gray_t *gray16, struct gui_dispdev *dc, gui_rect_t *rect)
{
    (void)rect;
    gui_log("gray16 w = %d, h = %d, data = 0x%x \n", gray16->w, gray16->h, gray16->data);
    // memcpy(dc->frame_buf, gray16->data, gray16->w * gray16->h * dc->bit_depth / 8);

    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;


    x_start = _UI_MAX(gray16->x, dc->section.x1);
    x_end = _UI_MIN(gray16->x + gray16->w - 1, dc->section.x2);

    y_start = _UI_MAX(gray16->y, dc->section.y1);
    y_end = _UI_MIN(gray16->y + gray16->h - 1, dc->section.y2);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t source_data = (uint32_t)(uintptr_t)(gray16->data);
    uint8_t *writebuf = dc->frame_buf;

    int16_t source_w = gray16->w;
    uint32_t len = x_end - x_start + 1;

    gui_matrix_t *inverse = &gray16->inverse;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i + inverse->m[1][2]) * source_w + x_start + inverse->m[0][2];

        uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                           dc->section.x1;
        memcpy(writebuf + des_off / 2, (uint8_t *)(uintptr_t)source_data + source_off / 2, len / 2);
    }

}

void engine_gray256_blit_to_dc(engine_gray_t *gray256, struct gui_dispdev *dc, gui_rect_t *rect)
{
    gui_log("gray256 w = %d, h = %d, data = 0x%x \n", gray256->w, gray256->h, gray256->data);
    // memcpy(dc->frame_buf, gray256->data, gray256->w * gray256->h * dc->bit_depth / 8);

    (void)rect;
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;


    x_start = _UI_MAX(gray256->x, dc->section.x1);
    x_end = _UI_MIN(gray256->x + gray256->w - 1, dc->section.x2);

    y_start = _UI_MAX(gray256->y, dc->section.y1);
    y_end = _UI_MIN(gray256->y + gray256->h - 1, dc->section.y2);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t source_data = (uint32_t)(uintptr_t)(gray256->data);
    uint8_t *writebuf = dc->frame_buf;

    int16_t source_w = gray256->w;
    uint32_t len = x_end - x_start + 1;

    gui_matrix_t *inverse = &gray256->inverse;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i + inverse->m[1][2]) * source_w + x_start + inverse->m[0][2];

        uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                           dc->section.x1;
        memcpy(writebuf + des_off, (uint8_t *)(uintptr_t)source_data + source_off, len);
    }
}

