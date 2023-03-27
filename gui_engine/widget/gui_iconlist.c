/*
 * File      : gui_iconview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_iconlist.h>
#include <string.h>
#include <gui_server.h>
#include "gui_obj.h"
#define ICONLIST_THEME_MAXNUM 10
#define SPRING_HEAD 10


static gui_iconlist_theme_t iconlist_setting[ICONLIST_THEME_MAXNUM];
void gui_iconlist_ctor(gui_obj_t *parent, const char *filename, int16_t x,
                       int16_t y, int16_t w, int16_t h, gui_iconlist_t *this)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    this->base.type = ICONLIST;
}


/*static void icon_update_att_theme4(gui_obj_t *obj, struct gui_dispdev *dc, struct rtgui_rect *rect,
                                   touch_info_t *tp)
{
    if (obj->parent->type == ICONLIST)
    {
        gui_icon_theme1_t *ext = (gui_icon_theme1_t *)obj;
        gui_obj_t *parent = obj->parent;
        gui_iconlist_theme_4_t *parent_ext = (gui_iconlist_theme_1_t *)parent;
        switch (((gui_iconlist_theme_4_t *)parent)->icon_layout)
        {
        case UP:
        case DOWN:
            obj->x = (ext->base.id * (int)(parent_ext->icon_gap));
            break;
        case LEFT:
        case RIGHT:
            obj->y = (ext->base.id * (int)(parent_ext->icon_gap));
            break;
        default:
            obj->x = (ext->base.id * (int)(parent_ext->icon_gap));
            break;
        }
    }
}*/






void (onClick)(gui_icon_t *this, void (*cb)(void *), void *p)
{
    GUI_ASSERT(NULL != NULL);
}
void (onPress)(gui_icon_t *this, void (*cb)(void *), void *p)
{
    GUI_ASSERT(NULL != NULL);
}


















void gui_iocnlist_set_theme(const char *iconlist_parent_name, ICON_LIST_THEME_T theme)
{
    for (size_t i = 0; i < ICONLIST_THEME_MAXNUM; i++)
    {
        if (iconlist_setting[i].name == NULL)
        {
            iconlist_setting->name = iconlist_parent_name;
            iconlist_setting->icon_list_theme = theme;
            return;
        }
    }
    //iocnlist num >= 10,should change ICONLIST_THEME_MAXNUM;
    gui_log("iocnlist num >= 10");
    GUI_ASSERT(false);
}

ICON_LIST_THEME_T gui_iocnlist_get_theme(char *iconlist_parent_name)
{
    for (size_t i = 0; i < ICONLIST_THEME_MAXNUM; i++)
    {
        if (iconlist_setting[i].name != NULL)
        {
            if (0 == strcmp(iconlist_setting[i].name, iconlist_parent_name))
            {
                return iconlist_setting[i].icon_list_theme;
            }
        }
        else
        {
            iconlist_setting[i].name = iconlist_parent_name;
            iconlist_setting[i].icon_list_theme = ICON_LIST_THEME_0;
            return ICON_LIST_THEME_0;
        }
    }
    //iocnlist num >= 10,should change ICONLIST_THEME_MAXNUM;
    gui_log("iocnlist num >= 10");
    GUI_ASSERT(false);
    return ICON_LIST_THEME_0;
}
extern gui_iconlist_default_t *gui_iconlist_create_default(void *parent, const char *filename,
                                                           int16_t x,
                                                           int16_t y,
                                                           int16_t w, int16_t h);

gui_iconlist_t *gui_iconlist_create(void *parent, const char *filename, int16_t x, int16_t y,
                                    int16_t w, int16_t h, uint16_t *config, uint8_t count)
{
    gui_obj_t *par = (gui_obj_t *)parent;
    ICON_LIST_THEME_T theme = gui_iocnlist_get_theme((char *)par->name);
    switch (theme)
    {
    case ICON_LIST_THEME_0:
        return (gui_iconlist_t *)gui_iconlist_create_default(par, filename, x, y, w, h);
    case ICON_LIST_THEME_1:
    //return (gui_iconlist_t *)gui_chessboard_create(par, filename, x, y, w, h, config, count);
    case ICON_LIST_THEME_2:
    //return (gui_iconlist_t *)gui_roulette_create(par, filename, x, y, w, h, config, count);
    default:
        return (gui_iconlist_t *)gui_iconlist_create_default(par, filename, x, y, w, h);
    }
}
