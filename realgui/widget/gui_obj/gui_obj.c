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
/**
 * Count the number of widgets of a specified type in the tree
 * @param node Current node
 * @param target_type The type of widgets to count
 * @return Total number of matching widgets
 */
static int count_widgets(gui_obj_t *node, T_OBJ_TYPE target_type)
{
    int count = 0;
    gui_list_t *node_ptr = NULL;
    gui_list_t *tmp = NULL;

    gui_list_for_each_safe(node_ptr, tmp, &node->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node_ptr, gui_obj_t, brother_list);

        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("Invalid object @line:%d, @%p", __LINE__, node);
            gui_log("@name:%s, @type:%d\n", node->name, node->type);
            continue;
        }

        if (target_type == obj->type)
        {
            count++;
        }

        count += count_widgets(obj, target_type);
    }
    return count;
}
/**
 * Fill the array with pointers to matching widgets
 * @param node Current node
 * @param target_type The type of widgets to find
 * @param array Target array to fill
 * @param index Pointer to the current fill position
 */
static void fill_widgets(gui_obj_t *node, T_OBJ_TYPE target_type,
                         gui_obj_t **array, int *index)
{
    gui_list_t *node_ptr = NULL;
    gui_list_t *tmp = NULL;

    gui_list_for_each_safe(node_ptr, tmp, &node->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node_ptr, gui_obj_t, brother_list);

        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            continue;
        }

        if (target_type == obj->type)
        {
            array[*index] = obj;
            (*index)++;
        }

        fill_widgets(obj, target_type, array, index);
    }
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




void gui_obj_show(void *obj, bool enable)
{
    if (obj != NULL)
    {
        GET_BASE(obj)->not_show = !enable;
    }
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
    GUI_WIDGET_TRY_EXCEPT(obj)
    obj->not_show = hidden;
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
void gui_obj_move(gui_obj_t *obj, int x, int y)
{
    GUI_WIDGET_TRY_EXCEPT(obj) // cppcheck-suppress unknownMacro
    obj->x = x;
    obj->y = y;
}

/**
 * Get an array of pointers to all widgets of a specified type in the GUI object tree (two-phase method)
 * @param root The root node
 * @param type The type of widgets to find
 * @param output_array Pointer to the array of widget pointers, should be NULL initially
 * @param length Length of the array, should be 0 initially, returns the number of widgets found
 * @return 0 on success, -1 on failure (e.g., memory allocation failure or invalid parameters)
 */
int gui_obj_tree_get_widget_array_by_type(gui_obj_t *root,
                                          T_OBJ_TYPE type,
                                          gui_obj_t ***output_array,
                                          int *length)
{
    // Check input parameters
    if (root == NULL || output_array == NULL || length == NULL)
    {
        gui_log("Invalid input parameters\n");
        return -1;
    }

    // Step 1: Count the total number of matching widgets
    int total_count = count_widgets(root, type);

    // If no matches are found, return immediately
    if (total_count == 0)
    {
        *output_array = NULL;
        *length = 0;
        return 0;
    }

    // Step 2: Allocate memory for the array in one go
    gui_obj_t **new_array = (gui_obj_t **)gui_malloc(total_count * sizeof(gui_obj_t *));
    if (new_array == NULL)
    {
        gui_log("Memory allocation failed for %d widgets\n", total_count);
        *output_array = NULL;
        *length = 0;
        return -1;
    }

    // Step 3: Fill the array with widget pointers
    int index = 0;
    fill_widgets(root, type, new_array, &index);

    // Set output parameters
    *output_array = new_array;
    *length = total_count;

    return 0;
}




