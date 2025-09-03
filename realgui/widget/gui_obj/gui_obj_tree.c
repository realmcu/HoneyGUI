/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_tree.c
  * @brief  object tree operations
  * @details tree related functions and structures
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
#include "gui_obj_tree.h"
#include "gui_api.h"
#include "gui_message.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define ENUM_TO_STRING(enum_value) case enum_value: return #enum_value;

/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_obj_destroy_cb(gui_obj_t *obj)
{
    //gui_log("do obj %s free(destroy), line = %d\n", obj->name, __LINE__);

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

    if (obj->suppress_conflict_obj_list != NULL)
    {
        gui_free(obj->suppress_conflict_obj_list);
    }

    if (obj->timer != NULL)
    {
        gui_free(obj->timer);
    }
}

static void gui_obj_tree_child_free(gui_obj_t *object)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;

    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }

        gui_obj_tree_child_free(obj);
        gui_obj_destroy_cb(obj);

        gui_free(obj);
    }
}

static void gui_obj_tree_child_show(gui_obj_t *obj, bool enable)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        obj->not_show = !enable;
        gui_obj_tree_child_show(obj, enable);
    }
}

#ifdef _WIN32
static const char *get_type_name(T_OBJ_TYPE type)
{
    switch (type)
    {
        ENUM_TO_STRING(SCREEN)
        ENUM_TO_STRING(WINDOW)
        ENUM_TO_STRING(TABVIEW)
        ENUM_TO_STRING(TAB)
        ENUM_TO_STRING(CURTAINVIEW)
        ENUM_TO_STRING(CURTAIN)
        ENUM_TO_STRING(IMAGE)
        ENUM_TO_STRING(BUTTON)
        ENUM_TO_STRING(ICONLIST)
        ENUM_TO_STRING(ICON)
        ENUM_TO_STRING(IMAGE_FROM_MEM)
        ENUM_TO_STRING(TEXTBOX)
        ENUM_TO_STRING(SCROLLTEXTBOX)
        ENUM_TO_STRING(SEEKBAR)
        ENUM_TO_STRING(PROGRESSBAR)
        ENUM_TO_STRING(CLICKSWITCH)
        ENUM_TO_STRING(PAGE)
        ENUM_TO_STRING(SCROLL_WHEEL)
        ENUM_TO_STRING(PAGEBAR)
        ENUM_TO_STRING(RETURNWIDGET)
        ENUM_TO_STRING(RECTANGLE)
        ENUM_TO_STRING(WINDOWWITHBORDER)
        ENUM_TO_STRING(CANVAS)
        ENUM_TO_STRING(VG_LITE_CLOCK)
        ENUM_TO_STRING(CUBE)
        ENUM_TO_STRING(GRID)
        ENUM_TO_STRING(RADIO)
        ENUM_TO_STRING(RADIOSWITCH)
        ENUM_TO_STRING(MACRO_CANVAS_ARC)
        ENUM_TO_STRING(JAVASCRIPT)
        ENUM_TO_STRING(MOVIE)
        ENUM_TO_STRING(IMAGE_SCOPE)
        ENUM_TO_STRING(CARDVIEW)
        ENUM_TO_STRING(CARD)
        ENUM_TO_STRING(PAGELIST)
        ENUM_TO_STRING(PAGELISTVIEW)
        ENUM_TO_STRING(MACRO_ANIMATETRANSFORM)
        ENUM_TO_STRING(HONEYCOMB_LIST)
        ENUM_TO_STRING(WHEEL_LIST)
        ENUM_TO_STRING(QRCODE)
        ENUM_TO_STRING(GALLERY)
        ENUM_TO_STRING(TURN_TABLE)
        ENUM_TO_STRING(KEYBOARD)
        ENUM_TO_STRING(MACRO_MOTORIZED_CURTAIN)
        ENUM_TO_STRING(MULTI_LEVEL)
        ENUM_TO_STRING(U8G2)
        ENUM_TO_STRING(MACRO_ONCLICK)
        ENUM_TO_STRING(MACRO_BACKICON)
        ENUM_TO_STRING(CANVAS_IMG)
        ENUM_TO_STRING(VG_LITE_SOCCER)
        ENUM_TO_STRING(SLIDER)
        ENUM_TO_STRING(MACRO_ONCHANGE)
        ENUM_TO_STRING(TYPE_SCROLL_WHEEL_NEW)
        ENUM_TO_STRING(MACRO_ONON)
        ENUM_TO_STRING(MACRO_ONOFF)
        ENUM_TO_STRING(MACRO_ONLOAD)
    default: return "UNKNOWN_TYPE";
    }
}
#endif

#ifdef _WIN32
static void sanitize_identifier(char *id)
{
    for (char *p = id; *p; p++)
    {
        if (*p == ' ')
        {
            *p = '_';
        }
    }
}
#endif

#ifdef _WIN32
static void internal_gui_obj_tree_print_mmd(FILE *file, gui_obj_t *obj)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;

    // Traverse the child list of the current object
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *child = gui_list_entry(node, gui_obj_t, brother_list);

        // Use unique identifier by combining obj pointer with type and name
        char parent_id[256], child_id[256];
        snprintf(parent_id, sizeof(parent_id), "%s:%s_%p",
                 get_type_name(obj->type), obj->name, (void *)obj);
        sanitize_identifier(parent_id);
        snprintf(child_id, sizeof(child_id), "%s:%s_%p",
                 get_type_name(child->type), child->name, (void *)child);
        sanitize_identifier(child_id);

        // Print the relationship in Mermaid syntax to the file
        fprintf(file, " %s --> %s;\n", parent_id, child_id);

        // Recursively print the subtree of the child
        internal_gui_obj_tree_print_mmd(file, child);
    }
}
#endif

