/*
 * File      : gui_iconlist.h
 */
#ifndef __GUI_ICONLIST_H__
#define __GUI_ICONLIST_H__

#include <guidef.h>
#include <gui_fb.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <gui_graphic.h>
/**********************
 *      TYPEDEFS
 **********************/
/*Data of icon*/
#define ICONLIST_THEME_MAXNUM 10
#define SPRING_HEAD 10
typedef enum ICON_LIST_THEME
{
    ICON_LIST_THEME_0,
    ICON_LIST_THEME_1,
    ICON_LIST_THEME_2,
    ICON_LIST_THEME_3,
    ICON_LIST_THEME_4,
    ICON_LIST_THEME_5,
} ICON_LIST_THEME_T;
#include "gui_curtain.h"
typedef gui_curtain_enum_t ICON_LAYOUT_T;
static ICON_LIST_THEME_T icon_list_theme;
typedef struct
{
    ICON_LIST_THEME_T icon_list_theme;
    const char *name;
} gui_iconlist_theme_t;

typedef struct iconlist_config
{
    uint16_t *config;
    uint8_t config_count;
} iconlist_config_t;
#include "gui_img.h"
#include "gui_text.h"
#include "gui_icon.h"



typedef struct gui_iconlist gui_iconlist_t;
struct gui_iconlist
{
    gui_obj_t base;
    uint16_t icon_cnt;
    int16_t dy_static;
    int16_t cur_id;
    bool show_flag;
    iconlist_config_t config;
    void (*set_theme)(char *iconlist_parent_name, ICON_LIST_THEME_T theme);
    void (*set_background)(gui_iconlist_t *this, void *backgroung_src, void *background_highlight_src);
    ICON_LIST_THEME_T(*gui_iocnlist_get_theme)(char *iconlist_parent_name);
    void (*ctor)(gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y, int16_t w, int16_t h, struct gui_iconlist *this);
} ;
void gui_iconlist_ctor(gui_obj_t *parent, const char *filename, int16_t x,
                       int16_t y, int16_t w, int16_t h, gui_iconlist_t *this);







/**
 * @brief create a iconlist widget,which can nest icons.
 * @param parent the father widget nested in.
 * @param filename this iconlist widget's name.
 * @param x the X-axis coordinate of the widget.
 * @param x the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 *
 */
gui_iconlist_t *gui_iconlist_create(void *parent, const char *filename, int16_t x, int16_t y,
                                    int16_t w, int16_t h, uint16_t *config, uint8_t count);


/**
 * @brief set iocnlist theme
 *
 * @param iconlist_parent_name parent of iconlist
 * @param theme
 */
void gui_iocnlist_set_theme(const char *iconlist_parent_name, ICON_LIST_THEME_T theme);
/**
 * @brief get iocnlist theme
 *
 * @param iconlist_parent_name parent of iconlist
 * @return ICON_LIST_THEME_T
 */
ICON_LIST_THEME_T gui_iocnlist_get_theme(char *iconlist_parent_name);

#include "gui_icon_default.h"

#include "gui_icon.h"
#include "gui_iconlist_default.h"
#include "gui_button.h"


#ifdef __cplusplus
}
#endif

#endif


