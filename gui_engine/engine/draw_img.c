#include <draw_img.h>
#include <string.h>
#include <math.h>

bool gui_image_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                           int16_t *x_start, int16_t *x_end, int16_t *y_start, int16_t *y_end)
{
    int16_t image_x = image->img_x;
    int16_t image_y = image->img_y;

    int16_t image_w = image->target_w ;
    int16_t image_h = image->target_h ;

    *x_start = _UI_MAX(image_x, 0);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->fb_width - 1);

    *y_start = _UI_MAX(dc->section.y1, image_y);
    *y_end = _UI_MIN(dc->section.y2, image_y + image_h - 1);

    if (rect != NULL)
    {

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

struct gui_rgb_data_head gui_image_get_header(draw_img_t *img)
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
    matrix_multiply_point(&img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];
    point[0][0] = pox.p[0];
    point[0][1] = pox.p[1];

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = 0.0f;
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

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = (float)img->img_h - 1;
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

    pox.p[0] = 0;
    pox.p[1] = (float)img->img_h - 1;
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

    img->img_x = (int16_t)x_min;
    img->img_y = (int16_t)y_min;
    img->target_w = ceil(x_max) - (int16_t)x_min + 1;
    img->target_h = ceil(y_max) - (int16_t)y_min + 1;
    return true;
}

static uint8_t gui_get_srcbpp(draw_img_t *image)
{
    struct gui_rgb_data_head head = gui_image_get_header(image);
    uint8_t source_bytes_per_pixel = 0;

    switch (head.type)
    {
    case RGB565:
        source_bytes_per_pixel = 2;
        break;
    case RTKARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case RGB888:
        source_bytes_per_pixel = 3;
        break;
    case RGBA8888:
        source_bytes_per_pixel = 4;
        break;
    default:
        break;
    }
    GUI_ASSERT(source_bytes_per_pixel != 0);
    return source_bytes_per_pixel;
}

static void gui_copy_area(uint8_t *buf_target, uint8_t *buf_src, gui_rect_t *rect,
                          uint16_t line_byte, uint8_t Bpp)
{
    uint16_t write_step = (rect->x2 - rect->x1 + 1) * Bpp;
    uint8_t *pdc_read = buf_src + rect->y1 * line_byte + rect->x1 * Bpp;
    uint8_t *pwrite = buf_target;
    uint16_t read_line = rect->y1;

    while (read_line <= rect->y2)
    {
        memcpy(pwrite, pdc_read, write_step);
        pdc_read += line_byte;
        pwrite += write_step;
        read_line++;
    }
}


void gui_destroy_subImg(draw_img_t *drawImg_sub)
{
    gui_free(drawImg_sub->data);
    gui_free(drawImg_sub);
}

draw_img_t *gui_create_subImg(draw_img_t *drawImg_src, gui_rect_t *rect)
{
    // draw_img_t *drawImg_sub = (draw_img_t *)gui_malloc(sizeof(draw_img_t));
    draw_img_t *drawImg_sub = (draw_img_t *)malloc(sizeof(draw_img_t));
    uint8_t img_Bpp = gui_get_srcbpp(drawImg_src);
    uint16_t rect_w = rect->x2 - rect->x1 + 1;
    uint16_t rect_h = rect->y2 - rect->y1 + 1;
    uint32_t sub_size = sizeof(struct gui_rgb_data_head) + rect_w * rect_h * img_Bpp;
    uint8_t *sub_data = (uint8_t *)gui_malloc(sub_size);

    if (!drawImg_sub || !sub_data)
    {
        if (drawImg_sub)
        {
            gui_free(drawImg_sub);
        }
        if (sub_data)
        {
            gui_free(sub_data);
        }
        return NULL;
    }

    if (drawImg_src->src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(drawImg_sub, drawImg_src, sizeof(draw_img_t));
        drawImg_sub->data = sub_data;
        // gui_log("0x%x 0x%x \n", drawImg_sub->data, drawImg_sub);
        memcpy(sub_data, drawImg_src->data, sizeof(struct gui_rgb_data_head));
        ((struct gui_rgb_data_head *)sub_data)->w = rect_w;
        ((struct gui_rgb_data_head *)sub_data)->h = rect_h;
        drawImg_sub->img_x = 0;
        drawImg_sub->img_y = 0;
        drawImg_sub->img_w = rect_w;
        drawImg_sub->img_h = rect_h;
        drawImg_sub->target_w = rect_w;
        drawImg_sub->target_h = rect_h;

        // no_rle
        gui_copy_area(sub_data + sizeof(struct gui_rgb_data_head), \
                      (uint8_t *)(drawImg_src->data) + sizeof(struct gui_rgb_data_head), \
                      rect, drawImg_src->img_w * img_Bpp, img_Bpp);
    }
    else
    {
        gui_destroy_subImg(drawImg_sub);
        drawImg_sub = NULL;
    }
    return drawImg_sub;
}
