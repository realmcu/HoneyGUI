#include <draw_img.h>
#include <stdio.h>
#include "acc_sw_rle.h"
#include "acc_init.h"


extern void sw_acc_init(void);
extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);






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
    uint16_t gpu_width = ((draw_img->img_w + 15) >> 4) << 4;
    uint16_t gpu_height = draw_img->img_h;

    int fd = gui_fs_open(path,  0);
    if (fd <= 0)
    {
        gui_log("open file fail:%s!\n", path);
        return;
    }
    gui_fs_read(fd, data, sizeof(struct gui_rgb_data_head));


    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(uintptr_t)data;
    for (uint16_t i = 0; i < gpu_height; i++)
    {
        gui_fs_lseek(fd, sizeof(struct gui_rgb_data_head) + i * draw_img->img_w * source_bytes_per_pixel,
                     SEEK_SET);
        gui_fs_read(fd, (void *)(uintptr_t)(image_off + i * gpu_width * source_bytes_per_pixel),
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
    uint8_t *data = (uint8_t *)(((((uint32_t)(uintptr_t)img_buff + 63) >> 6) << 6) - sizeof(
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




void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    const void *img_info = image->data;
    uint8_t *img_buff = NULL;
    bool flg_cache = false;

    if (gui_get_dc()->type == DC_SINGLE)
    {
        uint32_t gpu_width = ((image->img_w + 15) >> 4) << 4;
        uint32_t gpu_height = image->img_h;
        gui_rgb_data_head_t head = gui_image_get_header(image);
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
        case XRGB8888:
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
            uint8_t *data = (uint8_t *)(sizeof(struct gui_rgb_data_head) + (uint32_t)(uintptr_t)(image->data));
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

    gui_get_acc()->blit(image, dc, rect);

    if (flg_cache)
    {
        gui_release_imgfile_acc(image, img_info, img_buff);
    }
}
