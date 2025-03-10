/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_suppress.c
  * @brief  object suppression functions
  * @details suppression related functions for handling touch events
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
#include "gui_obj_suppress.h"
#include "gui_api.h"

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


/*============================================================================*
*                           Public Functions
*============================================================================*/

void gui_obj_skip_all_parent_left_hold(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_left_hold = true;
    }
}

void gui_obj_skip_all_child_left_hold(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_left_hold = true;
        gui_obj_skip_all_child_left_hold(o);
    }
}

void gui_obj_skip_all_parent_right_hold(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_right_hold = true;
    }
}

void gui_obj_skip_all_child_right_hold(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_right_hold = true;
        gui_obj_skip_all_child_right_hold(o);
    }
}

void gui_obj_skip_all_parent_up_hold(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_up_hold = true;
    }
}

void gui_obj_skip_all_child_up_hold(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_up_hold = true;
        gui_obj_skip_all_child_up_hold(o);
    }
}

void gui_obj_skip_all_parent_down_hold(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_down_hold = true;
    }
}

void gui_obj_skip_all_child_down_hold(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_down_hold = true;
        gui_obj_skip_all_child_down_hold(o);
    }
}

void gui_obj_skip_other_up_hold(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_up_hold(o);
    obj->skip_tp_up_hold = false;
}

void gui_obj_skip_other_down_hold(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_down_hold(o);
    obj->skip_tp_down_hold = false;
}

void gui_obj_skip_other_left_hold(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_left_hold(o);
    obj->skip_tp_left_hold = false;
}

void gui_obj_skip_other_right_hold(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_right_hold(o);
    obj->skip_tp_right_hold = false;
}


void gui_obj_skip_all_child_short(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_short = true;
        gui_obj_skip_all_child_short(o);
    }
}

void gui_obj_skip_other_short(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_short(o);
    obj->skip_tp_short = false;
}

void gui_obj_enable_this_parent_short(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_short = false;
    }
}

void gui_obj_skip_other_parent_short(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_short(o);
    gui_obj_enable_this_parent_short(obj);
}

void gui_obj_skip_all_child_long(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_long = true;
        gui_obj_skip_all_child_long(o);
    }
}

void gui_obj_skip_other_long(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_long(o);
    obj->skip_tp_long = false;
}

void gui_obj_enable_this_parent_long(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_long = false;
    }
}

void gui_obj_skip_other_parent_long(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_long(o);
    gui_obj_enable_this_parent_long(obj);
}

void gui_obj_skip_all_child_pressed(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, obj);
            gui_log("@name:%s, @type:%d\n", obj->name, obj->type);
            return;
        }
        o->skip_tp_pressed = true;
        gui_obj_skip_all_child_pressed(o);
    }
}

void gui_obj_skip_other_pressed(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_pressed(o);
    obj->skip_tp_pressed = false;
}

void gui_obj_enable_this_parent_pressed(gui_obj_t *obj)
{
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        o->skip_tp_pressed = false;
    }
}

void gui_obj_skip_other_parent_pressed(gui_obj_t *obj)
{
    gui_obj_t *o = obj;

    while (o->parent != NULL)
    {
        o = o->parent;
    }

    gui_obj_skip_all_child_pressed(o);
    gui_obj_enable_this_parent_pressed(obj);
}