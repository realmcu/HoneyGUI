#include <draw_img.h>
#include <stdio.h>
//#include <gui_matrix.h>
#include "acc_engine.h"


extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, rtgui_rect_t *rect);

extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, rtgui_rect_t *rect);
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

static const char *gui_load_imgfile_acc(draw_img_t *draw_img)
{
    const char *file_path = draw_img->data;
    if (gui_get_dc()->type == DC_SINGLE)
    {
        int fd = gui_fs_open(file_path,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s!\n", file_path);
            return file_path;
        }

        int size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
        // gui_log("img size: %d \n", size);
        draw_img->data = gui_malloc(size);
        GUI_ASSERT(draw_img->data != NULL);
        memset(draw_img->data, 0, size);
        gui_fs_read(fd, draw_img->data, size);
        gui_fs_close(fd);
    }
    else
    {
        draw_img->data = gui_get_file_address(file_path);
    }
    return file_path;
}

static void gui_release_imgfile_acc(draw_img_t *draw_img, const char *path)
{
    if (gui_get_dc()->type == DC_SINGLE)
    {
        gui_free(draw_img->data);
    }
    else
    {

    }
    draw_img->data = (void *)path;
}


void gui_acc_blit(draw_img_t *image, struct gui_dispdev *dc, rtgui_rect_t *rect)
{
    const char *path = NULL;
    if (image->src_mode == IMG_SRC_FILESYS)
    {
        path = gui_load_imgfile_acc(image);
    }

    acc.blit(image, dc, rect);

    if (image->src_mode == IMG_SRC_FILESYS)
    {
        gui_release_imgfile_acc(image, path);
    }

}
