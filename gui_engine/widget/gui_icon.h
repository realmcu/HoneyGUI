/*
 * File      : gui_icon.h
 */
#ifndef __GUI_ICON_H__
#define __GUI_ICON_H__




#ifdef __cplusplus
extern "C" {
#endif
#include <guidef.h>
#include "gui_graphic.h"
#include "gui_img.h"
typedef struct gui_icon gui_icon_t;
struct gui_icon
{
    gui_obj_t base;
    gui_img_t *back_img;
    gui_img_t *back_img2;
    int16_t id;
    bool ifon;
    int16_t icon_image_offset;
    int16_t icon_text_offset;
    void (* onClick)(gui_icon_t *this, void (*cb)(void *), void *p);
    void (* onPress)(gui_icon_t *this, void (*cb)(void *), void *p);
    void (*ctor)(struct gui_icon *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h, int16_t id);
};
void gui_icon_ctor(gui_icon_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                   int16_t y, int16_t w, int16_t h, int16_t id);
/**
 * @brief create a icon widget,which can nest images,texts,and so on.
 * @note icon widgets can be nested in a icon list widget or not.if nested in a icon list,the idx and idy parameters will be used.
 * @param parent the father widget nested in.
 * @param filename this iconlist widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param id the index in a icon list.
 * @return return the widget object pointer.
 *
 */
gui_icon_t *gui_icon_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h, int16_t id);
#ifdef __cplusplus
}
#endif

#endif
