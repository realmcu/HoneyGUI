#include <draw_img.h>
#include <stdio.h>
#include "acc_engine.h"
#include "acc_sw_rle.h"
#include "acc_init.h"


extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
extern void hw_acc_init(void);

static struct acc_engine acc = {0};

void gui_acc_init(void)
{
#ifdef RTK_MODULE_VG_LITE
    acc.blit = hw_acc_blit;
#elif defined RTK_MODULE_RTK_PPE
    hw_acc_init();
    acc.blit = hw_acc_blit;
#elif defined RTK_MODULE_RTK_PPEV2
    hw_acc_init();
    acc.blit = hw_acc_blit;
#else
    acc.blit = sw_acc_blit;
#endif
}

struct acc_engine *gui_get_acc(void)
{
    return &acc;
}

imdc_file_header_t rtgui_image_get_imdc_header(draw_img_t *img)
{
    imdc_file_header_t head = {0};

    if (img->src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(img->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)img->data);
        }
        gui_fs_lseek(fd, sizeof(struct gui_rgb_data_head), SEEK_SET);
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
    }
    else if (img->src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, (void *)((uint8_t *)(img->data) + sizeof(struct gui_rgb_data_head)), sizeof(head));
    }

    return head;
}

static void *gui_load_imgfile(draw_img_t *draw_img)
{
    const char *path = draw_img->data;
    uint8_t *data = NULL;

    int fd = gui_fs_open(path,  0);
    if (fd <= 0)
    {
        gui_log("open file fail:%s!\n", path);
        return NULL;
    }

    uint32_t size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
    data = (uint8_t *)gui_malloc(size);
    GUI_ASSERT(data != NULL);
    memset(data, 0, size);
    gui_fs_read(fd, data, size);
    gui_fs_close(fd);

    return data;
}

static void gui_load_imgfile_align(draw_img_t *draw_img, uint8_t *data,
                                   uint8_t source_bytes_per_pixel)
{
    const char *path = draw_img->data;
    uint32_t gpu_width = ((draw_img->img_w + 15) >> 4) << 4;
    uint32_t gpu_height = draw_img->img_h;

    int fd = gui_fs_open(path,  0);
    if (fd <= 0)
    {
        gui_log("open file fail:%s!\n", path);
        return;
    }
    gui_fs_read(fd, data, sizeof(struct gui_rgb_data_head));


    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)data;
    for (uint32_t i = 0; i < gpu_height; i++)
    {
        gui_fs_lseek(fd, sizeof(struct gui_rgb_data_head) + i * draw_img->img_w * source_bytes_per_pixel,
                     SEEK_SET);
        gui_fs_read(fd, (void *)(image_off + i * gpu_width * source_bytes_per_pixel),
                    draw_img->img_w * source_bytes_per_pixel);
    }
    gui_fs_close(fd);

    draw_img->data = data;
}

static uint8_t *gui_malloc_align_img(uint8_t *offset, uint32_t size)
{
    uint8_t *img_buff = (uint8_t *)gui_malloc(size + 63);
    GUI_ASSERT(img_buff != NULL);
    // align data address by 64
    uint8_t *data = (uint8_t *)(((((uint32_t)img_buff + 63) >> 6) << 6) - sizeof(
                                    struct gui_rgb_data_head));
    if (data < img_buff)
    {
        data = (uint8_t *)(data + 64);
    }
    *offset = data - img_buff;
    return img_buff;
}

static void gui_release_imgfile_acc(draw_img_t *draw_img, const void *img_info, void *img_buff)
{
    gui_free(img_buff);
    draw_img->data = (void *)img_info;
}

static uint8_t gui_get_srcbpp(draw_img_t *image)
{
    struct gui_rgb_data_head head = rtgui_image_get_header(image);
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


void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    const void *img_info = image->data;
    uint8_t *img_buff = NULL;
    bool flg_cache = false;

    if (gui_get_dc()->type == DC_SINGLE)
    {
        uint32_t gpu_width = ((image->img_w + 15) >> 4) << 4;
        uint32_t gpu_height = image->img_h;
        gui_rgb_data_head_t head = rtgui_image_get_header(image);
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

        if (image->src_mode == IMG_SRC_FILESYS)
        {
            uint32_t size = gpu_width * gpu_height * source_bytes_per_pixel;
            uint8_t *data = NULL;
            uint8_t offset = 0; // header address offset after image data address alignment

            img_buff = gui_malloc_align_img(&offset, size);
            data = img_buff + offset;
            if (head.compress)
            {
                // gui_log("IMG_SRC_FILESYS IMDC_COMPRESS %s\n", img_info);
                uint8_t *src_buff = (uint8_t *)gui_load_imgfile(image);
                image->data = src_buff;
                sw_acc_rle_uncompress(image, data);
                gui_free(src_buff);
                image->data = data;
            }
            else
            {
                gui_load_imgfile_align(image, data, source_bytes_per_pixel);
            }
            flg_cache = true;
        }
        else if (image->src_mode == IMG_SRC_MEMADDR)
        {
            uint8_t *data = (uint8_t *)(sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data));
#ifdef __WIN32
            // PC simulator: data alignment is unnecessary.
            if (head.compress)
#else
            if (gpu_width != image->img_w || (int)data % 64 != 0 || head.compress)
#endif
            {
                uint32_t size = gpu_width * gpu_height * source_bytes_per_pixel;
                uint8_t offset = 0;

                img_buff = gui_malloc_align_img(&offset, size);
                data = img_buff + offset;
#ifndef __WIN32
                if (head.compress)
#endif
                {
                    // gui_log("IMG_SRC_MEMADDR IMDC_COMPRESS\n");
                    sw_acc_rle_uncompress(image, data);
                    image->data = data;
                }
#ifndef __WIN32
                else
                {
                    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
                    for (uint32_t i = 0; i < gpu_height; i++)
                    {
                        memcpy((void *)((uint32_t)data + i * gpu_width * source_bytes_per_pixel),
                               (void *)(image_off + i * image->img_w * source_bytes_per_pixel),
                               image->img_w * source_bytes_per_pixel);
                    }
                }
#endif
                flg_cache = true;
            }
        }
    }

    acc.blit(image, dc, rect);

    if (flg_cache)
    {
        gui_release_imgfile_acc(image, img_info, img_buff);
    }
}
