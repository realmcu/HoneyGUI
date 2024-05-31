#include <draw_img.h>
#include <string.h>
#include <math.h>

void (* gui_image_acc_prepare_cb)(struct draw_img *image, gui_rect_t *rect) = NULL;
void (* gui_image_acc_end_cb)(struct draw_img *image) = NULL;

bool gui_image_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                           int16_t *x_start, int16_t *x_end, int16_t *y_start, int16_t *y_end)
{
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;

    int16_t image_w = image->img_target_w ;
    int16_t image_h = image->img_target_h ;

    *x_start = _UI_MAX(image_x, 0);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->fb_width - 1);

    *y_start = _UI_MAX(dc->section.y1, image_y);
    *y_end = _UI_MIN(dc->section.y2, image_y + image_h - 1);

    // if (rect != NULL)
    // {
    //     GUI_ASSERT(rect == NULL);
    //     *x_start = _UI_MAX(*x_start, rect->x1);
    //     *x_end = _UI_MIN(*x_end, rect->x2);
    //     *y_start = _UI_MAX(*y_start, rect->y1);
    //     *y_end = _UI_MIN(*y_end, rect->y2);
    // }
    if ((*x_start >= *x_end) || (*y_start >= *y_end))
    {
        return false;
    }
    return true;
}


gui_rgb_data_head_t gui_image_get_header(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
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
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, img->data, sizeof(head));
    }

    return head;
}
void gui_image_load_scale(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = {0};

    head = gui_image_get_header(img, src_mode);
    img->img_w = head.w;
    img->img_h = head.h;
}
uint32_t gui_image_get_pixel(draw_img_t *img)
{
    struct gui_rgb_data_head *head = img->data;
    if (head->type == RGB565)
    {
        return 2;
    }
    else if (head->type == RGB888)
    {
        return 3;
    }
    else if (head->type == ARGB8565)
    {
        return 3;
    }
    else if (head->type == ARGB8888)
    {
        return 4;
    }
    return 0;
}
uint8_t gui_get_srcBpp(draw_img_t *image, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_rgb_data_head head = gui_image_get_header(image, src_mode);
    uint8_t source_bytes_per_pixel = 0;

    switch (head.type)
    {
    case RGB565:
        source_bytes_per_pixel = 2;
        break;
    case RTKARGB8565:
    case ARGB8565:
    case RGB888:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8888:
    case XRGB8888:
        source_bytes_per_pixel = 4;
        break;
    default:
        break;
    }
    GUI_ASSERT(source_bytes_per_pixel != 0);
    return source_bytes_per_pixel;
}
bool gui_image_new_area(draw_img_t *img, gui_rect_t *rect)
{
    gui_point_t pox = {0.0f};
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

    if (gui_image_acc_prepare_cb != NULL)
    {
        gui_image_acc_prepare_cb(img, rect);
    }

    img->img_target_x = (int16_t)x_min;
    img->img_target_y = (int16_t)y_min;
    img->img_target_w = ceil(x_max) - (int16_t)x_min + 1;
    img->img_target_h = ceil(y_max) - (int16_t)y_min + 1;
    return true;
}

