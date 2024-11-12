/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.c
  * @brief  object widget
  * @details create a object
  * @author luke_sun@realsil.com.cn
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
#include <stdio.h>
#include "gui_obj.h"
#include "gui_matrix.h"


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

static void gui_obj_destory_cb(gui_obj_t *obj)
{
    //gui_log("do obj %s free(destory), line = %d\n", obj->name, __LINE__);

    if (obj->matrix != NULL)
    {
        gui_free(obj->matrix);
    }

    if (obj->has_destroy_cb)
    {
        obj->obj_cb(obj, OBJ_DESTORY);
    }
}

static void gui_obj_tree_child_free(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;

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
        gui_obj_destory_cb(obj);

        if (obj->event_dsc != NULL)
        {
            gui_free(obj->event_dsc);
        }

        gui_free(obj);
    }
}
void gui_obj_child_free(gui_obj_t *object)
{
    gui_obj_tree_child_free(object);
    gui_list_init(&object->child_list);
}
static void gui_obj_tree_child_show(gui_obj_t *obj, bool enable)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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

/*============================================================================*
*                           Public Functions
*============================================================================*/
void gui_obj_ctor(gui_obj_t  *this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h)
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

    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    this->w = w;
    this->h = h;
    this->opacity_value = UINT8_MAX;
    this->matrix = gui_malloc(sizeof(struct gui_matrix));

    matrix_identity(this->matrix);
    this->magic = GUI_MAGIC_NUMBER;
}

gui_obj_t *gui_obj_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h)
{
    gui_obj_t *this = gui_malloc(sizeof(gui_obj_t));
    memset(this, 0, sizeof(gui_obj_t));
    gui_obj_ctor(this, parent, name, x, y, w, h);

    gui_list_init(&(this->child_list));
    if ((this->parent) != ((void *)0))
    {
        gui_list_insert_before(&(this->parent->child_list), &(this->brother_list));
    }
    this->create_done = true;

    return this;
}

uint8_t gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len)
{
    uint8_t crc = seed;

    for (uint8_t i = 0; i < len; i++)
    {
        crc += *(data++);
    }

    return crc;
}

void gui_obj_add_event_cb(void           *obj,
                          gui_event_cb_t  event_cb,
                          gui_event_t     filter,
                          void           *user_data)
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

void gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code)
{
    if (!obj->gesture)
    {
        for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
        {
            gui_event_dsc_t *event_dsc = obj->event_dsc + i;
            event_dsc->event_code = event_code;
        }
    }
}
#define ENUM_TO_STRING(enum_value) case enum_value: return #enum_value;
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
#if _WIN32
static void internal_gui_obj_tree_print_mmd(FILE *file, gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;

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
void gui_obj_tree_print_mmd(gui_obj_t *obj)
{
#if _WIN32
    // Open the file for writing
    FILE *file = fopen("gui_engine/example/web/tree/diagram.mmd", "w");
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

void gui_obj_tree_print(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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
void gui_obj_tree_free(void *obj)
{
    //GUI_WIDGET_TRY_EXCEPT(obj)
    gui_obj_t *object = (gui_obj_t *)obj;

    gui_obj_tree_child_free(object);

    if (object->parent)
    {
        gui_list_remove(&object->brother_list);
        gui_obj_destory_cb(obj);

        if (object->event_dsc != NULL)
        {
            gui_free(object->event_dsc);
        }

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
        gui_list_init(&object->child_list);
    }
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

void gui_obj_show(void *obj, bool enable)
{
    if (obj != NULL)
    {
        GET_BASE(obj)->not_show = !enable;
    }
}

gui_obj_t *gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = obj;
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

void gui_obj_get_area(gui_obj_t *obj,
                      int16_t   *x,
                      int16_t   *y,
                      int16_t   *w,
                      int16_t   *h)
{
    gui_point3f_t p[4] =
    {
        {0, 0, 1},
        {obj->w - 1, 0, 1},
        {0, obj->h - 1, 1},
        {obj->w - 1, obj->h - 1, 1}
    };
    float x_min = 0.0f, x_max = 0.0f, y_min = 0.0f, y_max = 0.0f;

    matrix_multiply_point(obj->matrix, p);
    x_min = p->p[0];
    x_max = p->p[0];
    y_min = p->p[1];
    y_max = p->p[1];

    matrix_multiply_point(obj->matrix, p + 1);
    matrix_multiply_point(obj->matrix, p + 2);
    matrix_multiply_point(obj->matrix, p + 3);

    for (uint8_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;

        if (point->p[0] < x_min)
        {
            x_min = point->p[0];
        }

        if (point->p[0] > x_max)
        {
            x_max = point->p[0];
        }

        if (point->p[1] < y_min)
        {
            y_min = point->p[1];
        }

        if (point->p[1] > y_max)
        {
            y_max = point->p[1];
        }
    }

    *x = (int16_t)x_min;
    *y = (int16_t)y_min;
    *w = (int16_t)(x_max - x_min) + 1;
    *h = (int16_t)(y_max - y_min) + 1;
}

bool gui_obj_in_rect(gui_obj_t *obj,
                     int16_t    x,
                     int16_t    y,
                     int16_t    w,
                     int16_t    h)
{
    int16_t m_x, m_y, m_w, m_h;//caculate by obj matrix

    gui_obj_get_area(obj, &m_x, &m_y, &m_w, &m_h);

    if ((m_x >= x)
        && ((m_x + m_w) <= (x + w))
        && (m_y >= y)
        && ((m_y + m_h) <= (y + h)))
    {
        return true;
    }

    return false;
}

bool gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y)
{
    int16_t m_x, m_y, m_w, m_h;//caculate by obj matrix

    gui_obj_get_area(obj, &m_x, &m_y, &m_w, &m_h);

    if ((m_x < x)
        && ((m_x + m_w) > x)
        && (m_y < y)
        && ((m_y + m_h) > y))
    {
        return true;
    }

    return false;
}
void gui_obj_absolute_xy(gui_obj_t *obj, int *absolute_x, int *absolute_y)
{
    GUI_WIDGET_TRY_EXCEPT(obj)
    GUI_ASSERT(absolute_x) // cppcheck-suppress unknownMacro
    GUI_ASSERT(absolute_y)
    gui_obj_t *o = obj;
    *absolute_x = o->x;
    *absolute_y = o->y;
    while (o->parent != NULL)
    {
        o = o->parent;
        *absolute_x += o->x;
        *absolute_y += o->y;
    }
}
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

gui_obj_t *gui_get_root(gui_obj_t *object)
{
    gui_obj_t *o = object;

    while (o->parent != NULL)
    {
        o = o->parent;
    }
    if (o->type == SCREEN)
    {
        return o;
    }
    return 0;

}
void gui_obj_tree_get_widget_by_name(gui_obj_t *object, const char *name, gui_obj_t **output)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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
            GUI_WIDGET_TRY_EXCEPT(obj)
            return;
        }

        gui_obj_tree_get_widget_by_name(obj, name, output);
    }
}
void gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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
static void obj_tree_get_widget_by_type_and_index(gui_obj_t *root,
                                                  T_OBJ_TYPE type, gui_obj_t **output, int index, int *count, int *flag)
{
    if (*flag)
    {
        return;
    }

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
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
void gui_obj_tree_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output,
                                               int index)
{
    int count = 0;
    int flag = 0;
    obj_tree_get_widget_by_type_and_index(root, type, output, index, &count, &flag);
}
void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj)
{
    uint32_t cur_time_gap;
    if (animate && animate->animate)
    {
        animate->Beginning_frame = 0;
        animate->end_frame = 0;
        if (animate->progress_percent == 0.0f)
        {
            animate->Beginning_frame = 1;
        }
        if (animate->progress_percent == 0 && !animate->init)
        {
            animate->init = 1;
            animate->init_time_ms = gui_ms_get();
        }

        animate->cur_time_ms = gui_ms_get();
        cur_time_gap = animate->cur_time_ms - animate->init_time_ms;

        if (animate->repeat_count == 0)
        {
            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            if (cur_time_gap / animate->dur >= 1)
            {
                animate->end_frame = 1;
                animate->progress_percent = 1;
                animate->animate = 0;
            }
            animate->callback(animate->p, obj, animate);

        }
        else if (animate->repeat_count == -1)
        {
            // uint32_t  round_count = cur_time_gap / animate->dur;


            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            if (animate->progress_percent < animate->last_per)
            {

                animate->Beginning_frame = 1;
                animate->end_frame = 1;
            }


            animate->last_per = animate->progress_percent;
            animate->callback(animate->p, obj, animate);
        }
        else
        {
            uint32_t  round_count = cur_time_gap / animate->dur;
            if (round_count > animate->repeat_count)
            {
                animate->animate = 0;
                return;
            }

            if (round_count > animate->last_round)
            {
                animate->Beginning_frame = 1;
            }
            animate->last_round = round_count;
            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            animate->callback(animate->p, obj, animate);
        }
    }
    if (animate && !animate->animate)
    {
        animate->init_time_ms = gui_ms_get();
    }
}
gui_animate_t *gui_obj_set_animate(gui_animate_t *animate,
                                   uint32_t      dur,
                                   int           repeat_count,
                                   void         *callback,
                                   void         *p)
{
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }

    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (gui_animate_callback_t)callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    return animate;
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
        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
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
void gui_obj_hidden(gui_obj_t *obj, bool hidden)
{
    GUI_WIDGET_TRY_EXCEPT(obj)
    obj->not_show = hidden;
}
static size_t safe_strlen(const char *str, size_t max_len)
{
    if (str == NULL) { return 0; }

    size_t len = 0;
    while (len < max_len && str[len] != '\0')
    {
        len++;
    }

    return len;
}

