/*
 * File      : gui_stb_img.h
 */
#ifndef __GUI_STB_IMG_H__
#define __GUI_STB_IMG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <gui_img.h>


typedef struct gui_gif_info
{
    uint32_t init_time_ms;
    uint32_t *delay_ms;
    uint32_t cycle_index;
    uint16_t delay_ms_sum;
    uint8_t total_frame;
    uint8_t current_frame;
} gui_gif_info_t;

typedef struct gui_stb_img
{
    gui_img_t base;
    bool src_changed;
    void *buffer;
    gui_gif_info_t *gif_info;
} gui_stb_img_t;
/**
 * @brief Creat a image widget with buffer.
 * @note This widget is use to display image which needs to decode.
 * @note The data of image can be static char or data transformed by BLE.
 * @param parent The father widget which the scroll text nested in.
 * @param name The widget's name.
 * @param addr The data address of image.
 * @param size The data size of image.The unit is bytes.
 * @param type The type of image. jpeg 7, png 9, bmp 10
 * @param x The X-axis coordinate of the text box.
 * @param x The Y-axis coordinate of the text box.
 * @return gui_stb_img_t*
 */
gui_stb_img_t *gui_stbimg_create_from_mem(void *parent,  const char *name, void *addr,
                                          uint32_t size, uint8_t type, int16_t x, int16_t y);

void gui_stbimg_from_mem_ctor(gui_stb_img_t *this, gui_obj_t *parent, const char *name,
                              void *addr,
                              uint32_t size, uint8_t type, int16_t x, int16_t y);

void gui_stbimg_set_attribute(gui_stb_img_t *this, void *addr, uint32_t size,
                              uint8_t type, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif

