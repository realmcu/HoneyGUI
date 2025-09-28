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
#include "math.h"

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

static void **get_image_pointers(const char *input, size_t *num_pointers,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map);
static int get_index(gui_scroll_wheel_new_t *this)
{
    //GUI_WIDGET_TYPE_TRY_EXCEPT(this, WINDOW)
    return this->index_offset;
}
static int set_index(gui_scroll_wheel_new_t *this, int index)
{
    //GUI_WIDGET_TYPE_TRY_EXCEPT(this, WINDOW)
    this->index_offset = index;
    this->take_over = true;
    return this->index_offset;
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
    this->font_color = APP_COLOR_WHITE;
    this->font_color_highlight = APP_COLOR_GRAY;
    this->loop = 1;
    //


    //this->touch_y = touch_y[row_count-1];
    this->touch_y = ceil((row_count + 1) / 2.0) * row_gap;
    this->touch_y_old = this->touch_y;
}
static void text_widget_array_foreach(gui_scroll_wheel_new_t *this, gui_obj_t **text_widget_array,
                                      int array_count)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *obj = GUI_BASE(this->win);
    gui_obj_t *object = obj;
    GUI_UNUSED(object);
    int count = 0;
    T_OBJ_TYPE type = TEXTBOX;
    if (this->render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
    {
        type = IMAGE_FROM_MEM;
    }

    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (count >= array_count)
        {
            return;
        }

        if (obj && obj->type == WINDOW)
        {
            GUI_WIDGET_POINTER_BY_TYPE(text, type, obj)
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
    this->speed /= 2;
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
            //gui_text_convert_to_img((void *)obj, ARGB8888);
        }
        break;
    case GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY:
        {
            const char *input = text;
            size_t num_pointers;
            void **image_pointers = get_image_pointers(input, &num_pointers, text_image_map_length,
                                                       text_image_map);
            if (num_pointers == 0)
            {
                if (image_pointers)
                {
                    gui_free(image_pointers);
                }
                break;

            }
            else
            {
                gui_image_array_write((void *)obj, image_pointers, num_pointers);
                // Free allocated memory
                gui_free(image_pointers);
                GUI_TYPE(gui_image_array_t, obj)->text = text;
            }

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
    int16_t time_array_offset = this->time_array_offset;
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
        gui_obj_absolute_xy(GUI_BASE(this), &ax, &ay);
        if (touch->pressed &&
            touch->x > ax  &&
            touch->x < ax + win->w &&
            touch->y > ay &&
            touch->y < ay + this->gap * this->count)
        {
            this->history_y = this->touch_y;
            this->speed = 0;
            memset(this->recode, 0, sizeof(this->recode));
        }
        if (touch->pressing &&
            touch->x > ax  &&
            touch->x < ax + win->w &&
            touch->y > ay &&
            touch->y < ay + this->gap * this->count)
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
        if (!this->loop)
        {





            if (this->touch_y > this->touch_y_old)
            {
                this->touch_y = this->touch_y_old;
            }
            if (this->touch_y < this->touch_y_old - this->gap * (time_array_size - this->blank_count - 1))
            {
                this->touch_y = this->touch_y_old - this->gap * (time_array_size - this->blank_count - 1);
            }
            //gui_log("this->touch_y:%d\n",this->touch_y);

        }

        if (this->render)
        {
            time_array_offset = -(this->touch_y / this->gap % time_array_size);
            int widget_offset = this->touch_y % this->gap;
            GUI_BASE(win)->y = widget_offset;

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
                    if (i == (this->count + 2) / 2 && !this->take_over)
                    {
                        this->index_offset = index;
                    }
                    if (win->y + this->win2->base.y + i * this->gap <= this->gap * this->count / 2 &&
                        win->y + this->win2->base.y + i * this->gap + this->gap > this->gap * this->count / 2)
                    {
                        GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color_highlight;
                        gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                                     this->font_color_highlight,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                                     this->font_size_highlight
                                    );
                        gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                          (void *)this->font_highlight,
                                          GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
                    }
                    else
                    {
                        GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color;
                        gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                                     this->font_color,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                                     this->font_size
                                    );
                        gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                          (void *)this->font,
                                          GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
                    }

                    render(text, (void *)text_widget_array[i], this->render_mode, this->text_image_map_length,
                           this->text_image_map);
                }
            }

            if (this->alien && widget_offset == 0 && this->speed <= this->end_speed &&
                this->speed >= -this->end_speed)
            {
                this->render = 0;
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
                    if (i == (this->count + 2) / 2 && !this->take_over)
                    {
                        this->index_offset = index;
                    }
                    if (win->y + this->win2->base.y + i * this->gap <= this->gap * this->count / 2 &&
                        win->y + this->win2->base.y + i * this->gap + this->gap > this->gap * this->count / 2)
                    {
                        GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color_highlight;
                        gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                                     this->font_color_highlight,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                                     this->font_size_highlight
                                    );
                        gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                          (void *)this->font_highlight,
                                          GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
                    }
                    else
                    {
                        GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color;
                        gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                                     this->font_color,
                                     GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                                     this->font_size
                                    );
                        gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                          (void *)this->font,
                                          GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
                    }
                    render(text, (void *)text_widget_array[i], this->render_mode, this->text_image_map_length,
                           this->text_image_map);
                }
            }
        }
    }
    if (this->take_over)
    {
        this->take_over = false;
        this->history_y = -(this->index_offset - (this->count + 2) / 2) * this->gap;
        this->touch_y = this->history_y;
        gui_obj_t *text_widget_array[this->count + 2];
        text_widget_array_foreach(this, text_widget_array, this->count + 2);
        time_array_offset = -(this->touch_y / this->gap % time_array_size);
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
            // if (i == (this->count + 2) / 2)
            // {
            //     this->index_offset = index;
            // }
            if (win->y + this->win2->base.y + i * this->gap <= this->gap * this->count / 2 &&
                win->y + this->win2->base.y + i * this->gap + this->gap > this->gap * this->count / 2)
            {
                GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color_highlight;
                gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                             GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                             GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                             this->font_color_highlight,
                             GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                             this->font_size_highlight
                            );
                gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                  (void *)this->font_highlight,
                                  GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
            }
            else
            {
                GUI_TYPE(gui_text_t, text_widget_array[i])->color = this->font_color;
                gui_text_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                             GUI_TYPE(gui_text_t, text_widget_array[i])->content,
                             GUI_TYPE(gui_text_t, text_widget_array[i])->font_type,
                             this->font_color,
                             GUI_TYPE(gui_text_t, text_widget_array[i])->len,
                             this->font_size
                            );
                gui_text_type_set(GUI_TYPE(gui_text_t, text_widget_array[i]),
                                  (void *)this->font,
                                  GUI_TYPE(gui_text_t, text_widget_array[i])->font_mode);
            }
            render(text, (void *)text_widget_array[i], this->render_mode, this->text_image_map_length,
                   this->text_image_map);
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
                if (image_pointers != NULL)
                {
                    image_pointers[(*num_pointers)++] = img;
                }

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
                if (image_pointers)
                {
                    image_pointers[(*num_pointers)++] = img;
                }
            }
        }

        ptr = next;
    }

    return image_pointers;
}
#include "gui_text.h"
#include "gui_matrix.h"
#include "font_mem.h"
#include "font_ttf.h"




