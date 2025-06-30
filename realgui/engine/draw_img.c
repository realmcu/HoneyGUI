#include <draw_img.h>
#include <string.h>
#include <math.h>
#include "acc_api.h"

void (* draw_img_acc_prepare_cb)(struct draw_img *image, gui_rect_t *rect) = NULL;
void (* draw_img_acc_end_cb)(struct draw_img *image) = NULL;

bool draw_img_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                          int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end)
{
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;

    int16_t image_w = image->img_target_w ;
    int16_t image_h = image->img_target_h ;

    *x_start = _UI_MAX(image_x, dc->section.x1);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->section.x2);

    *y_start = _UI_MAX(image_y, dc->section.y1);
    *y_end = _UI_MIN(image_y + image_h - 1, dc->section.y2);

    if ((*x_start >= *x_end) || (*y_start >= *y_end))
    {
        return false;
    }
    return true;
}


gui_rgb_data_head_t draw_img_get_header(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};

    if (src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(img->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)img->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
    }
    else if (src_mode == IMG_SRC_FTL)
    {
        uint32_t base = (uint32_t)(uintptr_t)img->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, img->data, sizeof(head));
    }

    return head;
}
void draw_img_load_scale(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};

    head = draw_img_get_header(img, src_mode);
    img->img_w = head.w;
    img->img_h = head.h;
}
uint32_t draw_img_get_pixel_byte(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};
    head = draw_img_get_header(img, src_mode);
    if (head.type == RGB565)
    {
        return 2;
    }
    else if (head.type == RGB888)
    {
        return 3;
    }
    else if (head.type == ARGB8565)
    {
        return 3;
    }
    else if (head.type == ARGB8888)
    {
        return 4;
    }
    return 0;
}

bool draw_img_new_area(draw_img_t *img, gui_rect_t *rect)
{
    gui_point3f_t pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

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
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = x2;
    pox.p[1] = y1;
    pox.p[2] = 1.0f;
    matrix_multiply_point(&img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = x2;
    pox.p[1] = y2;
    pox.p[2] = 1.0f;
    matrix_multiply_point(&img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = x1;
    pox.p[1] = y2;
    pox.p[2] = 1.0f;
    matrix_multiply_point(&img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    if (draw_img_acc_prepare_cb != NULL)
    {
        draw_img_acc_prepare_cb(img, rect);
    }

    img->img_target_x = (int16_t)x_min;
    img->img_target_y = (int16_t)y_min;
    img->img_target_w = ceilf(x_max) - (int16_t)x_min + 1;
    img->img_target_h = ceilf(y_max) - (int16_t)y_min + 1;
    return true;
}


void draw_img_cache(draw_img_t *image, IMG_SOURCE_MODE_TYPE src_mode)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (dc->section_count != 0)
    {
        return;
    }
    if (src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open((const char *)image->data,  0);
        uint32_t size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
        uint8_t *data = (uint8_t *)gui_malloc(size);
        GUI_ASSERT(data != NULL);
        gui_fs_read(fd, data, size);
        gui_fs_close(fd);
        image->data = data;

        return;
    }
    else if (src_mode == IMG_SRC_FTL)
    {
        gui_rgb_data_head_t head;
        uint32_t base = (uint32_t)(uintptr_t)image->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
        uint8_t *data = NULL;
        if (head.compress == true)
        {
            uint32_t end = 0;
            gui_ftl_read(base + sizeof(gui_rgb_data_head_t) + sizeof(imdc_file_header_t) + 4 * (head.h),
                         (uint8_t *)&end, 4);
            uint32_t size = end  + sizeof(gui_rgb_data_head_t);
            data = (uint8_t *)gui_malloc(size);
            GUI_ASSERT(data != NULL);
            gui_ftl_read(base, data, size);
        }
        else
        {
            draw_img_get_header(image, src_mode);
            uint8_t pixel_byte = draw_img_get_pixel_byte(image, src_mode);
            uint32_t size = head.w * head.h * pixel_byte;
            data = (uint8_t *)gui_malloc(size);
            GUI_ASSERT(data != NULL);
            gui_ftl_read(base, data, size);
        }
        image->data = data;

        return;
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
        gui_jpeg_file_head_t *jpeg = image->data;

        if (head->type == JPEG)
        {
            int w = 0;
            int h = 0;
            int channel = 0;
            image->data = gui_acc_jpeg_load(jpeg->jpeg, jpeg->size, &w, &h, &channel);
            image->img_w = w;
            image->img_h = h;
            gui_log("JPEG image caching is not supported.\n");
            return;
        }
        return;
    }

}

void draw_img_free(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    gui_dispdev_t *dc = gui_get_dc();
    if (dc->section_count != dc->section_total - 1)
    {
        return;
    }
    if ((src_mode == IMG_SRC_FILESYS) || (src_mode == IMG_SRC_FTL))
    {
        gui_free(img->data);
    }
    if (src_mode == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img->data;
        if (head->jpeg)
        {
            gui_acc_jpeg_free(img->data);
        }
    }
}

