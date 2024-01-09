#include <draw_img.h>
#include <stdio.h>
//#include <gui_matrix.h>
#include "acc_engine.h"


extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
extern void hw_acc_init(void);

static struct acc_engine acc = {0};

void gui_acc_init(void)
{
#ifdef RTK_MODULE_VG_LITE
    acc.blit = hw_acc_blit;
#elif defined (RTK_MODULE_RTK_PPE) || defined(RTK_MODULE_RTK_PPEV2)
    acc.blit = hw_acc_blit;
#ifdef RTK_MODULE_RTK_PPEV2
    hw_acc_init();
#endif
#else
    acc.blit = sw_acc_blit;
#endif
}

struct acc_engine *gui_get_acc(void)
{
    return &acc;
}

void gui_load_imgfile_acc(draw_img_t *draw_img, void *data, uint8_t source_bytes_per_pixel)
{
    const char *path = draw_img->data;
    uint32_t gpu_width = ((draw_img->img_w + 15) >> 4) << 4;
    uint32_t gpu_height = draw_img->img_h;

    int fd = gui_fs_open(path,  0);
    if (fd <= 0)
    {
        gui_log("open file fail:%s!\n", path);
        return ;
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

static void gui_release_imgfile_acc(draw_img_t *draw_img, const void *img_info, void *img_buff)
{
    gui_free(img_buff);
    draw_img->data = (void *)img_info;
}


void gui_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    const void *img_info = image->data;
    void *img_buff = NULL;
    bool flg_cache = false;

    if (gui_get_dc()->type == DC_SINGLE)
    {
        uint32_t gpu_width = ((image->img_w + 15) >> 4) << 4;
        uint32_t gpu_height = image->img_h;
        struct gui_rgb_data_head head = rtgui_image_get_head(image);
        uint8_t source_bytes_per_pixel = 0;

        switch (head.type)
        {
        case RGB565:
            source_bytes_per_pixel = 2;
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

        if (image->src_mode == IMG_SRC_FILESYS)
        {
            uint32_t size = gpu_width * gpu_height * source_bytes_per_pixel;
            void *data = NULL;

            img_buff = gui_malloc(size + 63);
            GUI_ASSERT(img_buff != NULL);
            data = (void *)(((((uint32_t)img_buff + 63) >> 6) << 6) - sizeof(struct gui_rgb_data_head));
            if (data < img_buff)
            {
                data = (void *)((uint32_t)data + 64);
            }

            gui_load_imgfile_acc(image, data, source_bytes_per_pixel);
            flg_cache = true;
        }
        else if (image->src_mode == IMG_SRC_MEMADDR)
        {
            void *data = (uint8_t *)(sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data));
            if (gpu_width != image->img_w || (int)data % 64 != 0)
            {
                uint32_t size = gpu_width * gpu_height * source_bytes_per_pixel;

                img_buff = gui_malloc(size + 63);
                GUI_ASSERT(img_buff != NULL);
                data = (void *)(((((uint32_t)img_buff + 63) >> 6) << 6) - sizeof(struct gui_rgb_data_head));
                if (data < img_buff)
                {
                    data = (void *)((uint32_t)data + 64);
                }

                uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
                for (uint32_t i = 0; i < gpu_height; i++)
                {
                    memcpy((void *)((uint32_t)data + i * gpu_width * source_bytes_per_pixel),
                           (void *)(image_off + i * image->img_w * source_bytes_per_pixel),
                           image->img_w * source_bytes_per_pixel);
                }
                flg_cache = true;
            }
        }
        else if (image->src_mode == IMG_SRC_RLE)
        {
            // TODO
        }
    }

    acc.blit(image, dc, rect);

    if (flg_cache)
    {
        gui_release_imgfile_acc(image, img_info, img_buff);
        flg_cache = false;
    }
}