#include "gui_fb.h"
static void gui_scroll_text_read_scope(gui_text_t *text, gui_text_rect_t *rect)
{
    text->scope = 0;
    gui_obj_t *o = (gui_obj_t *)text;
    gui_win_t *win_scope = NULL;
    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == WINDOW && GUI_TYPE(gui_win_t, o)->scope)
        {
            text->scope = 1;
            win_scope = (void *)o;
            break;
        }
    }
    if (text->scope && win_scope != NULL)
    {
        int scopex = win_scope->base.matrix->m[0][2];
        int scopey = win_scope->base.matrix->m[1][2];
        int scopew = win_scope->base.matrix->m[0][0] * win_scope->base.w;
        int scopeh = win_scope->base.matrix->m[1][1] * win_scope->base.h;

        rect->xboundleft = _UI_MAX(scopex, rect->xboundleft);
        rect->xboundright = _UI_MIN(scopex + scopew - 1, rect->xboundright);
        rect->yboundtop = _UI_MAX(scopey, rect->yboundtop);
        rect->yboundbottom = _UI_MIN(scopey + scopeh - 1, rect->yboundbottom);
    }
}





static void gui_text_font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    extern void gui_text_font_load_global(gui_text_t *text, gui_text_rect_t *rect);
    gui_text_font_load_global(text, rect);
}

static void gui_text_font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    extern void gui_text_font_draw_global(gui_text_t *text, gui_text_rect_t *rect);
    gui_text_font_draw_global(text, rect);
}

static void gui_text_font_unload(gui_text_t *text)
{
    extern void gui_text_font_unload_global(gui_text_t *text);
    gui_text_font_unload_global(text);
}
static void gui_text_draw(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;
    struct gui_dispdev *dc;
    gui_text_rect_t draw_rect = {0};
    uint32_t total_section_count = UINT32_MAX;

    if (text->len == 0)
    {
        text->content_refresh = false;
        text->layout_refresh = false;
        return;
    }

    dc = gui_get_dc();
    draw_rect.x1 = text->offset_x;
    draw_rect.y1 = text->offset_y;
    draw_rect.x2 = draw_rect.x1 + obj->w - 1;
    draw_rect.y2 = draw_rect.y1 + obj->h - 1;
    if (text->mode != VERTICAL_LEFT_TOP)
    {
        draw_rect.xboundleft = draw_rect.x1;
        draw_rect.xboundright = draw_rect.x2;
        draw_rect.yboundtop = draw_rect.y1;
        draw_rect.yboundbottom = draw_rect.y2;
        gui_scroll_text_read_scope((gui_text_t *)text, &draw_rect);
    }



    if (dc->section_count == 0)
    {
        gui_text_font_load(text, &draw_rect);
    }
    if (text->font_type == GUI_FONT_SRC_TTF)
    {
        gui_font_ttf_adapt_rect(text, &draw_rect);
    }
    if (text->font_type == GUI_FONT_SRC_MAT && text->use_img_blit)
    {
        gui_font_ttf_adapt_rect(text, &draw_rect);
    }

    if (dc->pfb_type == PFB_X_DIRECTION)
    {
        total_section_count = dc->screen_width / dc->fb_width -
                              ((dc->screen_width % dc->fb_width) ? 0 : 1);
        if (!(draw_rect.x1 >= (int)(dc->section_count + 1)*dc->fb_width || \
              draw_rect.x2 < (int)(dc->section_count)*dc->fb_width))
        {
            gui_text_font_draw(text, &draw_rect);
        }
    }
    else if (dc->pfb_type == PFB_Y_DIRECTION)
    {
        total_section_count = dc->screen_height / dc->fb_height -
                              ((dc->screen_height % dc->fb_height) ? 0 : 1);
        if (!(draw_rect.y1 >= (int)(dc->section_count + 1)*dc->fb_height || \
              draw_rect.y2 < (int)(dc->section_count)*dc->fb_height))
        {
            gui_text_font_draw(text, &draw_rect);
        }
    }

    if (dc->section_count == total_section_count)
    {
        gui_text_font_unload(text);
    }
}


