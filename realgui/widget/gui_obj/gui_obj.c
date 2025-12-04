/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.c
  * @brief  object widget
  * @details create a object
  * @author luke_sun@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
#include "gui_matrix.h"
#include "gui_api.h"
#include "def_type.h"

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
static gui_matrix_t matrix =
{
    .m = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }
};
static gui_obj_t root =
{
    .name = "screen",
    .parent = NULL,
    .x = 0,
    .y = 0,
    .opacity_value = UINT8_MAX,
    .magic = GUI_MAGIC_NUMBER,
    .child_list = {&(root.child_list), &(root.child_list)},
    .brother_list = {&(root.brother_list), &(root.brother_list)},
    .create_done = true,
    .matrix = &matrix,
};

static gui_obj_t fake_root =
{
    .name = "fake_root",
    .parent = NULL,
    .x = 0,
    .y = 0,
    .opacity_value = UINT8_MAX,
    .magic = GUI_MAGIC_NUMBER,
    .child_list = {&(fake_root.child_list), &(fake_root.child_list)},
    .brother_list = {&(fake_root.brother_list), &(fake_root.brother_list)},
    .create_done = true,
    .matrix = &matrix,
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


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

/*============================================================================*
*                           Public Functions
*============================================================================*/

gui_obj_t *gui_obj_get_root(void)
{
    return &root;
}

gui_obj_t *gui_obj_get_fake_root(void)
{
    return &fake_root;
}

void gui_obj_ctor(gui_obj_t  *_this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    _this->parent = parent;

    if (!name)
    {
        name = "_default_widget";
    }

    _this->name = name;
    _this->x = x;
    _this->y = y;

    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }

    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }

    _this->w = w;
    _this->h = h;
    _this->opacity_value = UINT8_MAX;
    _this->matrix = gui_malloc(sizeof(struct gui_matrix));

    matrix_identity(_this->matrix);
    _this->magic = GUI_MAGIC_NUMBER;
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




void gui_obj_show(void *obj, bool enable)
{
    if (obj != NULL)
    {
        GET_BASE(obj)->not_show = !enable;
    }
}

