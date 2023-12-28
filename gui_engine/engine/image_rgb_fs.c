#include <draw_img.h>
#include <stdio.h>

static bool rtgui_image_rgb_check(gui_img_t *image);
static bool rtgui_image_rgb_load(gui_img_t *image,
                                 bool load);
static void rtgui_image_rgb_unload(gui_img_t *image);
static void rtgui_image_rgb_blit(gui_img_t *image, struct gui_dispdev *dc,
                                 rtgui_rect_t *rect);
//static int fd;

struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char rsvd : 4;
    unsigned char compress : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;


};


struct rtgui_image_engine rtgui_image_rgb_engine =
{
    "bin",
    { NULL },
    rtgui_image_rgb_check,
    rtgui_image_rgb_load,
    rtgui_image_rgb_unload,
    rtgui_image_rgb_blit,
};

static bool rtgui_image_rgb_check(gui_img_t *image)
{
    return true;
}

static bool rtgui_image_rgb_load(gui_img_t *image,
                                 bool load)
{
    struct gui_rgb_data_head head;

    GUI_ASSERT(image != NULL);
    int fd;
    fd = gui_fs_open(image->filename, 0);
    if (fd < 0)
    {
        gui_log("create filerw failed! line = %d \n", __LINE__);
        return false;
    }
    GUI_ASSERT(fd >= 0);

    /* move to the begining of file */
    gui_fs_lseek(fd, 0, SEEK_SET);
    if (gui_fs_read(fd, &head, sizeof(head)) == sizeof(head))
    {
        //uint8_t *buf = (uint8_t *)(&head);
        //for (size_t i = 0; i < sizeof(head); i++)
        //{
        //   gui_log("%d ", *(buf + i));
        //}

        //gui_log("\nwidth = %d \n", head.w);
        //gui_log("height = %d \n", head.h);
    }

    /* move to the begining of file */
    gui_fs_lseek(fd, 0, SEEK_SET);

    /* set image information */

    GET_BASE(image)->w = head.w;
    GET_BASE(image)->h = head.h;
    image->engine = &rtgui_image_rgb_engine;
    image->fd = fd;
    image->color_format = (gui_color_format)(head.type);


    return true;
}




static void rtgui_image_rgb_unload(gui_img_t *image)
{
    if (image != NULL)
    {
        gui_fs_close(image->fd);
    }
}

static void pixel_convert_rgb565_2_rgb565(int fd, int x_start, int x_end, struct gui_dispdev *dc,
                                          int write_off, uint8_t source_bytes_per_pixel, uint8_t dc_bytes_per_pixel)
{
    uint16_t pixel;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint16_t bitfile_line[dc->fb_width];
    gui_fs_read(fd, bitfile_line, (x_end - x_start) * source_bytes_per_pixel);
    for (uint32_t j = x_start; j < x_end; j++)
    {
        pixel = *((uint16_t *)bitfile_line + j - x_start);
        if (pixel  != 0)
        {
            writebuf[write_off + j] = (pixel);
        }
    }
}

static void pixel_convert_rgb565_2_argb8888(int fd, int x_start, int x_end, struct gui_dispdev *dc,
                                            int write_off, uint8_t source_bytes_per_pixel, uint8_t dc_bytes_per_pixel)
{
    uint16_t pixel;
    uint8_t *writebuf = dc->frame_buf;
    uint8_t bitfile_line[dc->fb_width * 2];
    gui_fs_read(fd, bitfile_line, (x_end - x_start) * source_bytes_per_pixel);
    for (uint32_t j = x_start; j < x_end; j++)
    {
        pixel = *((uint16_t *)bitfile_line + j - x_start);
        pixel = GUI_SWAP16(pixel);
        if (pixel  != 0)
        {
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
            writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
        }
    }
}

static void pixel_convert_rgb888_2_argb8888(int fd, int x_start, int x_end, struct gui_dispdev *dc,
                                            int write_off, uint8_t source_bytes_per_pixel, uint8_t dc_bytes_per_pixel)
{
    uint8_t *pixel;
    uint8_t *writebuf = dc->frame_buf;
    uint8_t bitfile_line[dc->fb_width * 3];
    gui_fs_read(fd, bitfile_line, (x_end - x_start) * source_bytes_per_pixel);
    for (uint32_t j = x_start; j < x_end; j++)
    {
        pixel = bitfile_line + (j - x_start) * source_bytes_per_pixel;
        if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
        {
            writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//R
            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //B
        }
    }
}

static void rtgui_image_rgb_blit(gui_img_t *image, struct gui_dispdev *dc,
                                 rtgui_rect_t *rect)
{

    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = GET_BASE(image)->w;
    int image_h = GET_BASE(image)->h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    int fd = image->fd;
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint8_t source_bytes_per_pixel;
    switch (image->color_format)
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
        source_bytes_per_pixel = 0;
        break;
    }
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int line = i - image_y;
        int write_off = (i - dc->section.y1) * dc->fb_width ;
        int read_y_off = (line * image_w) * source_bytes_per_pixel;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel;

        gui_fs_lseek(fd, read_y_off + read_x_off + sizeof(struct gui_rgb_data_head), SEEK_SET);

        if ((source_bytes_per_pixel == 2) && (dc_bytes_per_pixel == 2))
        {
            pixel_convert_rgb565_2_rgb565(fd, x_start, x_end, dc, write_off, source_bytes_per_pixel,
                                          dc_bytes_per_pixel);
        }
        else if ((source_bytes_per_pixel == 2) && (dc_bytes_per_pixel == 4))
        {
            pixel_convert_rgb565_2_argb8888(fd, x_start, x_end, dc, write_off, source_bytes_per_pixel,
                                            dc_bytes_per_pixel);
        }
        else if ((source_bytes_per_pixel == 3) && (dc_bytes_per_pixel == 4))
        {
            pixel_convert_rgb888_2_argb8888(fd, x_start, x_end, dc, write_off, source_bytes_per_pixel,
                                            dc_bytes_per_pixel);
        }
    }

}





void rtgui_image_rgb_init(void)
{
    /* register rgb565 on image system */
    rtgui_image_register_engine(&rtgui_image_rgb_engine);
}

