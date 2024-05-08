#include <draw_img.h>
#include <string.h>
#include <math.h>

void (* gui_image_acc_prepare_cb)(struct draw_img *image) = NULL;
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

    if (rect != NULL)
    {
        GUI_ASSERT(rect == NULL);
        *x_start = _UI_MAX(*x_start, rect->x1);
        *x_end = _UI_MIN(*x_end, rect->x2);
        *y_start = _UI_MAX(*y_start, rect->y1);
        *y_end = _UI_MIN(*y_end, rect->y2);
    }
    if ((*x_start >= *x_end) || (*y_start >= *y_end))
    {
        return false;
    }
    return true;
}
void gui_image_load_scale(draw_img_t *img)
{
    struct gui_rgb_data_head head = {0};

    if (img->src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(img->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)img->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
    }
    else if (img->src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, img->data, sizeof(head));
    }

    img->img_w = head.w;
    img->img_h = head.h;
}

gui_rgb_data_head_t gui_image_get_header(draw_img_t *img)
{
    struct gui_rgb_data_head head = {0};

    if (img->src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(img->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)img->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
    }
    else if (img->src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, img->data, sizeof(head));
    }

    return head;
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
    else if (head->type == RGBA8888)
    {
        return 4;
    }
    return 0;
}
bool gui_image_new_area(draw_img_t *img, gui_rect_t *rect)
{
    float point[4][2];
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
    point[0][0] = pox.p[0];
    point[0][1] = pox.p[1];

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
    point[1][0] = pox.p[0];
    point[1][1] = pox.p[1];

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
    point[2][0] = pox.p[0];
    point[2][1] = pox.p[1];

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
    point[3][0] = pox.p[0];
    point[3][1] = pox.p[1];

    if (img->matrix.m[2][2] != 1 || img->matrix.m[0][1] != 0 || \
        img->matrix.m[1][0] != 0 || img->matrix.m[2][0] != 0 || \
        img->matrix.m[2][1] != 0)
    {
        img->line = gui_malloc(12 * sizeof(float));
        if (point[0][0] == point[1][0])
        {
            img->line[0] = 1;
            img->line[1] = 0;
            img->line[2] = -point[0][0];
        }
        else if (point[0][1] == point[1][1])
        {
            img->line[0] = 0;
            img->line[1] = 1;
            img->line[2] = -point[0][1];
        }
        else
        {
            img->line[1] = -1;
            img->line[0] = (point[1][1] - point[0][1]) / (point[1][0] - point[0][0]);
            img->line[2] = point[1][1] - img->line[0] * point[1][0];
        }

        if (point[0][0] == point[3][0])
        {
            img->line[3] = 1;
            img->line[4] = 0;
            img->line[5] = -point[0][0];
        }
        else if (point[0][1] == point[3][1])
        {
            img->line[3] = 0;
            img->line[4] = 1;
            img->line[5] = -point[0][1];
        }
        else
        {
            img->line[4] = -1;
            img->line[3] = (point[3][1] - point[0][1]) / (point[3][0] - point[0][0]);
            img->line[5] = point[3][1] - img->line[3] * point[3][0];
        }

        if (point[2][0] == point[1][0])
        {
            img->line[6] = 1;
            img->line[7] = 0;
            img->line[8] = -point[2][0];
        }
        else if (point[2][1] == point[1][1])
        {
            img->line[6] = 0;
            img->line[7] = 1;
            img->line[8] = -point[2][1];
        }
        else
        {
            img->line[7] = -1;
            img->line[6] = (point[1][1] - point[2][1]) / (point[1][0] - point[2][0]);
            img->line[8] = point[1][1] - img->line[6] * point[1][0];
        }

        if (point[2][0] == point[3][0])
        {
            img->line[9] = 1;
            img->line[10] = 0;
            img->line[11] = -point[2][0];
        }
        else if (point[2][1] == point[3][1])
        {
            img->line[9] = 0;
            img->line[10] = 1;
            img->line[11] = -point[2][1];
        }
        else
        {
            img->line[10] = -1;
            img->line[9] = (point[3][1] - point[2][1]) / (point[3][0] - point[2][0]);
            img->line[11] = point[3][1] - img->line[9] * point[3][0];
        }
    }
    if (gui_image_acc_prepare_cb != NULL)
    {
        gui_image_acc_prepare_cb(img);
    }

    img->img_target_x = (int16_t)x_min;
    img->img_target_y = (int16_t)y_min;
    img->img_target_w = ceil(x_max) - (int16_t)x_min + 1;
    img->img_target_h = ceil(y_max) - (int16_t)y_min + 1;
    return true;
}

