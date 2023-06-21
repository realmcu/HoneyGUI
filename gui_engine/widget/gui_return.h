/*
 * File      : gui_curtain.h
 */
#ifndef __GUI_RETURN_H__
#define __GUI_RETURN_H__

#include <guidef.h>
#include <gui_img.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
typedef struct gui_return gui_return_t;
struct gui_return
{
    gui_obj_t base;
    gui_img_t *return_img_left;
    gui_img_t *return_img_right;
    void (*onReturn)(void *func);
    bool hit;
    void (*ctor)(gui_return_t *this, gui_obj_t *parent, const char *widgetame);
};

/**
 * @brief create a return widget.
 * @param parent the father widget it nested in.
 * @return return the widget object pointer.
 *
 */
gui_return_t *gui_return_create(void *parent);


#ifdef __cplusplus
}
#endif

#endif



