/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_wheel_new.c
  * @brief scroll wheel effect
  * @author triton_yu@realsil.com.cn
  * @date 2024/08/27
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "gui_scroll_wheel_new.h"
#include "gui_text.h"
#include "gui_page.h"
#include "gui_image_array.h"
/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


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

static void **get_image_pointers(const char *input, size_t *num_pointers,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map);
static int get_index(gui_scroll_wheel_new_t *this)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(this, WINDOW)
    return this->index_offset;
}
static int set_index(gui_scroll_wheel_new_t *this, int index)
{
    GUI_WIDGET_TYPE_TRY_EXCEPT(this, WINDOW)
    this->index_offset = index;
}
static void ctor(struct gui_scroll_wheel_new *this,
                 gui_obj_t               *parent,
                 int16_t                  x,
                 int16_t                  y,
                 int16_t                  w,
                 int16_t                  row_gap,
                 int16_t                  row_count,
                 const char             **string_array,
                 int16_t                  string_array_size)
{
    extern void gui_win_ctor(gui_win_t  *this,
                             gui_obj_t  *parent,
                             const char *name,
                             int16_t     x,
                             int16_t     y,
                             int16_t     w,
                             int16_t     h);
    gui_win_ctor(GUI_TYPE(gui_win_t, this), parent, "scroll wheel new", x, y, w, row_gap * row_count);
    gui_win_set_scope(GUI_TYPE(gui_win_t, this), 1);
    this->string_array = string_array;
    this->string_array_size = string_array_size;
    this->gap = row_gap;
    this->count = row_count;
    this->alien = 1;
    this->end_speed = 3;
    this->index_offset = (row_count + 2) / 2;
}
static void text_widget_array_foreach(gui_scroll_wheel_new_t *this, gui_obj_t **text_widget_array,
                                      int array_count)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *obj = GUI_BASE(this->win);
    gui_obj_t *object = obj;
    int count = 0;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (count >= array_count)
        {
            return;
        }

        if (obj->type == WINDOW)
        {
            gui_obj_t *text = gui_list_entry((&obj->child_list)->next, gui_obj_t, brother_list);
            text_widget_array[count] = text;
            count++;
        }
    }
}
static void update_speed(gui_scroll_wheel_new_t *this)
{
    IMPORT_GUI_TOUCHPAD
    int recode_num = 4;
    for (size_t i = 0; i < recode_num; i++)
    {
        this->recode[i] = this->recode[i + 1];
    }
    this->recode[recode_num] = touch->deltaY;
    this->speed = this->recode[recode_num] - this->recode[0];
    int max_speed = GUI_PAGE_MAX_SPEED;
    int min_speed = GUI_PAGE_MIN_SPEED;
    if (this->speed > max_speed)
    {
        this->speed = max_speed;
    }
    else if (this->speed < -max_speed)
    {
        this->speed = -max_speed;
    }
    if ((this->speed > 0) && (this->speed < min_speed))
    {
        this->speed = min_speed;
    }
    else if ((this->speed < 0) && (this->speed > -min_speed))
    {
        this->speed = -min_speed;
    }
}
static void inertial(gui_scroll_wheel_new_t *this)
{
    if (this->speed > this->end_speed)
    {
        this->touch_y += this->speed;
        this->speed -= 1;
    }
    else if (this->speed < -this->end_speed)
    {
        this->touch_y += this->speed;
        this->speed += 1;
    }
}
static void render(const char *text, gui_obj_t *obj, unsigned char render_mode,
                   int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    switch (render_mode)
    {
    case GUI_SCROLL_WHEEL_NEW_RENDER_TEXT:
        {
            gui_text_content_set((void *)obj, (void *)text, strlen(text));
            gui_text_convert_to_img((void *)obj, ARGB8888);
        }
        break;
    case GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY:
        {
            const char *input = text;
            size_t num_pointers;
            void **image_pointers = get_image_pointers(input, &num_pointers, text_image_map_length,
                                                       text_image_map);
            gui_image_array_write((void *)obj, image_pointers, num_pointers);
            // Free allocated memory
            gui_free(image_pointers);
            GUI_TYPE(gui_image_array_t, obj)->text = text;
        }
        break;
    default:
        break;
    }
}
static void override(gui_obj_t *win)
{
    gui_scroll_wheel_new_t *this = GUI_TYPE(gui_scroll_wheel_new_t, win->parent->parent);
    if (this->render_mode != GUI_SCROLL_WHEEL_NEW_RENDER_TEXT &&
        this->render_mode != GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
    {
        bool please_set_scroll_wheel_new_render_mode = 0;
        GUI_ASSERT(please_set_scroll_wheel_new_render_mode);
    }
    IMPORT_GUI_TOUCHPAD
    IMPORT_GUI_WHEEL
    bool wheel_take_over = this->wheel_take_over;
    char time_array_offset = this->time_array_offset;
    int time_array_size = this->string_array_size;
    if (wheel->pressed)
    {
        wheel_take_over = 1;
        wheel->history_x = this->touch_y;
    }
    if (wheel->released)
    {
        wheel_take_over = 0;
    }

    if (!wheel_take_over)
    {
        int ax, ay;
        gui_obj_absolute_xy(win, &ax, &ay);
        if (touch->pressed && touch->x > ax - this->col_offset && touch->x < ax + win->w)
        {
            this->history_y = this->touch_y;
            this->speed = 0;
            memset(this->recode, 0, sizeof(this->recode));
        }
        if (touch->pressing && touch->x > ax - this->col_offset && touch->x < ax + win->w)
        {
            this->touch_y = this->history_y + touch->deltaY;
            /**
             * Index = offset / gap % array length
               WidgetOffset = offset % gap
            */
            update_speed(this);
            this->render = 1;


        }
        else
        {
            inertial(this);
        }
        if (this->render)
        {
            time_array_offset = -(this->touch_y / this->gap % time_array_size);
            int widget_offset = this->touch_y % this->gap;
            GUI_BASE(win)->y = widget_offset;
            if (this->alien && widget_offset == 0 && this->speed <= this->end_speed &&
                this->speed >= -this->end_speed)
            {
                this->render = 0;
            }
            if (this->alien && widget_offset != 0)
            {
                if (this->speed > 0 && this->speed <= this->end_speed)
                {

                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        this->touch_y += this->end_speed;
                    }


                }
                else if (this->speed < 0 && this->speed >= -this->end_speed)
                {

                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        this->touch_y -= this->end_speed;
                    }

                }
                else if (this->speed == 0)
                {
                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > this->gap / 2)
                        {
                            this->touch_y += this->end_speed;
                        }
                        else
                        {
                            this->touch_y -= this->end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > this->gap / 2)
                        {
                            this->touch_y -= this->end_speed;
                        }
                        else
                        {
                            this->touch_y += this->end_speed;
                        }
                    }
                }
            }
            {
                gui_obj_t *text_widget_array[this->count + 2];
                text_widget_array_foreach(this, text_widget_array, this->count + 2);
                for (size_t i = 0; i < this->count + 2; i++)
                {

                    int index = time_array_offset + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = this->string_array[index];
                    if (i == (this->count + 2) / 2)
                    {
                        this->index_offset = index;
                    }
                    render(text, (void *)text_widget_array[i], this->render_mode, this->text_image_map_length,
                           this->text_image_map);
                }
            }

        }
        else
        {
            this->history_y = this->touch_y;
        }
    }
    else
    {
        this->touch_y = wheel->history_y;
        {
            time_array_offset = -(this->touch_y / this->gap % time_array_size);
            int widget_offset = this->touch_y % this->gap;
            GUI_BASE(win)->y = widget_offset;
            if (this->alien && widget_offset == 0 && this->speed <= this->end_speed &&
                this->speed >= -this->end_speed)
            {
                this->render = 0;
            }
            if (this->alien && widget_offset != 0)
            {
                if (this->speed > 0 && this->speed <= this->end_speed)
                {
                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        this->touch_y += this->end_speed;
                    }
                }
                else if (this->speed < 0 && this->speed >= -this->end_speed)
                {
                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    else
                    {
                        this->touch_y -= this->end_speed;
                    }
                }
                else if (this->speed == 0)
                {
                    if (_UI_ABS(widget_offset) <= this->end_speed)
                    {
                        widget_offset = 0;
                    }
                    if (widget_offset > 0)
                    {
                        if (widget_offset > this->gap / 2)
                        {
                            this->touch_y += this->end_speed;
                        }
                        else
                        {
                            this->touch_y -= this->end_speed;
                        }
                    }
                    else if (widget_offset < 0)
                    {
                        if (-widget_offset > this->gap / 2)
                        {
                            this->touch_y -= this->end_speed;
                        }
                        else
                        {
                            this->touch_y += this->end_speed;
                        }
                    }
                }
            }
            {
                gui_obj_t *text_widget_array[this->count + 2];
                text_widget_array_foreach(this, text_widget_array, this->count + 2);
                for (size_t i = 0; i < this->count + 2; i++)
                {

                    int index = time_array_offset + i;
                    if (index >= time_array_size)
                    {
                        index -= time_array_size;
                    }
                    if (index < 0)
                    {
                        index += time_array_size;
                    }
                    const char *text = this->string_array[index];
                    if (i == (this->count + 2) / 2)
                    {
                        this->index_offset = index;
                    }

                    render(text, (void *)text_widget_array[i], this->render_mode, this->text_image_map_length,
                           this->text_image_map);
                }
            }
        }
    }
}
// Function to retrieve image data based on text input
static void *get_image_data(const char *text, int16_t text_image_map_length,
                            const struct gui_text_image_map *text_image_map)
{
    for (size_t i = 0; i < text_image_map_length; ++i)
    {
        if (strcmp(text_image_map[i].text, text) == 0)
        {
            return text_image_map[i].image_data;
        }
    }
    return NULL; // Return NULL if no match is found
}

