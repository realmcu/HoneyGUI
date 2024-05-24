/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_scroll_text.c
  * @brief scroll text widget
  * @details scroll text widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/11/08
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_scroll_text.h"
#include "font_mem.h"
#include "font_stb.h"
#include "font_mem_matrix.h"
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

static uint32_t cur_time_ms;
/**
 * @brief The number of frames that need to be skipped.
 * If the value is 2, then scrolling text will skip two frames and scroll one frame.
 */
static uint8_t scroll_text_count = 0;
static uint8_t scroll_skip_frame = 10;
static uint8_t scroll_skip_frame_count = 0;

extern void gui_text_ctor(gui_text_t *this,
                          gui_obj_t  *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_scroll_text_font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_load(text, rect);
        }
        break;

    case GUI_FONT_SRC_TTF:
        {
            gui_font_stb_load(text, rect);
        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            gui_font_mat_load(text, rect);
        }
        break;

    default:
        break;
    }
}

static void gui_scroll_text_font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_draw(text, rect);
        }
        break;

    case GUI_FONT_SRC_TTF:
        {
            gui_font_stb_draw(text, rect);
        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            gui_font_mat_draw(text, rect);
        }
        break;

    default:
        break;
    }
}

static void gui_scroll_text_font_unload(gui_text_t *text)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_unload(text);
        }
        break;

    case GUI_FONT_SRC_TTF:
        {
            gui_font_stb_unload(text);
        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            gui_font_mat_unload(text);
        }
        break;

    default:
        break;
    }
}

static void gui_scroll_text_update_att(gui_obj_t *obj)
{
    gui_scroll_text_t *this = (void *)obj;
    uint32_t cur_time_gap;

    if (this->base.animate && this->base.animate->animate)
    {
        cur_time_gap = gui_ms_get() - this->base.animate->cur_time_ms;
        this->base.animate->cur_time_ms = gui_ms_get();

        if (cur_time_gap >= 2 * this->base.animate->dur)
        {
            this->base.animate->init_time_ms += cur_time_gap;
        }

        if (this->base.animate->repeat_count == 0)
        {
            if ((this->base.animate->cur_time_ms - this->base.animate->init_time_ms) >= this->base.animate->dur)
            {
                this->base.animate->callback(this->base.animate->p);
                this->base.animate->animate = false;
                this->base.animate->progress_percent = 1.0f;
            }
            else
            {
                this->base.animate->progress_percent = (float)(this->base.animate->cur_time_ms -
                                                               this->base.animate->init_time_ms) /
                                                       (float)this->base.animate->dur;
            }
        }
        else if (this->base.animate->repeat_count < 0)
        {
            if ((this->base.animate->cur_time_ms - this->base.animate->init_time_ms) >= this->base.animate->dur)
            {
                this->base.animate->callback(this->base.animate->p);
                this->base.animate->init_time_ms += this->base.animate->dur;
                this->base.animate->progress_percent = 1.0f;
            }
            else
            {
                this->base.animate->progress_percent = (float)(this->base.animate->cur_time_ms -
                                                               this->base.animate->init_time_ms) /
                                                       (float)this->base.animate->dur;
            }
        }
        else if (this->base.animate->repeat_count > 0)
        {
            if ((this->base.animate->cur_time_ms - this->base.animate->init_time_ms -
                 this->base.animate->current_repeat_count * this->base.animate->dur) >=
                this->base.animate->dur)
            {
                if (this->base.animate->current_repeat_count < this->base.animate->repeat_count)
                {
                    this->base.animate->callback(this->base.animate->p);
                    this->base.animate->current_repeat_count ++;
                }
                else
                {
                    this->base.animate->callback(this->base.animate->p);
                    this->base.animate->animate = false;
                }
                this->base.animate->progress_percent = 1.0f;
            }
            else
            {
                this->base.animate->progress_percent = (float)(this->base.animate->cur_time_ms -
                                                               this->base.animate->init_time_ms -
                                                               this->base.animate->current_repeat_count * this->base.animate->dur) /
                                                       (float)this->base.animate->dur;
            }
        }
    }
}

