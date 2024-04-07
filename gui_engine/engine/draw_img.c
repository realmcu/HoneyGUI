#include <draw_img.h>
#include <string.h>
#include <math.h>

bool gui_image_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                           int16_t *x_start, int16_t *x_end, int16_t *y_start, int16_t *y_end)
{
    int16_t image_x = rect->x1;
    int16_t image_y = rect->y1;

    int16_t image_w = image->target_w ;
    int16_t image_h = image->target_h ;

    *x_start = _UI_MAX(_UI_MAX(image_x, image_x + rect->xboundleft), 0);
    *x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    *x_end = _UI_MIN(*x_end, rect->x2);

    if (rect->xboundright > 0)
    {
        *x_end = _UI_MIN(_UI_MIN(image_x + image_w, image_x + rect->xboundright), dc->fb_width);
    }

    *y_start = _UI_MAX(_UI_MAX(dc->section.y1, image_y), image_y + rect->yboundtop);
    *y_end = _UI_MIN(dc->section.y2, image_y + image_h);
    *y_end = _UI_MIN(*y_end, rect->y2);
    if (rect->yboundbottom > 0)
    {
        *y_end = _UI_MIN(*y_end, image_y + rect->yboundbottom);
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

struct gui_rgb_data_head rtgui_image_get_header(draw_img_t *img)
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

bool gui_image_new_area(draw_img_t *img)
{
    float point[4][2];
    struct gui_point pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    matrix_multiply_point(img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];
    point[0][0] = pox.p[0];
    point[0][1] = pox.p[1];

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    matrix_multiply_point(img->matrix, &pox);
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

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    matrix_multiply_point(img->matrix, &pox);
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

    pox.p[0] = 0;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    matrix_multiply_point(img->matrix, &pox);
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

    if (img->matrix->m[2][2] != 1 || img->matrix->m[0][1] != 0 || \
        img->matrix->m[1][0] != 0 || img->matrix->m[2][0] != 0 || \
        img->matrix->m[2][1] != 0)
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

    img->img_x = (int16_t)x_min;
    img->img_y = (int16_t)y_min;
    img->target_w = ceil(x_max) - (int16_t)x_min + 1;
    img->target_h = ceil(y_max) - (int16_t)y_min + 1;
    return true;
}