static void obj_tree_get_widget_by_type_and_index(gui_obj_t *root,
                                                  T_OBJ_TYPE type, gui_obj_t **output, int index, int *count, int *flag)
{
    if (*flag)
    {
        return;
    }

    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &root->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, root);
            gui_log("@name:%s, @type:%d\n", root->name, root->type);
            return;
        }
        if (type == obj->type)
        {
            if (index == *count)
            {
                *output = obj;
                *flag = 1;
                return;
            }
            (*count)++;
            //gui_log("%d,%d\n", *count, index);
        }

        obj_tree_get_widget_by_type_and_index(obj, type, output, index, count, flag);
    }
}


/*============================================================================*
*                           Public Functions
*============================================================================*/
void gui_obj_child_free(gui_obj_t *object)
{
    gui_obj_tree_child_free(object);
    gui_list_init(&object->child_list);
}


void gui_obj_tree_free(void *obj)
{
    gui_obj_t *object = (gui_obj_t *)obj;

    gui_obj_tree_child_free(object);

    if (object->parent)
    {
        gui_list_remove(&object->brother_list);
        gui_obj_destroy_cb(obj);

        gui_free(obj);
    }
    else
    {
        gui_log("obj %s parent equal NULL!\n", object->name);
        if (object->matrix != NULL)
        {
            gui_free(object->matrix);
            object->matrix = NULL;
        }
        if (object->event_dsc != NULL)
        {
            gui_free(object->event_dsc);
            object->event_dsc = NULL;
        }
        if (object->suppress_conflict_obj_list != NULL)
        {
            gui_free(object->suppress_conflict_obj_list);
            object->suppress_conflict_obj_list = NULL;
        }
        if (object->timer != NULL)
        {
            gui_free(object->timer);
            object->timer = NULL;
        }
        gui_list_init(&object->child_list);
    }
}

static void __obj_tree_free_async(void *msg)
{
    gui_obj_t *obj = (gui_obj_t *)((gui_msg_t *)msg)->payload;

    gui_log("%s asynchronous free!\n", obj->name);
    gui_obj_tree_free(obj);
}

void gui_obj_tree_free_async(void *obj)
{
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = __obj_tree_free_async,
        .payload = obj,
    };
    gui_send_msg_to_server(&msg);
}

void gui_obj_tree_show(gui_obj_t *obj, bool enable)
{
    gui_obj_tree_child_show(obj, enable);
    GET_BASE(obj)->not_show = !enable;
}

gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj)
{
    gui_obj_t *child = obj;

    while (true)
    {
        if (child->parent == NULL)
        {
            gui_log("gui_obj_tree_get_root = %s\n", child->name);
            return child;
        }
        else
        {
            child = child->parent;
        }
    }
}

gui_obj_t *gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic == GUI_MAGIC_NUMBER && obj->type == child_type)
        {
            return obj;
        }
    }

    return NULL;
}

void gui_obj_tree_print(gui_obj_t *obj)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        gui_log(" %s-->%s \n", obj->name, obj->parent->name);
        gui_obj_tree_print(obj);
    }
}

void gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        if (obj->type == type)
        {
            (*count)++;
        }

        gui_obj_tree_count_by_type(obj, type, count);
    }
}

void gui_obj_tree_print_mmd(gui_obj_t *obj)
{
#ifdef _WIN32
    // Open the file for writing
    FILE *file = fopen("realgui/example/web/tree/diagram.mmd", "w");
    if (file == NULL)
    {
        perror("Failed to open file");
        return;
    }

    // Start of the Mermaid code output
    fprintf(file, "graph LR;\n");

    // Print the object tree into the file
    internal_gui_obj_tree_print_mmd(file, obj);

    // Close the file
    fclose(file);
#endif
}

void gui_obj_tree_get_widget_by_name(gui_obj_t *object, const char *name, gui_obj_t **output)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        if ((strlen(name) == strlen(obj->name)) && (strcmp(name, obj->name) == 0))
        {
            *output = obj;
            GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER));
            return;
        }

        gui_obj_tree_get_widget_by_name(obj, name, output);
    }
}

void gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output)
{
    gui_node_list_t *node = NULL;
    gui_node_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &root->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, root);
            gui_log("@name:%s, @type:%d\n", root->name, root->type);
            return;
        }
        if (type == obj->type)
        {
            *output = obj;
            return;
        }

        gui_obj_tree_get_widget_by_type(obj, type, output);
    }
}

void gui_obj_tree_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output,
                                               int index)
{
    int count = 0;
    int flag = 0;
    obj_tree_get_widget_by_type_and_index(root, type, output, index, &count, &flag);
}

void gui_obj_tree_print_bfs(gui_obj_t *root)
{
    if (root == NULL)
    {
        return;
    }

    gui_obj_t *queue[100];
    int front = 0;
    int rear = 0;

    queue[rear++] = root;

    while (front != rear)
    {
        gui_obj_t *current_obj = queue[front++];
        gui_log(" %s \n", current_obj->name);
        gui_node_list_t *node = NULL;
        gui_node_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &current_obj->child_list)
        {
            gui_obj_t *child_obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (child_obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            queue[rear++] = child_obj;
        }
    }
}
