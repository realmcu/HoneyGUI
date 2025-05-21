/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_win.c
  * @brief Window widget
  * @details A rectangular container widget which can listen to many gestures
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_multi_level.h"
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

static void ctor(gui_multi_level_t  *this,
                 gui_obj_t  *parent,
                 const char *filename, void (*ui_design)(gui_obj_t *))
{
    gui_obj_ctor(&this->base, parent, filename, 0, 0, 0, 0);
    GET_BASE(this)->type = MULTI_LEVEL;



}

static void tree_child_free(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;

    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        tree_child_free(obj);
        if (obj->type != MULTI_LEVEL)
        {
            //gui_log("do obj %s free(destroy), line = %d\n", obj->name, __LINE__);
//gui_log("free name:%s\n",obj->name);
            if (obj->matrix != NULL)
            {
                gui_free(obj->matrix);
            }

            if (obj->has_destroy_cb)
            {
                obj->obj_cb(obj, OBJ_DESTROY);
            }

            if (obj->event_dsc != NULL)
            {
                gui_free(obj->event_dsc);
            }
            gui_list_remove(&obj->brother_list);
            gui_free(obj);
        }


    }
}
typedef struct _queue_node_t
{
    gui_obj_t *gui_obj;
    int level;
    int order;
} queue_node_t;

static gui_obj_t *gui_obj_get_by_level_order_hide(gui_obj_t *root, int target_level,
                                                  int target_order)
{
    if (root == NULL)
    {
        return NULL;
    }
    int count = 0;
    gui_obj_tree_count_by_type(root, MULTI_LEVEL, &count);
    queue_node_t queue[count + 3];
    int front = 0, rear = 0, level_order[count + 3];
    memset(level_order, 0, count + 3 * sizeof(int));
    if (root->type == MULTI_LEVEL)
    {
        queue[rear].gui_obj = root;
        queue[rear].level = 1;
        queue[rear].order = ++level_order[1];
        ++rear;
        if (GUI_TYPE(gui_multi_level_t, root)->hidden == 0)
        {
            GUI_TYPE(gui_multi_level_t, root)->hidden = 1;
        }
    }


    while (front != rear)
    {
        queue_node_t current_queue_node = queue[front++];
        gui_obj_t *current_gui_obj = current_queue_node.gui_obj;
        int current_level = current_queue_node.level;
        int current_order = current_queue_node.order;
        if (current_gui_obj->type == MULTI_LEVEL && current_level == target_level &&
            current_order == target_order)
        {
            return current_gui_obj;
        }
        gui_list_t *node = NULL;
        gui_list_for_each(node, &current_gui_obj->child_list)
        {
            gui_obj_t *child_obj = gui_list_entry(node, gui_obj_t, brother_list);

            if (child_obj->type == MULTI_LEVEL)
            {
                queue[rear].gui_obj = child_obj;
                int child_level = current_level + 1;
                int child_order = ++level_order[child_level];
                queue[rear].level = child_level;
                queue[rear].order = child_order;
                //gui_log("child_level:%d, child_order:%d\n", child_level, child_order);
                GUI_TYPE(gui_multi_level_t, child_obj)->index = child_order - 1;
                GUI_TYPE(gui_multi_level_t, child_obj)->level = child_level - 1;
                ++rear;
                if (GUI_TYPE(gui_multi_level_t, child_obj)->hidden == 0)
                {
                    GUI_TYPE(gui_multi_level_t, child_obj)->hidden = 1;
                }
            }
        }
    }

    return NULL;
}
#define MULTI_LEVEL_GROUP_DEFAULT "_MULTI_LEVEL_GROUP_DEFAULT"

