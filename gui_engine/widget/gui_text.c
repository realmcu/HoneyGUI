/*
 * File      : gui_text.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_text.h>
#include <string.h>
#include "gui_obj.h"
#include <draw_font.h>



static void text_prepare(gui_obj_t *o)
{
    gui_text_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        size_t frame_count = obj->animate->dur * 30 / (1000);
        obj->animate->callback(obj->animate->p);
        obj->animate->current_frame++;

        if (obj->animate->current_frame > frame_count)
        {
            if (obj->animate->repeatCount == 0)
            {
                obj->animate->animate = false;
            }
            else if (obj->animate->repeatCount < 0)
            {
                obj->animate->current_frame = 0;
            }
            else if (obj->animate->repeatCount > 0)
            {
                obj->animate->current_repeat_count++;
                if (obj->animate->current_repeat_count >= obj->animate->repeatCount)
                {
                    obj->animate->animate = false;
                }
                else
                {
                    obj->animate->current_frame = 0;
                }
            }
        }
        obj->animate->progress_percent = ((float)(obj->animate->current_frame)) / ((float)(
                                                                                       frame_count));

    }
}
static void text_draw(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;
    if (text->len == 0)
    {
        return;
    }
    struct gui_dispdev *dc = gui_get_dc();
    rtgui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->ax + obj->dx + obj->tx;
    draw_rect.y1 = obj->ay + obj->dy + obj->ty;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;
    if (dc->section_count == 0)
    {
        rtgui_text_create(text);
    }
    rtgui_font_draw(text, &draw_rect);
    uint32_t total_section_count = dc->screen_height / dc->fb_height - ((dc->screen_height %
                                                                         dc->fb_height) ?
                                                                        0 : 1);
    if (dc->section_count == total_section_count)
    {
        rtgui_text_destroy(text);
    }
}
static void text_end(gui_obj_t *obj)
{
    // gui_text_t *text = (gui_text_t *)obj;
    // if (text->len == 0)
    // {
    //     return;
    // }
    // rtgui_text_destroy(text);
}

void gui_text_set(gui_text_t *this, const char *text, char *text_type, uint32_t color,
                  uint16_t length, uint8_t font_size)
{
    this->text_type = text_type;
    this->utf_8 = (uint8_t *)text;
    this->color = color;
    this->len = length;
    this->font_height = font_size;
    this->text_offset = 0;
}
void gui_text_set_animate(void *o, uint32_t dur, int repeatCount, void *callback, void *p)
{
    gui_animate_t *animate = ((gui_text_t *)o)->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeatCount = repeatCount;
    animate->p = p;
    ((gui_text_t *)o)->animate = animate;
}

void gui_text_mode_set(gui_text_t *this, TEXT_MODE mode)
{
    this->mode = mode;
}
// void gui_text_scale(gui_text_t *this, float scale_x, float scale_y)
// {
//     if (scale_x > 0 && scale_y > 0)
//     {
//         img->scale_x = scale_x;
//         img->scale_y = scale_y;
//     }
// }
void gui_text_move(gui_text_t *this, int16_t x, int16_t y)
{
    this->base.x = x ;
    this->base.y = y ;
}
void gui_text_size_set(gui_text_t *this, uint8_t height, uint8_t width)
{
    this->font_height = height;
}

void gui_text_type_set(gui_text_t *this, void *type)
{
    this->path = type;
}
void gui_text_ctor(gui_text_t *this, gui_obj_t *parent, const char *name, int16_t x,
                   int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = TEXTBOX;
    root->obj_prepare = text_prepare;
    root->obj_draw = text_draw;
    root->obj_end = text_end;

    //for self
    this->mode = LEFT;
}

gui_text_t *gui_text_create(void *parent, const char *name, int16_t x, int16_t y,
                            int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_TEXT";
    }
    gui_text_t *text = gui_malloc(sizeof(gui_text_t));
    GUI_ASSERT(text != NULL);
    memset(text, 0, sizeof(gui_text_t));

    gui_text_ctor(text, parent, name, x, y, w, h);
    gui_list_init(&(GET_BASE(text)->child_list));
    if ((GET_BASE(text)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(text)->parent)->child_list), &(GET_BASE(text)->brother_list));
    }
    GET_BASE(text)->create_done = true;
    return text;
}