static void gui_scroll_text_prepare(gui_obj_t *obj)
{
    gui_text_t *this = (void *)obj;
    gui_point_t point = {0, 0, 1};

    if (!this->base.not_show)
    {
        if (scroll_skip_frame == 0)
        {
            gui_fb_change();
        }
        else
        {
            if (scroll_skip_frame_count < (scroll_skip_frame) * scroll_text_count - 1)
            {
                gui_fb_change();
                scroll_skip_frame_count++;
            }
            else if (scroll_skip_frame_count < (scroll_skip_frame + 1) * scroll_text_count - 1)
            {
                scroll_skip_frame_count++;
            }
            else
            {
                scroll_skip_frame_count = 0;
            }
        }
    }

    gui_scroll_text_update_att(obj);
    matrix_multiply_point(obj->matrix, &point);

    this->offset_x = point.p[0];
    this->offset_y = point.p[1];
}

static void gui_scroll_text_draw(gui_obj_t *obj)
{
    gui_scroll_text_t *text = (gui_scroll_text_t *)obj;
    struct gui_dispdev *dc;
    int32_t offset;
    uint32_t index;
    gui_text_rect_t draw_rect = {0};
    uint32_t total_section_count;

    if (text->base.len == 0)
    {
        return;
    }

    dc = gui_get_dc();
    if (dc->section_count == 0)
    {
        cur_time_ms = gui_ms_get();
    }
    draw_rect.x1 = text->base.offset_x;
    draw_rect.y1 = text->base.offset_y;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;

    if (text->base.mode == SCROLL_X)
    {
        offset = text->base.char_width_sum;
        if (offset > obj->w)
        {
            index = (cur_time_ms - text->init_time_ms) % text->interval_time_ms;
            text->cnt_value = (text->end_value + text->start_value + offset) * index
                              / text->interval_time_ms;
            draw_rect.x1 = text->base.offset_x - text->cnt_value + text->start_value;
            draw_rect.x2 = draw_rect.x1 + offset;
            draw_rect.y1 = text->base.offset_y;
            draw_rect.y2 = draw_rect.y1 + obj->h;
        }
    }
    else if (text->base.mode == SCROLL_Y)
    {
        offset = text->base.char_line_sum * text->base.font_height;
        if (offset > obj->h || offset == 0)
        {
            index = (cur_time_ms - text->init_time_ms) % text->interval_time_ms;
            text->cnt_value = (text->end_value + text->start_value + offset) * index
                              / text->interval_time_ms;
            draw_rect.x1 = text->base.offset_x;
            draw_rect.x2 = draw_rect.x1 + obj->w;
            draw_rect.y1 = text->base.offset_y - text->cnt_value + text->start_value;
            draw_rect.y2 = draw_rect.y1 + offset;
        }
    }
    else if (text->base.mode == SCROLL_Y_REVERSE)
    {
        offset = text->base.char_height_sum;
        if (offset > obj->h || offset == 0)
        {
            index = (cur_time_ms - text->init_time_ms) % text->interval_time_ms;
            text->cnt_value = (text->end_value + text->start_value + offset) * index
                              / text->interval_time_ms;
            draw_rect.x1 = text->base.offset_x;
            draw_rect.x2 = draw_rect.x1 + obj->w;
            draw_rect.y1 = text->base.offset_y + obj->h + text->cnt_value - text->start_value;
            draw_rect.y2 = draw_rect.y1 - offset;
        }
    }

    draw_rect.xboundleft = text->base.offset_x;
    draw_rect.xboundright = text->base.offset_x + obj->w;
    draw_rect.yboundtop = text->base.offset_y;
    draw_rect.yboundbottom = text->base.offset_y + obj->h;

    if (dc->section_count == 0)
    {
        gui_scroll_text_font_load(&text->base, &draw_rect);
    }

    if (text->duration_time_ms == 0)
    {
        gui_scroll_text_font_draw(&text->base, &draw_rect);
    }
    else
    {
        if (cur_time_ms < (text->init_time_ms + text->duration_time_ms))
        {
            gui_scroll_text_font_draw(&text->base, &draw_rect);
        }
    }

    total_section_count = dc->screen_height / dc->fb_height -
                          ((dc->screen_height % dc->fb_height) ? 0 : 1);

    if (dc->section_count == total_section_count)
    {
        gui_scroll_text_font_unload(&text->base);
    }
}

