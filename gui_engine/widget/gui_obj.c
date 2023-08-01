/*
 * File      : gui_obj.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_obj.h>
#include <string.h>
#include <string.h>
#include <stdio.h>



void gui_obj_ctor(gui_obj_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y,
                  int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);

    this->parent = parent;
    if (!name)
    {
        name = "_default_widget";
    }

    this->name = name;

    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;

}
void gui_obj_dtor(gui_obj_t *this)
{
    volatile int a;
    a = this->x;
    return;
}


void gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter,
                          void *user_data)
{
    gui_obj_t *object = (gui_obj_t *)obj;
    object->event_dsc_cnt++;
    object->event_dsc = gui_realloc(object->event_dsc, sizeof(gui_event_dsc_t) * object->event_dsc_cnt);

    gui_event_dsc_t *event_dsc = object->event_dsc + object->event_dsc_cnt - 1;
    event_dsc->event_cb = event_cb;
    event_dsc->filter = filter;
    event_dsc->user_data = user_data;
    event_dsc->event_code = GUI_EVENT_INVALIDE;

}

//this API only for Widget, not for Application
void gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code)
{
    for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        event_dsc->event_code = event_code;
    }
}

/**
 * @brief print the widget tree recursively,from the root to the leaves.Preorder traversal.
 * @param obj the root of the widget tree.
 */
void gui_tree_print(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_log(" %s \n", obj->name);
        gui_tree_print(obj);
    }
}




static void gui_obj_destory_cb(gui_obj_t *obj)
{
    gui_log("do obj %s free, line = %d\n", obj->name, __LINE__);
    if (obj->obj_destory)
    {
        obj->obj_destory(obj);
    }


}
static void gui_tree_child_free(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_tree_child_free(obj);
        gui_obj_destory_cb(obj);
        gui_free(obj);
    }
}



void gui_tree_free(void *obj)
{
    gui_obj_t *object = (gui_obj_t *)obj;
    gui_tree_child_free(object);
    if (object->parent)
    {
        gui_list_remove(&object->brother_list);
        gui_obj_destory_cb(obj);
        gui_free(object->event_dsc);
        gui_free(obj);
    }
    else
    {
        gui_log("obj %s parent equal NULL!\n", object->name);
        gui_list_init(&object->child_list);
        gui_list_init(&object->child_list);
    }
}


void gui_tree_not_show(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->not_show = true;
        gui_tree_print(obj);
    }
}
void gui_tree_show(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->not_show = false;
        gui_tree_show(obj);
    }
}



gui_obj_t *gui_tree_get_root(gui_obj_t *obj)
{
    gui_obj_t *child = obj;
    while (true)
    {
        if (child->parent == NULL)
        {
            gui_log("gui_tree_get_root = %s\n", child->name);
            return child;
        }
        else
        {
            child = child->parent;
        }
    }
}


void gui_obj_show(void *obj, bool show_info)
{
    // gui_log("%p",obj);
    if (obj != NULL)
    {
        GET_BASE(obj)->not_show = !show_info;
    }
}



gui_obj_t *gui_get_child_handle(gui_obj_t *obj, obj_type_t child_type)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == child_type)
        {
            return obj;
        }
    }
    return NULL;
}
void gui_tree_get_cover(gui_obj_t *obj, obj_type_t target, bool *rst)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == target)
        {
            if (!(*rst))
            {
                *rst =  obj->cover;
            }
        }
        gui_tree_get_cover(obj, target, rst);
    }
}