// Function to process the input string and return image pointers
static void **get_image_pointers(const char *input, size_t *num_pointers,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    size_t capacity = 16; // Start with a reasonable initial capacity
    /*Why Start with a Capacity of 16?
    Balance Between Space and Performance:
    Memory Efficiency: Starting with some preallocated space avoids constant reallocations and copying when adding elements to the array. A small initial allocation (like 16) means you don't waste much memory upfront.
    Performance: Frequent reallocations can be costly, both in terms of time (because data has to be copied to a new location) and CPU usage. Starting with a moderate size helps balance these factors.*/

    *num_pointers = 0;
    void **image_pointers = gui_malloc(capacity * sizeof(void *));
    if (image_pointers == NULL)
    {
        gui_log("Failed to allocate memory");
    }

    const char *ptr = input;
    while (*ptr != '\0')
    {
        const char *next = ptr + 1;

        // Handle multi-character tokens (e.g., "AM" and "PM")
        if (strncmp(ptr, "PM", 2) == 0 || strncmp(ptr, "AM", 2) == 0)
        {
            char token[3] = {ptr[0], ptr[1], '\0'};
            void *img = get_image_data(token, text_image_map_length, text_image_map);
            if (img != NULL)
            {
                if (*num_pointers == capacity)
                {
                    capacity *= 2;
                    image_pointers = gui_realloc(image_pointers, capacity * sizeof(void *));
                    if (image_pointers == NULL)
                    {
                        gui_log("Failed to reallocate memory");
                    }
                }
                image_pointers[(*num_pointers)++] = img;
            }
            next = ptr + 2; // Skip the next character as it's part of "AM" or "PM"
        }
        else
        {
            char token[2] = {ptr[0], '\0'};
            void *img = get_image_data(token, text_image_map_length, text_image_map);
            if (img != NULL)
            {
                if (*num_pointers == capacity)
                {
                    capacity *= 2;
                    image_pointers = gui_realloc(image_pointers, capacity * sizeof(void *));
                    if (image_pointers == NULL)
                    {
                        gui_log("Failed to reallocate memory");
                    }
                }
                image_pointers[(*num_pointers)++] = img;
            }
        }

        ptr = next;
    }

    return image_pointers;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_scroll_wheel_new_render_text(gui_scroll_wheel_new_t *widget, const void *font_file_pointer,
                                      int font_size)
{
    widget->render_mode = GUI_SCROLL_WHEEL_NEW_RENDER_TEXT;
    for (size_t i = 0; i < widget->count + 2; i++)
    {
        gui_win_t *win = gui_win_create(widget->win, 0, 0, widget->gap * i, GUI_BASE(widget)->w,
                                        widget->gap);
        {
            const char *text = widget->string_array[i];
            {
                gui_text_t *t = gui_text_create(win, text, 0, 0, GUI_BASE(widget)->w,
                                                font_size);
                gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
                const void *addr1 = font_file_pointer;
                gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                gui_text_mode_set(t, LEFT);
                gui_text_convert_to_img(t, ARGB8888);
            }
        }
    }
}


void gui_scroll_wheel_new_render_image_array(gui_scroll_wheel_new_t *widget,
                                             const struct gui_text_image_map *map, int map_length)
{
    widget->text_image_map = map;
    widget->text_image_map_length = map_length;
    widget->render_mode = GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY;
    for (size_t i = 0; i < widget->count + 2; i++)
    {
        gui_win_t *win = gui_win_create(widget->win, 0, 0, widget->gap * i, GUI_BASE(widget)->w,
                                        widget->gap);
        {
            const char *text = widget->string_array[i];
            const char *input = text;
            size_t num_pointers;
            void **image_pointers = get_image_pointers(input, &num_pointers, widget->text_image_map_length,
                                                       widget->text_image_map);

            // Print results
            gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
            for (size_t i = 0; i < num_pointers; ++i)
            {
                gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
            }
            gui_image_array_create(win, 0, 0, image_pointers, num_pointers,
                                   "num");
            // Free allocated memory
            gui_free(image_pointers);
        }
    }
}
void gui_scroll_wheel_new_set_col_offset(gui_scroll_wheel_new_t *widget, int offset)
{
    if (offset >= GUI_BASE(widget)->w)
    {
        offset = GUI_BASE(widget)->w - 1;
    }

    widget->col_offset = offset;
    GUI_BASE(widget->win)->x = offset;
    GUI_BASE(widget->win)->w -= offset;

}
gui_scroll_wheel_new_t *gui_scroll_wheel_new_create(void    *parent,
                                                    int16_t  x,
                                                    int16_t  y,
                                                    int16_t  w,
                                                    int16_t  row_gap,
                                                    int16_t  row_count, const char **string_array, int16_t string_array_size)
{
#define _GUI_NEW_GUI_SCROLL_WHEEL_NEW_PARAM this, parent, x, y, w, row_gap, row_count, string_array, string_array_size
    GUI_CREATE_HELPER(gui_scroll_wheel_new_t, ctor, _GUI_NEW_GUI_SCROLL_WHEEL_NEW_PARAM)
    {
        gui_win_t *win = gui_win_create(this, 0, 0, -row_gap, w, row_gap * (row_count + 1));
        gui_win_t *timer1 = gui_win_create(win, 0, 0, 0, w, row_gap * (row_count + 1));
        gui_win_set_animate(win, 1000, -1, override, timer1);
        this->win = timer1;

    }
    return this;
}
void **gui_scroll_wheel_new_get_image_pointers(const char *input, size_t *num_pointers,
                                               int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    get_image_pointers(input, num_pointers, text_image_map_length, text_image_map);
}
void gui_scroll_wheel_new_render(const char *text, gui_obj_t *obj, unsigned char render_mode,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    render(text, obj, render_mode, text_image_map_length, text_image_map);
}
_GUI_API_ASSIGN(gui_scroll_wheel_new_t)
.get_index = get_index,
 .set_index = set_index,
};
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
