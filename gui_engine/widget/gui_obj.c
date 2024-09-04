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

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** @brief  ... */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_obj_destory_cb(gui_obj_t *obj)
{
    gui_log("do obj %s free(destory), line = %d\n", obj->name, __LINE__);

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
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
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

static void gui_obj_tree_child_show(gui_obj_t *obj, bool enable)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
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

void gui_obj_tree_print(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
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
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
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
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return 0;
        }
        if (obj->type == child_type)
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
        if (o == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, obj);
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
        if (o == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, obj);
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
        if (o == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, obj);
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
        if (o == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, obj);
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
        if (o == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, obj);
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
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        if ((strlen(name) == strlen(obj->name)) && (strcmp(name, obj->name) == 0))
        {
            *output = obj;
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
        if (obj == 0)
        {
            gui_log("list NULL @line:%d, @%x", __LINE__, root);
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
void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj)
{
    uint32_t cur_time_gap;
    if (animate && animate->animate)
    {
        animate->Beginning_frame = 0;
        animate->end_frame = 0;
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
            animate->callback(animate->p, obj);

        }
        else if (animate->repeat_count == -1)
        {
            // uint32_t  round_count = cur_time_gap / animate->dur;


            animate->progress_percent = (float)(cur_time_gap % animate->dur) /
                                        (float)animate->dur;
            if (animate->progress_percent < animate->last_per)
            {

                animate->Beginning_frame = 1;
            }
            animate->last_per = animate->progress_percent;
            animate->callback(animate->p, obj);
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
            animate->callback(animate->p, obj);
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
    animate->callback = (void (*)(void *, void *))callback;
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
            if (child_obj == 0)
            {
                gui_log("list NULL @line:%d, @%x", __LINE__, root);
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
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