void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        extern void gui_text_input_prepare_global(gui_obj_t *obj);

        extern void gui_text_prepare_global(gui_obj_t *obj);

        extern void gui_text_end_global(gui_obj_t *obj);

        extern void gui_text_destroy_global(gui_obj_t *obj);

        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_text_input_prepare_global(obj);
            break;
        case OBJ_PREPARE:
            gui_text_prepare_global(obj);
            break;

        case OBJ_DRAW:
            gui_text_draw(obj);
            break;

        case OBJ_END:
            gui_text_end_global(obj);
            break;

        case OBJ_DESTROY:
            gui_text_destroy_global(obj);
            break;

        default:
            break;
        }
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_scroll_wheel_new_render_text_alien(gui_scroll_wheel_new_t *widget,
                                            const void *font_file_pointer,
                                            int font_size, TEXT_MODE mode)
{
    widget->render_mode = GUI_SCROLL_WHEEL_NEW_RENDER_TEXT;
    for (size_t i = 0; i < widget->count + 2; i++)
    {
        gui_win_t *win = gui_win_create(widget->win, 0, 0, widget->gap * i, GUI_BASE(widget)->w,
                                        widget->gap);
        gui_color_t color = widget->font_color;
        // if (widget->win->base.y + widget->win2->base.y + i * widget->gap <= widget->gap * widget->count / 2
        //     &&
        //     widget->win->base.y + widget->win2->base.y + i * widget->gap + widget->gap > widget->gap *
        //     widget->count / 2)
        // {
        //     color = widget->font_color_highlight;
        // }
        {
            int time_array_offset = -((widget->count + 2) / 2 % widget->string_array_size);
            int index = time_array_offset + i;
            if (index >= widget->string_array_size)
            {
                index -= widget->string_array_size;
            }
            if (index < 0)
            {
                index += widget->string_array_size;
            }
            const char *text = widget->string_array[index];
            bool highlight_bigger = false;
            if (widget->font_size < widget->font_size_highlight)
            {
                highlight_bigger = true;
            }

            if (widget->index_highlight == index)
            {
                gui_text_t *t = gui_text_create(win, text, 0, 0, GUI_BASE(widget)->w,
                                                widget->gap);
                gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, widget->font_color_highlight, strlen(text),
                             widget->font_size_highlight);

                const void *addr1 = widget->font_highlight;
                gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                gui_text_mode_set(t, mode);

                GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
                //GUI_BASE(t)->h = t->font_height;

            }
            else
            {
                gui_text_t *t = gui_text_create(win, text, 0, 0, GUI_BASE(widget)->w,
                                                widget->gap);
                if (highlight_bigger && widget->font_size_default == 0)
                {
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, widget->font_color_highlight, strlen(text),
                                 widget->font_size_highlight);

                    const void *addr1 = widget->font_highlight;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, mode);
                    widget->font_size_default = t->font_height;
                }
                gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);

                const void *addr1 = font_file_pointer;
                gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                gui_text_mode_set(t, mode);
                GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
                //GUI_BASE(t)->h = t->font_height;
                if (!highlight_bigger)
                {
                    widget->font_size_default = t->font_height;
                }


                //gui_text_convert_to_img(t, ARGB8888);

            }
        }

    }
    gui_obj_t *text_widget_array[widget->count + 2];
    text_widget_array_foreach(widget, text_widget_array, widget->count + 2);
    for (size_t i = 0; i < widget->count + 2; i++)
    {
        GUI_BASE(text_widget_array[i])->h = widget->font_size_default;
    }
    set_index(widget, widget->index_highlight);
}
void gui_scroll_wheel_new_render_text(gui_scroll_wheel_new_t *widget, const void *font_file_pointer,
                                      int font_size)
{
    gui_scroll_wheel_new_render_text_alien(widget, font_file_pointer, font_size, LEFT);
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
            if (num_pointers == 0)
            {
                gui_log("No image pointers found for input \"%s\"\n", input);
                if (image_pointers)
                {
                    gui_free(image_pointers);
                }
                continue;
            }

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
void gui_scroll_wheel_new_render_image_array_ftl(gui_scroll_wheel_new_t *widget,
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
            if (num_pointers == 0)
            {
                gui_log("No image pointers found for input \"%s\"\n", input);
                if (image_pointers)
                {
                    gui_free(image_pointers);
                }
                continue;
            }

            // Print results
            gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
            for (size_t i = 0; i < num_pointers; ++i)
            {
                gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
            }
            gui_image_array_create_ftl(win, 0, 0, image_pointers, num_pointers,
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
    //GUI_BASE(widget->win)->w -= offset;

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
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)override, timer1);
        this->win = timer1;
        this->win2 = win;

    }
    return this;
}
void **gui_scroll_wheel_new_get_image_pointers(const char *input, size_t *num_pointers,
                                               int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    return get_image_pointers(input, num_pointers, text_image_map_length, text_image_map);
}
void gui_scroll_wheel_new_render(const char *text, gui_obj_t *obj, unsigned char render_mode,
                                 int16_t text_image_map_length, const struct gui_text_image_map *text_image_map)
{
    render(text, obj, render_mode, text_image_map_length, text_image_map);
}
//_GUI_API_ASSIGN(gui_scroll_wheel_new_t)
_gui_api_gui_scroll_wheel_new_t _gui_api_for_gui_scroll_wheel_new_t =
{
    .get_index = get_index,
    .set_index = set_index,
};
