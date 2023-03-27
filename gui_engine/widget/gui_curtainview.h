/*
 * File      : gui_curtainview.h
 */
#ifndef __GUI_CURTAINVIEW_H__
#define __GUI_CURTAINVIEW_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
typedef struct
{
    int32_t x;
    int32_t y;
} gui_curtain_ext_id_t;
typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} gui_curtain_bool_t;
typedef struct
{
    float up;
    float down;
    float left;
    float right;
} gui_curtain_float_t;
typedef enum
{
    CURTAIN_UNDEFINED,
    CURTAIN_UP,
    CURTAIN_DOWN,
    CURTAIN_LEFT,
    CURTAIN_RIGHT,
    CURTAIN_MIDDLE,
} gui_curtain_enum_t;

typedef struct gui_curtainview
{
    gui_obj_t base;
    uint16_t curtain_cnt;
    gui_curtain_enum_t cur_curtain;
    gui_curtain_float_t scopes;
    gui_curtain_bool_t orientations;
    float scopeup;
    float scopedown;
    float scopeleft;
    float scoperight;
    void (*ctor)(struct gui_curtainview *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h);
} gui_curtainview_t;



/**
 * @brief create a curtainview widget,which can nest curtains.
 * @param parent the father widget it nested in.
 * @param filename this curtainview widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @param scope the scope of every curtain.
 * @return return the widget object pointer.
 *
 */
gui_curtainview_t *gui_curtainview_create(void *parent, const char *filename, int16_t x,
                                          int16_t y,
                                          int16_t w, int16_t h);



#ifdef __cplusplus
}
#endif

#endif