bool gui_obj_out_screen(gui_obj_t *obj)
{
    float m00 = obj->matrix->m[0][0];
    float m01 = obj->matrix->m[0][1];
    float m02 = obj->matrix->m[0][2];
    float m10 = obj->matrix->m[1][0];
    float m11 = obj->matrix->m[1][1];
    float m12 = obj->matrix->m[1][2];
    float m20 = obj->matrix->m[2][0];
    float m21 = obj->matrix->m[2][1];
    float m22 = obj->matrix->m[2][2];

    if ((m01 == 0)
        && (m10 == 0)
        && (m20 == 0)
        && (m21 == 0)
        && (m22 == 1)) //scale and translate, no rotate
    {
        float x_min = m02;
        float x_max = m02 + m00 * obj->w;
        float y_min = m12;
        float y_max = m12 + m11 * obj->h;

        if (y_min > y_max)
        {
            float temp = y_min;
            y_min = y_max;
            y_max = temp;
        }

        if ((x_min > (int)gui_get_screen_width())
            || (x_max < 0)
            || (y_min > (int)gui_get_screen_height())
            || (y_max < 0))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Calculate the clipping rectangle of a GUI object relative to its top-level ancestor.
 *
 * @param obj The GUI object for which the clipping rectangle is calculated.
 * @param rect The output rectangle that will contain the calculated clipping area.
 */
void gui_obj_get_clip_rect(gui_obj_t *obj, gui_rect_t *rect)
{
#define MAX_OBJ_LIST 20
    gui_obj_t *obj_list[MAX_OBJ_LIST] = {NULL};
    gui_rect_t rect_list[MAX_OBJ_LIST];
    gui_rect_t clip_rect_list[MAX_OBJ_LIST];

    uint32_t i = 0;
    uint32_t j = 0;
    for (gui_obj_t *o = obj; o->parent != NULL; o = o->parent)
    {
        obj_list[i++] = o;
    }

    for (uint32_t k = 0; k < i / 2; k++)
    {
        gui_obj_t *temp = obj_list[k];
        obj_list[k] = obj_list[i - 1 - k];
        obj_list[i - 1 - k] = temp;
    }


    rect_list[0].x1 = obj_list[0]->x;
    rect_list[0].y1 = obj_list[0]->y;
    rect_list[0].x2 = obj_list[0]->x + obj_list[0]->w - 1;
    rect_list[0].y2 = obj_list[0]->y + obj_list[0]->h - 1;

    clip_rect_list[0].x1 = obj_list[0]->x;
    clip_rect_list[0].y1 = obj_list[0]->y;
    clip_rect_list[0].x2 = obj_list[0]->x + obj_list[0]->w - 1;
    clip_rect_list[0].y2 = obj_list[0]->y + obj_list[0]->h - 1;

    for (j = 1; j < i; j++)
    {
        rect_list[j].x1 = obj_list[j]->x + rect_list[j - 1].x1;
        rect_list[j].y1 = obj_list[j]->y + rect_list[j - 1].y1;
        rect_list[j].x2 = obj_list[j]->x + rect_list[j - 1].x1 + obj_list[j]->w - 1;
        rect_list[j].y2 = obj_list[j]->y + rect_list[j - 1].y1 + obj_list[j]->h - 1;

        clip_rect_list[j].x1 = _UI_MAX(rect_list[j - 1].x1, rect_list[j].x1);
        clip_rect_list[j].y1 = _UI_MAX(rect_list[j - 1].y1, rect_list[j].y1);
        clip_rect_list[j].x2 = _UI_MIN(rect_list[j - 1].x2, rect_list[j].x2);
        clip_rect_list[j].y2 = _UI_MIN(rect_list[j - 1].y2, rect_list[j].y2);
    }
    j  = j - 1;
    rect->x1 = clip_rect_list[j].x1 - rect_list[j].x1;
    rect->y1 = clip_rect_list[j].y1 - rect_list[j].y1;
    int16_t w = clip_rect_list[j].x2 - clip_rect_list[j].x1;
    int16_t h = clip_rect_list[j].y2 - clip_rect_list[j].y1;
    if (w < 0)
    {
        w = 0;
    }
    if (h < 0)
    {
        h = 0;
    }
    rect->x2 = rect->x1 + w;
    rect->y2 = rect->y1 + h;

}

void gui_obj_get_area(gui_obj_t *obj,
                      int16_t   *x,
                      int16_t   *y,
                      int16_t   *w,
                      int16_t   *h)
{
    gui_point3f_t p[4] =
    {
        {{0, 0, 1}},
        {{obj->w - 1, 0, 1}},
        {{0, obj->h - 1, 1}},
        {{obj->w - 1, obj->h - 1, 1}}
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

    if ((m_x < x + w) && (m_x + m_w > x) &&
        (m_y < y + h) && (m_y + m_h > y))
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

bool gui_obj_point_in_obj_circle(gui_obj_t *obj, int16_t x, int16_t y)
{
    int16_t m_x, m_y, m_w, m_h;//caculate by obj matrix

    gui_obj_get_area(obj, &m_x, &m_y, &m_w, &m_h);
    int circle_w = (float)m_w / 1.41421356f;
    int circle_h = (float)m_h / 1.41421356f;
    //gui_log("%d,%d,%d,%d\n",m_x, m_y, m_w, circle_w);
    m_x += (m_w - circle_w) / 2;
    m_y += (m_h - circle_h) / 2;
    m_w = circle_w;
    m_h = circle_h;
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
    GUI_ASSERT(obj != NULL);
    GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER));
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
// gui_obj_skip_* functions are implemented in gui_obj_suppress.c

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


void gui_obj_hidden(gui_obj_t *obj, bool hidden)
{
    GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER));
    obj->not_show = hidden;
}

#define MAX_WIDGET_NAME_LENGTH 128
const char *gui_widget_name(gui_obj_t *widget, const char *name)
{
    GUI_ASSERT((GUI_BASE(widget)->magic == GUI_MAGIC_NUMBER));
    if (is_string(name, MAX_WIDGET_NAME_LENGTH + 1))
    {
        widget->name = name;
    }
    return widget->name;

}

void gui_set_location(gui_obj_t *obj, uint16_t x, uint16_t y)
{
    GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER));
    obj->x = x;
    obj->y = y;
}
void gui_obj_move(gui_obj_t *obj, int x, int y)
{
    GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER));
    obj->x = x;
    obj->y = y;
}






