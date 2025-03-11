/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelist_new.c
  * @brief pagelist_new widget
  * @details A ram friendly page widget to show large amount of lists.
  *
  * @author triton_yu@realsil.com
  * @date 2023/12/15
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_obj.h"
#include "gui_pagelist_new.h"
#include "gui_button.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void render(int index, gui_obj_t *obj, gui_pagelist_new_t *pl)
{
    //GUI_WIDGET_POINTER_BY_TYPE(t, TEXTBOX, obj)
    {
        if (!(obj != ((void *)0)))
        {
            gui_assert_handler("obj != NULL", __FUNCTION__, 53);
        };
        if (!((((gui_obj_t *)obj)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 53);
        }
    }
    gui_obj_t *t = 0;
    extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE, gui_obj_t **);
    gui_obj_tree_get_widget_by_type(obj, TEXTBOX, &t);
    {
        if (!(t != ((void *)0)))
        {
            gui_assert_handler("t != NULL", __FUNCTION__, 53);
        };
        if (!((((gui_obj_t *)t)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(t)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 53);
        }
    }
    gui_text_content_set((void *)t, (void *)pl->item_text_array[index],
                         strlen(pl->item_text_array[index]));
    //change obj callback
    if (pl->click_function_array && pl->click_function_array[index])
    {
        obj->event_dsc->event_cb = pl->click_function_array[index];
        obj->event_dsc->user_data = (void *)(size_t)index;
    }



}
static void win_press(void *obj, gui_event_t e, void *param)
{
    // GUI_WIDGET_TYPE_TRY_EXCEPT(GUI_TYPE(gui_pagelist_new_t, GUI_BASE(obj)->parent->parent->parent),
    //                            MACRO_PAGE_LIST_NEW)
    {
        {
            if (!(((gui_pagelist_new_t *)
                   ((gui_obj_t *)obj)->parent->parent->parent) != ((void *)0)))
            {
                gui_assert_handler("((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent) != NULL",
                                   __FUNCTION__, 80);
            };
            if (!((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->magic ==
                   0x0b)))
            {
                gui_assert_handler("(GUI_BASE(((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->magic == GUI_MAGIC_NUMBER)",
                                   __FUNCTION__, 80);
            }
        }
        if ((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type !=
             MACRO_PAGE_LIST_NEW))
        {
            extern void gui_log(const char *format, ...);
            gui_log("GUI_WIDGET_TYPE_TRY_EXCEPT: type is %d ;",
                    ((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type);
        }
        {
            if (!((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type ==
                   MACRO_PAGE_LIST_NEW)))
            {
                gui_assert_handler("(GUI_BASE(((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type == MACRO_PAGE_LIST_NEW)",
                                   __FUNCTION__, 80);
            }
        }
    }
    //GUI_WIDGET_POINTER_BY_TYPE(img, IMAGE_FROM_MEM, obj)
    {
        if (!(obj != ((void *)0)))
        {
            gui_assert_handler("obj != NULL", __FUNCTION__, 82);
        };
        if (!((((gui_obj_t *)obj)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 82);
        }
    }
    gui_obj_t *img = 0;
    extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE, gui_obj_t **);
    gui_obj_tree_get_widget_by_type(obj, IMAGE_FROM_MEM, &img);
    {
        if (!(img != ((void *)0)))
        {
            gui_assert_handler("img != NULL", __FUNCTION__, 82);
        };
        if (!((((gui_obj_t *)img)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(img)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 82);
        }
    }
    GUI_TYPE(gui_img_t, img)->data = (void *)GUI_TYPE(gui_pagelist_new_t,
                                                      GUI_BASE(obj)->parent->parent->parent)->item_image_hl;
    GUI_TYPE(gui_pagelist_new_t, GUI_BASE(obj)->parent->parent->parent)->current_item_index = (size_t)(
                param);

}
static void win_release(void *obj, gui_event_t e, void *param)
{
    // GUI_WIDGET_TYPE_TRY_EXCEPT(GUI_TYPE(gui_pagelist_new_t, GUI_BASE(obj)->parent->parent->parent),
    //                            MACRO_PAGE_LIST_NEW)
    {
        {
            if (!(((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent) != ((void *)0)))
            {
                gui_assert_handler("((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent) != NULL",
                                   __FUNCTION__, 93);
            };
            if (!((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->magic ==
                   0x0b)))
            {
                gui_assert_handler("(GUI_BASE(((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->magic == GUI_MAGIC_NUMBER)",
                                   __FUNCTION__, 93);
            }
        }
        if ((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type !=
             MACRO_PAGE_LIST_NEW))
        {
            extern void gui_log(const char *format, ...);
            gui_log("GUI_WIDGET_TYPE_TRY_EXCEPT: type is %d ;",
                    ((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type);
        }
        {
            if (!((((gui_obj_t *)((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type ==
                   MACRO_PAGE_LIST_NEW)))
            {
                gui_assert_handler("(GUI_BASE(((gui_pagelist_new_t *)((gui_obj_t *)obj)->parent->parent->parent))->type == MACRO_PAGE_LIST_NEW)",
                                   __FUNCTION__, 93);
            }
        }
    }
    //GUI_WIDGET_POINTER_BY_TYPE(img, IMAGE_FROM_MEM, obj)
    {
        if (!(obj != ((void *)0)))
        {
            gui_assert_handler("obj != NULL", __FUNCTION__, 95);
        };
        if (!((((gui_obj_t *)obj)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 95);
        }
    }
    gui_obj_t *img = 0;
    extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE, gui_obj_t **);
    gui_obj_tree_get_widget_by_type(obj, IMAGE_FROM_MEM, &img);
    {
        if (!(img != ((void *)0)))
        {
            gui_assert_handler("img != NULL", __FUNCTION__, 95);
        };
        if (!((((gui_obj_t *)img)->magic == 0x0b)))
        {
            gui_assert_handler("(GUI_BASE(img)->magic == GUI_MAGIC_NUMBER)", __FUNCTION__, 95);
        }
    }
    GUI_TYPE(gui_img_t, img)->data = (void *)GUI_TYPE(gui_pagelist_new_t,
                                                      GUI_BASE(obj)->parent->parent->parent)->item_image;
    GUI_TYPE(gui_pagelist_new_t, GUI_BASE(obj)->parent->parent->parent)->current_item_index = (size_t)(
                param);
}
static void if_pl_app_take_over(int gap,
                                int time_array_size,
                                gui_pagelist_new_t *pl,
                                gui_win_t *timer1,
                                int count
                               )
{


    pl->app_take_over = 0;
    pl->touch_y = pl->app_y;
    {
        (pl->time_array_offset) = -(pl->touch_y / gap % time_array_size);
        int widget_offset = pl->touch_y % gap;
        GUI_BASE(timer1)->y = widget_offset;
        if (pl->alien && widget_offset == 0 && (pl->speed) <= pl->end_speed &&
            (pl->speed) >= -pl->end_speed)
        {
            pl->render_flag = 0;
        }
        if (pl->alien && widget_offset != 0)
        {
            if ((pl->speed) > 0 && (pl->speed) <= pl->end_speed)
            {
                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                else
                {
                    pl->touch_y += pl->end_speed;
                }
            }
            else if ((pl->speed) < 0 && (pl->speed) >= -pl->end_speed)
            {
                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                else
                {
                    pl->touch_y -= pl->end_speed;
                }
            }
            else if ((pl->speed) == 0)
            {
                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                if (widget_offset > 0)
                {
                    if (widget_offset > gap / 2)
                    {
                        pl->touch_y += pl->end_speed;
                    }
                    else
                    {
                        pl->touch_y -= pl->end_speed;
                    }
                }
                else if (widget_offset < 0)
                {
                    if (-widget_offset > gap / 2)
                    {
                        pl->touch_y -= pl->end_speed;
                    }
                    else
                    {
                        pl->touch_y += pl->end_speed;
                    }
                }
            }
        }
        {
            for (size_t i = 0; i < count; i++)
            {

                int index = (pl->time_array_offset) + i;
                if (index >= time_array_size)
                {
                    index -= time_array_size;
                }
                if (index < 0)
                {
                    index += time_array_size;
                }
                const char *text = pl->item_text_array[index]; GUI_UNUSED(text);
                if (i == (count) / 2)
                {
                    (pl->index_offset) = index;
                }
                gui_obj_t *output = 0;
                gui_obj_tree_get_widget_by_type_and_index((void *)timer1, WINDOW, &output, i);
                render(index, output, pl);
            }
        }
    }



}
static void if_not_pl_wheel_take_over(int gap,
                                      int time_array_size,
                                      gui_pagelist_new_t *pl,
                                      gui_win_t *timer1,
                                      int count, gui_win_t *win, int ax, int *speed_recode, int sizeof_speed_recode
                                     )

{
    IMPORT_GUI_TOUCHPAD

    if (touch->pressed && touch->x > ax && touch->x < ax + GUI_BASE(win)->w)
    {
        (pl->history_y) = pl->touch_y;
        (pl->speed) = 0;
        memset(speed_recode, 0, sizeof_speed_recode);
        (pl->event5_flag) = 0;
    }
    if (touch->released && (pl->time_array_offset) < 0)
    {
        GUI_BASE(timer1)->y = 0;
        (pl->time_array_offset) = 0;
        pl->render_flag = 0;
        (pl->history_y) = 0;
        pl->touch_y = 0;
    }
    if (touch->pressing && (touch->deltaX != 0 || touch->deltaY != 0))
    {
        gui_img_t *img = 0;
        gui_obj_tree_get_widget_by_type_and_index((void *)pl->timer, IMAGE_FROM_MEM, (void *)&img,
                                                  pl->current_item_index);
        GUI_TYPE(gui_img_t, img)->data = (void *)pl->item_image;
    }

    if (touch->pressing && touch->x > ax && touch->x < ax + GUI_BASE(win)->w)
    {
        pl->touch_y = (pl->history_y) + touch->deltaY;
        /**
         * Index = offset / gap % array length
           WidgetOffset = offset % gap
        */
        gui_update_speed_by_displacement(&(pl->speed), speed_recode, touch->deltaY);
        pl->render_flag = 1;


    }
    else
    {
        gui_inertial(&(pl->speed), pl->end_speed, &(pl->touch_y));
        if (pl->touch_y > 0)
        {
            pl->touch_y = 0;
        }
    }

    if (pl->touch_y < -gap * (time_array_size) + (int)gui_get_screen_height())
    {
        pl->touch_y = -gap * (time_array_size) + (int)gui_get_screen_height();
    }
    if (pl->render_flag)
    {
        (pl->time_array_offset) = -(pl->touch_y / gap % time_array_size);
        int widget_offset = pl->touch_y % gap;
        if ((pl->time_array_offset) < 0)
        {
            widget_offset = pl->touch_y;
            (pl->time_array_offset) = -1;

            if (!(pl->event5_flag))
            {
                win->event5_flag = 1;
            }
            else
            {
                win->event5_flag = 0;
            }

            (pl->event5_flag) = 1;
        }


        GUI_BASE(timer1)->y = widget_offset;
        if (pl->alien && widget_offset == 0 && (pl->speed) <= pl->end_speed &&
            (pl->speed) >= -pl->end_speed)
        {
            pl->render_flag = 0;
        }
        if (pl->alien && widget_offset != 0)
        {
            if ((pl->speed) > 0 && (pl->speed) <= pl->end_speed)
            {

                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                else
                {
                    pl->touch_y += pl->end_speed;
                }


            }
            else if ((pl->speed) < 0 && (pl->speed) >= -pl->end_speed)
            {

                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                else
                {
                    pl->touch_y -= pl->end_speed;
                }

            }
            else if ((pl->speed) == 0)
            {
                if (_UI_ABS(widget_offset) <= pl->end_speed)
                {
                    widget_offset = 0;
                }
                if (widget_offset > 0)
                {
                    if (widget_offset > gap / 2)
                    {
                        pl->touch_y += pl->end_speed;
                    }
                    else
                    {
                        pl->touch_y -= pl->end_speed;
                    }
                }
                else if (widget_offset < 0)
                {
                    if (-widget_offset > gap / 2)
                    {
                        pl->touch_y -= pl->end_speed;
                    }
                    else
                    {
                        pl->touch_y += pl->end_speed;
                    }
                }
            }
        }
        if ((pl->time_array_offset) >= 0)
        {
            for (size_t i = 0; i < count; i++)
            {
                if (i >= pl->item_count)
                {
                    continue;
                }

                int index = (pl->time_array_offset) + i;
                if (index >= time_array_size)
                {
                    index -= time_array_size;
                }
                if (index < 0)
                {
                    index += time_array_size;
                }
                const char *text = pl->item_text_array[index]; GUI_UNUSED(text);
                if (i == (count) / 2)
                {
                    (pl->index_offset) = index;
                }
                gui_obj_t *output = 0;
                gui_obj_tree_get_widget_by_type_and_index((void *)timer1, WINDOW, &output, i);
                //gui_log("type_and_index:%d, %x,%x\n", timer1, output, i);
                render(index, output, pl);
            }
        }
    }
    else
    {
        (pl->history_y) = pl->touch_y;
    }
}
static void if_pl_alien_and_widget_offset_not_0(int gap,
                                                int time_array_size,
                                                gui_pagelist_new_t *pl,
                                                gui_win_t *timer1,
                                                int count, gui_win_t *win, int ax, int *speed_recode, int sizeof_speed_recode,
                                                int widget_offset
                                               )

{
    if ((pl->speed) > 0 && (pl->speed) <= pl->end_speed)
    {
        if (_UI_ABS(widget_offset) <= pl->end_speed)
        {
            widget_offset = 0;
        }
        else
        {
            pl->touch_y += pl->end_speed;
        }
    }
    else if ((pl->speed) < 0 && (pl->speed) >= -pl->end_speed)
    {
        if (_UI_ABS(widget_offset) <= pl->end_speed)
        {
            widget_offset = 0;
        }
        else
        {
            pl->touch_y -= pl->end_speed;
        }
    }
    else if ((pl->speed) == 0)
    {
        if (_UI_ABS(widget_offset) <= pl->end_speed)
        {
            widget_offset = 0;
        }
        if (widget_offset > 0)
        {
            if (widget_offset > gap / 2)
            {
                pl->touch_y += pl->end_speed;
            }
            else
            {
                pl->touch_y -= pl->end_speed;
            }
        }
        else if (widget_offset < 0)
        {
            if (-widget_offset > gap / 2)
            {
                pl->touch_y -= pl->end_speed;
            }
            else
            {
                pl->touch_y += pl->end_speed;
            }
        }
    }
}

static void if_pl_wheel_take_over(int gap,
                                  int time_array_size,
                                  gui_pagelist_new_t *pl,
                                  gui_win_t *timer1,
                                  int count, gui_win_t *win, int ax, int *speed_recode, int sizeof_speed_recode
                                 )

{
    IMPORT_GUI_WHEEL
    pl->touch_y = wheel->history_y;
    {
        (pl->time_array_offset) = -(pl->touch_y / gap % time_array_size);
        int widget_offset = pl->touch_y % gap;
        GUI_BASE(timer1)->y = widget_offset;
        if (pl->alien && widget_offset == 0 && (pl->speed) <= pl->end_speed &&
            (pl->speed) >= -pl->end_speed)
        {
            pl->render_flag = 0;
        }
        if (pl->alien && widget_offset != 0)
        {
            if_pl_alien_and_widget_offset_not_0(gap,
                                                time_array_size,
                                                pl,
                                                timer1,
                                                count, win,  ax, speed_recode,  sizeof_speed_recode,
                                                widget_offset
                                               );
        }
        {
            for (size_t i = 0; i < count; i++)
            {

                int index = (pl->time_array_offset) + i;
                if (index >= time_array_size)
                {
                    index -= time_array_size;
                }
                if (index < 0)
                {
                    index += time_array_size;
                }
                const char *text = pl->item_text_array[index]; GUI_UNUSED(text);
                if (i == (count) / 2)
                {
                    (pl->index_offset) = index;
                }
                gui_obj_t *output = 0;
                gui_obj_tree_get_widget_by_type_and_index((void *)timer1, WINDOW, &output, i);
                render(index, output, pl);
            }
        }
    }
}

static void override(void *p, void *this_widget, gui_animate_t *animate)
{
    gui_win_t *win = this_widget;
    gui_pagelist_new_t *pl = p;
    if (GUI_BASE(pl)->gesture)
    {
        return;
    }
    if (pl->item_count < pl->row_count - 1)
    {
        return;
    }

    gui_win_t *timer1 = pl->timer;
    IMPORT_GUI_TOUCHPAD
    IMPORT_GUI_WHEEL
    static int speed_recode[5];
    int gap = pl->row_space;
    int time_array_size = pl->item_count;
    int count = pl->row_count;
    pl->end_speed = 3;
    pl->alien = 1;
    if (wheel->pressed)
    {
        (pl->wheel_take_over) = 1;
        wheel->history_x = pl->touch_y;
    }
    if (wheel->released)
    {
        (pl->wheel_take_over) = 0;
    }
    int ax, ay;
    gui_obj_absolute_xy(GUI_BASE(win), &ax, &ay);
    if ((touch->pressed && touch->x > ax && touch->x < ax + GUI_BASE(win)->w) || touch->released ||
        touch->pressing)
    {
        pl->app_take_over = 0;
    }
    if (pl->app_take_over)
    {

        if_pl_app_take_over(gap, time_array_size,
                            pl,
                            timer1,
                            count);
        return;
    }
    if (!(pl->wheel_take_over))
    {
        if_not_pl_wheel_take_over(gap, time_array_size,
                                  pl,
                                  timer1,
                                  count, win,  ax,   speed_recode, sizeof(speed_recode));
    }
    else
    {
        if_pl_wheel_take_over(gap, time_array_size,
                              pl,
                              timer1,
                              count, win,  ax,   speed_recode, sizeof(speed_recode));
    }
}
static void override_horizontal(void *p, void *this_widget, gui_animate_t *animate)
{
    gui_win_t *win = this_widget;
    gui_pagelist_new_t *pl = p;
    if (GUI_BASE(pl)->gesture)
    {
        return;
    }

    if (pl->item_count < pl->row_count - 1)
    {
        return;
    }

    gui_win_t *timer1 = pl->timer;
    IMPORT_GUI_TOUCHPAD
    IMPORT_GUI_WHEEL
    static int speed_recode[5];
    int gap = pl->row_space;
    int time_array_size = pl->item_count;
    int count = pl->row_count;
    pl->end_speed = 3;
    pl->alien = 1;
    if (wheel->pressed)
    {
        (pl->wheel_take_over) = 1;
        wheel->history_x = pl->touch_y;
    }
    if (wheel->released)
    {
        (pl->wheel_take_over) = 0;
    }

    if (!(pl->wheel_take_over))
    {
        int ax, ay;
        gui_obj_absolute_xy(GUI_BASE(win), &ax, &ay);
        if (touch->pressed && touch->y > ay && touch->y < ay + GUI_BASE(win)->h)
        {
            (pl->history_y) = pl->touch_y;
            (pl->speed) = 0;
            memset(speed_recode, 0, sizeof(speed_recode));
            (pl->event5_flag) = 0;
        }
        if (touch->released && (pl->time_array_offset) < 0)
        {
            GUI_BASE(timer1)->x = 0;
            (pl->time_array_offset) = 0;
            pl->render_flag = 0;
            (pl->history_y) = 0;
            pl->touch_y = 0;
        }
        if (touch->pressing && (touch->deltaY != 0 || touch->deltaX != 0))
        {
            gui_img_t *img = 0;
            gui_obj_tree_get_widget_by_type_and_index((void *)pl->timer, IMAGE_FROM_MEM, (void *)&img,
                                                      pl->current_item_index);
            GUI_TYPE(gui_img_t, img)->data = (void *)pl->item_image;
        }

        if (touch->pressing && touch->y > ay && touch->y < ay + GUI_BASE(win)->h)
        {
            pl->touch_y = (pl->history_y) + touch->deltaX;
            /**
             * Index = offset / gap % array length
               WidgetOffset = offset % gap
            */
            gui_update_speed_by_displacement(&(pl->speed), speed_recode, touch->deltaX);
            pl->render_flag = 1;


        }
        else
        {
            gui_inertial(&(pl->speed), pl->end_speed, &(pl->touch_y));
            if (pl->touch_y > 0)
            {
                pl->touch_y = 0;
            }
        }

        if (pl->touch_y < -gap * (time_array_size) + (int)gui_get_screen_width())
        {
            pl->touch_y = -gap * (time_array_size) + (int)gui_get_screen_width();
        }
        if (pl->render_flag)
        {
            (pl->time_array_offset) = -(pl->touch_y / gap % time_array_size);
            int widget_offset = pl->touch_y % gap;
            if ((pl->time_array_offset) < 0)
            {
                widget_offset = pl->touch_y;
                (pl->time_array_offset) = -1;

                if (!(pl->event5_flag))
                {
                    win->event5_flag = 1;
                }
                else
                {
                    win->event5_flag = 0;
                }

                (pl->event5_flag) = 1;
            }


            GUI_BASE(timer1)->x = widget_offset;
            if (pl->alien && widget_offset == 0 && (pl->speed) <= pl->end_speed &&
                (pl->speed) >= -pl->end_speed)
            {
                pl->render_flag = 0;
            }
            if (pl->alien && widget_offset != 0)
            {
                if ((pl->speed) > 0 && (pl->speed) <= pl->end_speed)
                {

                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        pl->touch_y += pl->end_speed;
                    }


                }
                else if ((pl->speed) < 0 && (pl->speed) >= -pl->end_speed)
                {

                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        pl->touch_y -= pl->end_speed;
                    }

                }
                else if ((pl->speed) == 0)
                {
                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > gap / 2)
                        {
                            pl->touch_y += pl->end_speed;
                        }
                        else
                        {
                            pl->touch_y -= pl->end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > gap / 2)
                        {
                            pl->touch_y -= pl->end_speed;
                        }
                        else
                        {
                            pl->touch_y += pl->end_speed;
                        }
                    }
                }
            }
            if ((pl->time_array_offset) >= 0)
            {
                for (size_t i = 0; i < count; i++)
                {
                    if (i >= pl->item_count)
                    {
                        continue;
                    }

                    int index = (pl->time_array_offset) + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = pl->item_text_array[index]; GUI_UNUSED(text);
                    if (i == (count) / 2)
                    {
                        (pl->index_offset) = index;
                    }
                    gui_obj_t *output = 0;
                    gui_obj_tree_get_widget_by_type_and_index((void *)timer1, WINDOW, &output, i);
                    //gui_log("type_and_index:%d, %x,%x\n", timer1, output, i);
                    render(index, output, pl);
                }
            }
        }
        else
        {
            (pl->history_y) = pl->touch_y;
        }
    }
    else
    {
        pl->touch_y = wheel->history_y;
        {
            (pl->time_array_offset) = -(pl->touch_y / gap % time_array_size);
            int widget_offset = pl->touch_y % gap;
            GUI_BASE(timer1)->x = widget_offset;
            if (pl->alien && widget_offset == 0 && (pl->speed) <= pl->end_speed &&
                (pl->speed) >= -pl->end_speed)
            {
                pl->render_flag = 0;
            }
            if (pl->alien && widget_offset != 0)
            {
                if ((pl->speed) > 0 && (pl->speed) <= pl->end_speed)
                {
                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        pl->touch_y += pl->end_speed;
                    }
                }
                else if ((pl->speed) < 0 && (pl->speed) >= -pl->end_speed)
                {
                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        pl->touch_y -= pl->end_speed;
                    }
                }
                else if ((pl->speed) == 0)
                {
                    if (_UI_ABS(widget_offset) <= pl->end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > gap / 2)
                        {
                            pl->touch_y += pl->end_speed;
                        }
                        else
                        {
                            pl->touch_y -= pl->end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > gap / 2)
                        {
                            pl->touch_y -= pl->end_speed;
                        }
                        else
                        {
                            pl->touch_y += pl->end_speed;
                        }
                    }
                }
            }
            {
                for (size_t i = 0; i < count; i++)
                {

                    int index = (pl->time_array_offset) + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = pl->item_text_array[index]; GUI_UNUSED(text);
                    if (i == (count) / 2)
                    {
                        (pl->index_offset) = index;
                    }
                    gui_obj_t *output = 0;
                    gui_obj_tree_get_widget_by_type_and_index((void *)timer1, WINDOW, &output, i);
                    render(index, output, pl);
                }
            }
        }
    }
}
static void ctor(gui_pagelist_new_t *this, void       *parent,
                 int16_t     x,
                 int16_t     y,
                 uint16_t     w,
                 uint16_t row_space,
                 const uint8_t *item_background_image,
                 const uint8_t *item_background_image_hl,
                 BLEND_MODE_TYPE item_background_blending_mode,
                 const uint8_t *font,
                 uint16_t font_size,
                 gui_color_t font_color,
                 bool horizontal)
{
    gui_obj_ctor(GUI_BASE(this), parent, "pageListNew", x, y, w,
                 row_space * (gui_get_screen_height() / row_space + 1));
    this->row_space = row_space;
    this->row_count = gui_get_screen_height() / row_space + 2;
    this->item_image = item_background_image;
    this->blending = item_background_blending_mode;
    this->font = font;
    this->font_size = font_size;
    this->font_color = font_color;
    this->item_image_hl = item_background_image_hl;
    GUI_BASE(this)->type = MACRO_PAGE_LIST_NEW;
    this->horizontal = horizontal;
    if (horizontal)
    {
        GUI_BASE(this)->w = row_space * (gui_get_screen_height() / row_space + 1);
        GUI_BASE(this)->h = w;
    }
    this->text_mode = MID_CENTER;
}


gui_pagelist_new_t *gui_pagelist_new_create(void       *parent,
                                            int16_t     x,
                                            int16_t     y,
                                            uint16_t     w,
                                            uint16_t item_space,
                                            const uint8_t *item_background_image,
                                            const uint8_t *item_background_image_highlight,
                                            BLEND_MODE_TYPE item_background_blending_mode,
                                            const uint8_t *font,
                                            uint16_t font_size,
                                            gui_color_t font_color
                                           )
{
#define _GUI_NEW_GUI_PAGE_LIST_NEW_PARAM this, parent, x, y, w,item_space ,item_background_image,item_background_image_highlight,item_background_blending_mode,font,font_size, font_color,0
    GUI_CREATE_HELPER(gui_pagelist_new_t, ctor, _GUI_NEW_GUI_PAGE_LIST_NEW_PARAM)
    return this;
}
gui_pagelist_new_t *gui_pagelist_new_create_horizontal(void       *parent,
                                                       int16_t     x,
                                                       int16_t     y,
                                                       uint16_t item_space,
                                                       uint16_t     h,
                                                       const uint8_t *item_background_image,
                                                       const uint8_t *item_background_image_highlight,
                                                       BLEND_MODE_TYPE item_background_blending_mode,
                                                       const uint8_t *font,
                                                       uint16_t font_size,
                                                       gui_color_t font_color
                                                      )
{
#define _GUI_NEW_GUI_PAGE_LIST_NEW_HORIZONTAL_PARAM this, parent, x, y, h,item_space ,item_background_image,item_background_image_highlight,item_background_blending_mode,font,font_size, font_color,1
    GUI_CREATE_HELPER(gui_pagelist_new_t, ctor, _GUI_NEW_GUI_PAGE_LIST_NEW_HORIZONTAL_PARAM)
    return this;
}
gui_error_t gui_page_list_new_render(gui_pagelist_new_t *pagelist_new,
                                     const uint16_t item_count,
                                     const gui_event_cb_t *item_click_function_array,
                                     const char **item_text_array
                                    )
{
    if (!(item_count  && item_text_array && pagelist_new))
    {
        return GUI_ERROR_NULL;
    }
    gui_obj_child_free((void *)pagelist_new);
    {
        if (pagelist_new->click_function_array)
        {
            if (pagelist_new->append)
            {
                gui_free((void *)pagelist_new->click_function_array);
            }
        }
        if (pagelist_new->item_text_array)
        {
            if (pagelist_new->append)
            {
                gui_free(pagelist_new->item_text_array);
            }
        }
        pagelist_new->append = false;
        pagelist_new->item_count = item_count;
        pagelist_new->click_function_array = item_click_function_array;
        pagelist_new->item_text_array = item_text_array;
    }
    gui_win_t *win = gui_win_create(pagelist_new, 0, 0, 0, GUI_BASE(pagelist_new)->w,
                                    GUI_BASE(pagelist_new)->h);
    gui_win_t *timer1 = gui_win_create(win, 0, 0, 0, 0, 0);

    pagelist_new->timer = timer1;
    int count = pagelist_new->row_count;
    if (item_count < count)
    {
        count = item_count;
    }

    if (pagelist_new->horizontal)
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)override_horizontal, pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, 0, i * pagelist_new->row_space, 0, pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->h);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, pagelist_new->row_space, GUI_BASE(win)->h);
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);
            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i], (gui_event_cb_t)i);
            }

            gui_win_press(win, win_press, (void *)i);
            gui_win_release(win, win_release, (void *)i);
        }
    }
    else
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)override, pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, 0, 0, i * pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->w, pagelist_new->row_space);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, GUI_BASE(win)->w, pagelist_new->row_space);
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);

            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i], (gui_event_cb_t)i);
            }
            gui_win_press(win, win_press, (void *)i);
            gui_win_release(win, win_release, (void *)i);
        }
    }
    return GUI_SUCCESS;

}
void gui_page_list_new_pushback(gui_pagelist_new_t *pagelist_new,
                                const gui_event_cb_t item_click_function,
                                const char *item_text)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(pagelist_new, MACRO_PAGE_LIST_NEW)
    gui_event_cb_t *item_click_function_array = gui_malloc(sizeof(gui_event_cb_t) *
                                                           (++pagelist_new->item_count));
    char **item_text_array = gui_malloc(sizeof(char *) * (pagelist_new->item_count));

    if (item_click_function_array && item_text_array)
    {
        if (pagelist_new->click_function_array)
        {
            for (size_t i = 0; i < pagelist_new->item_count - 1; i++)
            {
                item_click_function_array[i] = pagelist_new->click_function_array[i];
            }
        }
        if (pagelist_new->item_text_array)
        {
            for (size_t i = 0; i < pagelist_new->item_count - 1; i++)
            {
                item_text_array[i] = (void *)pagelist_new->item_text_array[i];
            }
        }
        item_click_function_array[pagelist_new->item_count - 1] = item_click_function;
        item_text_array[pagelist_new->item_count - 1] = (void *)item_text;
        gui_page_list_new_render(pagelist_new, pagelist_new->item_count, item_click_function_array,
                                 (void *)item_text_array);
    }
    pagelist_new->append = true;
}
void gui_page_list_new_erase(gui_pagelist_new_t *pagelist_new, int id)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(pagelist_new, MACRO_PAGE_LIST_NEW)
    if (id + 1 > pagelist_new->item_count)
    {
        return;
    }

    gui_event_cb_t *item_click_function_array = gui_malloc(sizeof(gui_event_cb_t) *
                                                           (--pagelist_new->item_count));
    char **item_text_array = gui_malloc(sizeof(char *) * (pagelist_new->item_count));
    if (item_click_function_array && item_text_array)
    {
        if (pagelist_new->click_function_array)
        {
            int id_new = 0, id_old = 0;
            for (size_t i = 0; i < pagelist_new->item_count; i++)
            {

                if (i == id)
                {
                    id_old++;
                }

                item_click_function_array[id_new] = pagelist_new->click_function_array[id_old];
                id_new++;
                id_old++;
            }
        }
        if (pagelist_new->item_text_array)
        {
            int id_new = 0, id_old = 0;
            for (size_t i = 0; i < pagelist_new->item_count; i++)
            {

                if (i == id)
                {
                    id_old++;
                }
                item_text_array[id_new] = (void *)pagelist_new->item_text_array[id_old];
                id_new++;
                id_old++;
            }
        }
        gui_page_list_new_render(pagelist_new, pagelist_new->item_count, item_click_function_array,
                                 (void *)item_text_array);
    }
    pagelist_new->append = true;
}
