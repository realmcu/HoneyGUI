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

void *gui_draw_cache_img(draw_img_t *image, IMG_SOURCE_MODE_TYPE src_mode)
{
    uint8_t *img_buff = NULL;
    struct gui_dispdev *dc = gui_get_dc();

    do
    {
        if (dc->type == DC_RAMLESS)
        {
            if (src_mode == IMG_SRC_FILESYS)
            {
                // RAMLESS + MEM ADDRESS, cache img when section 0
                if (dc->section_count != 0)
                {
                    // draw img is not released
                    return img_buff;
                }
            }
            else
            {
                // RAMLESS + MEM ADDRESS, not cache it
                return img_buff;
            }
        }

    }
    while (0);

    // start cache: SINGLE / RAMLESS + FILESYS(sect 0 only)
    {
        uint32_t gpu_width = ((image->img_w + 15) >> 4) << 4;
        uint32_t gpu_height = image->img_h;
        gui_rgb_data_head_t head = gui_image_get_header(image, src_mode);
        uint8_t source_bytes_per_pixel = gui_get_srcBpp(image, src_mode);

        if (src_mode == IMG_SRC_FILESYS)
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
        }
        else if (src_mode == IMG_SRC_MEMADDR)
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
            }
        }
    }
    return img_buff;
}

void gui_draw_free_img(draw_img_t *draw_img, void *img_buff, IMG_SOURCE_MODE_TYPE src_mode)
{
    struct gui_dispdev *dc = gui_get_dc();

    if (dc->type == DC_RAMLESS)
    {
        if (src_mode == IMG_SRC_FILESYS)
        {
            uint32_t section_count = dc->screen_height / dc->fb_height + ((dc->screen_height % dc->fb_height) ?
                                                                          1 : 0);
            // gui_log("%d: 0x%x 0x%x\n", dc->section_count, this->data, img_buff);
            if (dc->section_count == (section_count - 1))
            {
                gui_free(img_buff);
            }
        }
        else if (src_mode == IMG_SRC_MEMADDR)
        {
            // RAMLESS + MEM, not cached

        }

    }
    else if (dc->type == DC_SINGLE)
    {
        if (img_buff)
        {
            gui_free(img_buff);
        }
    }
}


void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    gui_get_acc()->blit(image, dc, rect);
}
