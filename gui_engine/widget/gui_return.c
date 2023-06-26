/*
 * File      : gui_return.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_return.h>
#include <string.h>
#include <gui_return.h>
#include "gui_obj.h"
#include "gui_app.h"
#include "gui_magic_img.h"
#include "tp_algo.h"
static void screen_backfunc()
{
#ifdef RTK_GUI_SCRIPT_AS_A_APP
    gui_app_t *app = gui_current_app();
    extern void *get_app_launcher_frontend(void);
    gui_switch_app(app, get_app_launcher_frontend());
#endif
}
void return_update_att(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_return_t *this = (gui_return_t *)obj;
    if (tp->type == TOUCH_HOLD_X)
    {
        if (tp->x > 0 && tp->x < 65 && tp->deltaX < 100 && tp->y > (gui_get_screen_height() - 200))
        {
            gui_img_set_attribute(this->return_img_left, NULL, NULL, tp->deltaX - 100,
                                  this->return_img_left->base.y);
            this->hit = true;
        }
        else if (tp->x > (gui_get_screen_width() - 100) && tp->x < gui_get_screen_width() &&
                 tp->deltaX > -100 && tp->y > (gui_get_screen_height() - 200))
        {
            gui_img_set_attribute(this->return_img_right, NULL, NULL, tp->deltaX + gui_get_screen_width(),
                                  this->return_img_right->base.y);
            this->hit = true;
        }
        else
        {
            //this->hit = false;
        }

    }
    else if (tp->released)
    {
        if (this->hit)
        {
            this->hit = false;
            if (tp->deltaX > 99 || tp->deltaX < -99)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
            }
            gui_img_set_attribute(this->return_img_left, NULL, NULL, -100, this->return_img_left->base.y);
            gui_img_set_attribute(this->return_img_right, NULL, NULL, gui_get_screen_width(),
                                  this->return_img_right->base.y);
        }


    }

}


void gui_return_ctor(gui_return_t *this, gui_obj_t *parent, const char *widgetame)
{
    gui_obj_ctor(&this->base, parent, widgetame, 0, 0, 0, 0);
    ((gui_obj_t *)this)->type = RETURNWIDGET;
    this->base.obj_update_att = return_update_att;
}
gui_return_t *gui_return_create(void *parent)
{

    gui_return_t *this = gui_malloc(sizeof(gui_return_t));
    memset(this, 0, sizeof(gui_return_t));
    gui_return_ctor(this, parent, "return");
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));

    }
    gui_obj_add_event_cb(this, (gui_event_cb_t)screen_backfunc, GUI_EVENT_TOUCH_CLICKED, NULL);
    this->return_img_left = (void *)gui_img_create_from_fs(this, "app/system/resource/back_left.bin",
                                                           -100, gui_get_screen_height() - 200);
    this->return_img_right = (void *)gui_img_create_from_fs(this, "app/system/resource/back_right.bin",
                                                            gui_get_screen_width(), gui_get_screen_height() - 200);

    ((gui_obj_t *)this)->create_done = 1;
    return this;

}


