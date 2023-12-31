#include <draw_img.h>
#include <string.h>

void gui_image_load_scale(draw_img_t *img)
{
    struct gui_rgb_data_head head = {0};

    if (img->src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(img->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", img->data);
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

bool gui_image_new_area(draw_img_t *img)
{

    memcpy(img->inverse, img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(img->inverse);

    struct gui_pox pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];


    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
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


    pox.p[0] = 0.0f;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
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

    pox.p[0] = (float)img->img_w - 1;
    pox.p[1] = (float)img->img_h - 1;
    pox.p[2] = 1.0f;
    pox_mul(img->matrix, &pox);
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

    img->img_x = (int16_t)x_min;
    img->img_y = (int16_t)y_min;
    img->target_w = (int16_t)x_max - (int16_t)x_min;
    img->target_h = (int16_t)y_max - (int16_t)y_min;
    return true;
}