static bool is_string(const char *str, size_t max_len)
{
    if (str == NULL) { return 0; }
    size_t len = safe_strlen(str, max_len);
    if (len < max_len)
    {
        return 1;
    }
    return 0;
}
#define MAX_WIDGET_NAME_LENGTH 128
const char *gui_widget_name(gui_obj_t *widget, const char *name)
{
    GUI_WIDGET_TRY_EXCEPT(widget)
    if (is_string(name, MAX_WIDGET_NAME_LENGTH + 1))
    {
        widget->name = name;
    }
    return widget->name;

}
void gui_update_speed(int *speed, int speed_recode[])
{
    IMPORT_GUI_TOUCHPAD
    int recode_num = 4;
    for (size_t i = 0; i < recode_num; i++)
    {
        speed_recode[i] = speed_recode[i + 1];
    }
    speed_recode[recode_num] = touch->deltaY;
    *speed = speed_recode[recode_num] - speed_recode[0];
    int max_speed = GUI_SPEED_MAX;
    int min_speed = GUI_SPEED_MIN;
    if (*speed > max_speed)
    {
        *speed = max_speed;
    }
    else if (*speed < -max_speed)
    {
        *speed = -max_speed;
    }
    if ((*speed > 0) && (*speed < min_speed))
    {
        *speed = min_speed;
    }
    else if ((*speed < 0) && (*speed > -min_speed))
    {
        *speed = -min_speed;
    }
}
void gui_update_speed_by_displacement(int *speed, int speed_recode[], int displacement)
{
    int recode_num = 4;
    for (size_t i = 0; i < recode_num; i++)
    {
        speed_recode[i] = speed_recode[i + 1];
    }
    speed_recode[recode_num] = displacement;
    *speed = speed_recode[recode_num] - speed_recode[0];
    int max_speed = GUI_SPEED_MAX;
    int min_speed = GUI_SPEED_MIN;
    if (*speed > max_speed)
    {
        *speed = max_speed;
    }
    else if (*speed < -max_speed)
    {
        *speed = -max_speed;
    }
    if ((*speed > 0) && (*speed < min_speed))
    {
        *speed = min_speed;
    }
    else if ((*speed < 0) && (*speed > -min_speed))
    {
        *speed = -min_speed;
    }
}
void gui_inertial(int *speed, int end_speed, int *offset)
{
    if (*speed > end_speed)
    {
        *offset += *speed;
        *speed -= 1;
    }
    else if (*speed < -end_speed)
    {
        *offset += *speed;
        *speed += 1;
    }
}
void gui_set_location(gui_obj_t *obj, uint16_t x, uint16_t y)
{
    GUI_WIDGET_TRY_EXCEPT(obj)
    obj->x = x;
    obj->y = y;
}