static void gui_scroll_text_end(gui_obj_t *obj)
{

}

static void gui_scroll_text_destory(gui_obj_t *obj)
{
    scroll_text_count = scroll_text_count > 0 ? scroll_text_count - 1 : 0;
}

static void gui_scroll_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_scroll_text_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_scroll_text_draw(obj);
            break;

        case OBJ_END:
            gui_scroll_text_end(obj);
            break;

        case OBJ_DESTORY:
            gui_scroll_text_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_scroll_text_ctor(gui_scroll_text_t *this,
                                 gui_obj_t         *parent,
                                 const char        *name,
                                 int16_t            x,
                                 int16_t            y,
                                 int16_t            w,
                                 int16_t            h)
{
    gui_text_t *base = (gui_text_t *)this;
    gui_obj_t *root = (gui_obj_t *)this;

    //for base class
    gui_text_ctor(base, parent, name, x, y, w, h);
    base->mode = SCROLL_X;

    //for root class
    root->type = SCROLLTEXTBOX;
    root->obj_cb = gui_scroll_text_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    this->init_time_ms = gui_ms_get();
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_scroll_text_skip_frame_set(uint8_t skip_frame)
{
    scroll_skip_frame = skip_frame;
}

void gui_scroll_text_set(gui_scroll_text_t *this,
                         void              *text,
                         FONT_SRC_TYPE      text_type,
                         gui_color_t        color,
                         uint16_t           length,
                         uint8_t            font_size)
{
    gui_text_set(&this->base, text, text_type, color, length, font_size);
    gui_scroll_text_restart(this);
}

void gui_scroll_text_type_set(gui_scroll_text_t *this, void *font_source)
{
    this->base.path = font_source;
}

void gui_scroll_text_content_set(gui_scroll_text_t *this, void *text, uint16_t length)
{
    gui_text_content_set(&this->base, text, length);
    gui_scroll_text_restart(this);
}

void gui_scroll_text_restart(gui_scroll_text_t *this)
{
    this->base.char_width_sum = 0;
    this->base.char_line_sum = 0;
    this->init_time_ms = gui_ms_get();
    gui_fb_change();
}

void gui_scroll_text_encoding_set(gui_scroll_text_t *this, TEXT_CHARSET charset)
{
    this->base.charset = charset;
}

void gui_scroll_text_scroll_set(gui_scroll_text_t *this,
                                TEXT_MODE          mode,
                                uint8_t            start_value,
                                uint8_t            end_value,
                                uint32_t           interval_time_ms,
                                uint32_t           duration_time_ms)
{
    this->base.mode = mode;
    this->start_value = start_value;
    this->end_value = end_value;
    this->interval_time_ms = interval_time_ms;
    this->duration_time_ms = duration_time_ms;
}

gui_scroll_text_t *gui_scroll_text_create(void       *parent,
                                          const char *name,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h)
{
    gui_scroll_text_t *scroll_text;

    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_SCROLLTEXTBOX";
    }

    scroll_text = gui_malloc(sizeof(gui_scroll_text_t));
    GUI_ASSERT(scroll_text != NULL);
    memset(scroll_text, 0, sizeof(gui_scroll_text_t));

    gui_scroll_text_ctor(scroll_text, parent, name, x, y, w, h);
    gui_list_init(&(GET_BASE(scroll_text)->child_list));
    if ((GET_BASE(scroll_text)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(scroll_text)->parent)->child_list),
                               &(GET_BASE(scroll_text)->brother_list));
    }

    GET_BASE(scroll_text)->create_done = true;
    scroll_text_count++;

    return scroll_text;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
