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
#include <guidef.h>
#include <gui_scroll_text.h>
#include <string.h>
#include <draw_font.h>

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

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void scrolltext_prepare(gui_obj_t *obj)
{
    gui_scroll_text_t *object = (gui_scroll_text_t *)obj;

    if (object->base.animate && object->base.animate->animate)
    {
        size_t frame_count = object->base.animate->dur * 30 / (1000);
        object->base.animate->callback(object->base.animate->p);
        object->base.animate->current_frame++;

        if (object->base.animate->current_frame > frame_count)
        {
            if (object->base.animate->repeatCount == 0)
            {
                object->base.animate->animate = false;
            }
            else if (object->base.animate->repeatCount < 0)
            {
                object->base.animate->current_frame = 0;
            }
            else if (object->base.animate->repeatCount > 0)
            {
                object->base.animate->current_repeat_count++;
                if (object->base.animate->current_repeat_count >= object->base.animate->repeatCount)
                {
                    object->base.animate->animate = false;
                }
                else
                {
                    object->base.animate->current_frame = 0;
                }
            }
        }
        object->base.animate->progress_percent = ((float)(object->base.animate->current_frame)) /
                                                 ((float)(frame_count));
    }
}

static void scrolltext_draw(gui_obj_t *obj)
{
    gui_scroll_text_t *text = (gui_scroll_text_t *)obj;
    if (text->base.len == 0)
    {
        return;
    }
    struct gui_dispdev *dc = gui_get_dc();
    if (dc->section_count == 0)
    {
        cur_time_ms = gui_ms_get();
        rtgui_text_create(&text->base);
    }
    uint32_t offset = text->base.text_offset;
    uint32_t index = (cur_time_ms - text->init_time_ms) % text->interval_time_ms;
    text->cnt_value = (text->end_value + text->start_value + offset) * index
                      / text->interval_time_ms;
    rtgui_rect_t draw_rect = {0};
    if (text->base.mode == SCROLL_X && offset > obj->w)
    {

        //draw_rect.x1 = obj->dx - text->cnt_value + text->start_value;
        draw_rect.x1 = obj->ax + obj->dx + obj->tx - text->cnt_value + text->start_value;
        draw_rect.x2 = draw_rect.x1 + text->base.text_offset;
        //draw_rect.y1 = obj->dy;
        draw_rect.y1 = obj->ay + obj->dy + obj->ty;
        draw_rect.y2 = draw_rect.y1 + obj->h;
    }
    else if (text->base.mode == SCROLL_Y && (offset > obj->h || offset == 0))
    {
        draw_rect.x1 = obj->dx;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y1 = obj->dy - text->cnt_value + text->start_value;
        draw_rect.y2 = draw_rect.y1 + text->base.text_offset;
    }
    else
    {
        draw_rect.x1 = obj->dx;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y1 = obj->dy;
        draw_rect.y2 = draw_rect.y1 + obj->h;
    }
    //draw_rect.xboundleft = obj->dx;
    // draw_rect.xboundright = obj->dx + obj->w;
    // draw_rect.yboundtop = obj->dy;
    // draw_rect.yboundbottom = obj->dy + obj->h;

    draw_rect.xboundleft = obj->ax + obj->dx + obj->tx;
    draw_rect.xboundright = obj->ax + obj->dx + obj->tx + obj->w;
    draw_rect.yboundtop = obj->ay + obj->dy + obj->ty;
    draw_rect.yboundbottom = obj->ay + obj->dy + obj->ty + obj->h;
    if (text->duration_time_ms == 0)
    {
        rtgui_font_draw(&text->base, &draw_rect);
    }
    else
    {
        if (cur_time_ms < (text->init_time_ms + text->duration_time_ms))
        {
            rtgui_font_draw(&text->base, &draw_rect);
        }
    }
    uint32_t total_section_count = dc->screen_height / dc->fb_height - ((dc->screen_height %
                                                                         dc->fb_height) ?
                                                                        0 : 1);
    if (dc->section_count == total_section_count)
    {
        rtgui_text_destroy(&text->base);
    }
}

static void scrolltext_end(gui_obj_t *obj)
{
    // gui_text_t *text = (gui_text_t *)obj;
    // if (text->len == 0)
    // {
    //     return;
    // }
    // rtgui_text_destroy(text);
}

static void gui_scrolltext_ctor(gui_scroll_text_t *this, gui_obj_t *parent, const char *name,
                                int16_t x, int16_t y, int16_t w, int16_t h)
{
    gui_text_t *base = (gui_text_t *)this;
    extern void gui_text_ctor(gui_text_t *this, gui_obj_t *parent, const char *name, int16_t x,
                              int16_t y, int16_t w, int16_t h);
    gui_text_ctor(base, parent, name, x, y, w, h);
    base->mode = SCROLL_X;

    gui_obj_t *root = (gui_obj_t *)this;
    root->type = SCROLLTEXTBOX;
    root->obj_prepare = scrolltext_prepare;
    root->obj_draw = scrolltext_draw;
    root->obj_end = scrolltext_end;

    this->init_time_ms = gui_ms_get();
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_scrolltext_text_set(gui_scroll_text_t *this, const char *text, char *text_type,
                             gui_color_t color, uint16_t length, uint8_t font_size)
{
    this->base.text_type = text_type;
    this->base.utf_8 = (uint8_t *)text;
    this->base.color = color;
    this->base.len = length;
    this->base.font_height = font_size;
    this->base.text_offset = 0;
}

void gui_scrolltext_scroll_set(gui_scroll_text_t *this, TEXT_MODE mode, uint8_t start_value,
                               uint8_t end_value,
                               uint32_t interval_time_ms, uint32_t duration_time_ms)
{
    this->base.mode = mode;
    this->start_value = start_value;
    this->end_value = end_value;
    this->interval_time_ms = interval_time_ms;
    this->duration_time_ms = duration_time_ms;
}

gui_scroll_text_t *gui_scrolltext_create(void *parent, const char *name, int16_t x, int16_t y,
                                         int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_SCROLLTEXTBOX";
    }
    gui_scroll_text_t *scrolltext = gui_malloc(sizeof(gui_scroll_text_t));
    GUI_ASSERT(scrolltext != NULL);
    memset(scrolltext, 0, sizeof(gui_scroll_text_t));

    gui_scrolltext_ctor(scrolltext, parent, name, x, y, w, h);
    gui_list_init(&(GET_BASE(scrolltext)->child_list));
    if ((GET_BASE(scrolltext)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(scrolltext)->parent)->child_list),
                               &(GET_BASE(scrolltext)->brother_list));
    }
    GET_BASE(scrolltext)->create_done = true;
    return scrolltext;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