static void jump(gui_multi_level_t  *this, int level, int index)
{
    gui_obj_t *o = GUI_BASE(this);
    gui_obj_t *root = GUI_BASE(this);
    if (this->group_name == 0)
    {
        this->group_name = MULTI_LEVEL_GROUP_DEFAULT;
    }
    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == MULTI_LEVEL)
        {
            if (GUI_TYPE(gui_multi_level_t, o)->group_name == 0)
            {
                GUI_TYPE(gui_multi_level_t, o)->group_name = MULTI_LEVEL_GROUP_DEFAULT;
            }
            if (GUI_STRINGS_EQUAL(this->group_name, GUI_TYPE(gui_multi_level_t, o)->group_name))
            {
                root = o;
            }
        }
    }
    gui_multi_level_t *rst = 0;
    tree_child_free(root);
    rst = (gui_multi_level_t *)gui_obj_get_by_level_order_hide(root, ++level, ++index);
    if (rst)
    {
        rst->ui_design((void *)rst);
    }


}
static void get_group_root(gui_obj_t *screen, const char *group_name, gui_obj_t **output)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &screen->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, screen);
            gui_log("@name:%s, @type:%d\n", screen->name, screen->type);
            *output = 0;
            return;
        }
        //gui_log("type%d\n", obj->type);
        //if (obj->type == MULTI_LEVEL)gui_log("group_name%s", GUI_TYPE(gui_multi_level_t, obj)->group_name);
        if (obj->type == MULTI_LEVEL &&
            ((strlen(group_name) == strlen(GUI_TYPE(gui_multi_level_t, obj)->group_name)) &&
             (strcmp(group_name, (GUI_TYPE(gui_multi_level_t, obj)->group_name)) == 0)))
        {
            *output = obj;
            GUI_WIDGET_TRY_EXCEPT(obj)
            return;
        }

        get_group_root(obj, group_name, output);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_multi_level_t *gui_multi_level_create(void *parent, const char *widget_name,
                                          void (*ui_design)(gui_obj_t *))
{
#define _GUI_NEW_GUI_MULTI_LEVEL_PARAM this, parent, widget_name, ui_design
    GUI_CREATE_HELPER(gui_multi_level_t, ctor, _GUI_NEW_GUI_MULTI_LEVEL_PARAM)

    gui_obj_t *o = GUI_BASE(this);
    gui_obj_t *root = GUI_BASE(this);
    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == MULTI_LEVEL)
        {
            root = o;
        }
    }
    if (root != GUI_BASE(this))
    {
        this->hidden = 1;
    }
    if (!this->hidden)
    {
        ui_design(GUI_BASE(this));
    }
    this->ui_design = ui_design;
    this->group_name = MULTI_LEVEL_GROUP_DEFAULT;
    return this;
}
gui_multi_level_t *gui_multi_level_create_group(void *parent, const char *widget_name,
                                                void (*ui_design)(gui_obj_t *), const char *group_name)
{
#define _GUI_NEW_GUI_MULTI_LEVEL_PARAM this, parent, widget_name, ui_design
    GUI_CREATE_HELPER(gui_multi_level_t, ctor, _GUI_NEW_GUI_MULTI_LEVEL_PARAM)

    gui_obj_t *o = GUI_BASE(this);
    gui_obj_t *root = GUI_BASE(this);

    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == MULTI_LEVEL)
        {
            root = o;
        }
    }
    if (root != GUI_BASE(this))
    {
        this->hidden = 1;
    }
    this->group_name = MULTI_LEVEL_GROUP_DEFAULT;
    if (group_name && strlen(group_name) > 0)
    {
        this->group_name = group_name;
    }
    if (!this->hidden)
    {
        ui_design(GUI_BASE(this));
    }
    this->ui_design = ui_design;


    return this;
}
_gui_api_gui_multi_level_t _gui_api_for_gui_multi_level_t =
{
    .jump = jump,
};
void gui_multi_level_jump(gui_multi_level_t  *this, int level, int index)
{
    GUI_API(gui_multi_level_t).jump(this, level, index);
}
#include "gui_app.h"
gui_multi_level_t *gui_multi_level_get_group(const char *group_name)
{
    gui_app_t *app = gui_current_app();
    gui_multi_level_t *rst = 0;
    if (app && (&(app->screen)))
    {
        get_group_root(&(app->screen), group_name, (void *)&rst);
    }
    return rst;
}
