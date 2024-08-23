/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_text.c
  * @brief text widget
  * @details text widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/10/25
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_text.h"
#include "gui_matrix.h"
#include "font_mem.h"
#include "font_stb.h"
#include "font_mem_img.h"
#include "font_mem_matrix.h"
#include "tp_algo.h"
#include "gui_fb.h"
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
static bool gui_text_matrix_is_identity(gui_matrix_t *matrix)
{
    if (matrix == NULL)
    {
        return false;
    }
    if (matrix->m[0][0] == 1.0f && matrix->m[1][1] == 1.0f)
    {
        return true;
    }
    else
    {
        return false;
    }
}
static void gui_text_font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_load(text, rect);
        }
        break;

    case GUI_FONT_SRC_STB:
        {
            gui_font_stb_load(text, rect);
        }
        break;

    case GUI_FONT_SRC_IMG:
        {

        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            if (gui_text_matrix_is_identity(text->base.matrix))
            {
                gui_font_mem_load(text, rect);
            }
            else
            {
                gui_font_mat_load(text, rect);
            }
        }
        break;

#ifdef ENABLE_FONT_FREETYPE
    case GUI_FONT_SRC_FT:
        {
            gui_font_ft_load(text, rect);
        }
        break;
#endif

    default:
        break;
    }
}

static void gui_text_font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_draw(text, rect);
        }
        break;

    case GUI_FONT_SRC_STB:
        {
            gui_font_stb_draw(text, rect);
        }
        break;

    case GUI_FONT_SRC_IMG:
        {

        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            if (gui_text_matrix_is_identity(text->base.matrix))
            {
                gui_font_mem_draw(text, rect);
            }
            else
            {
                gui_font_mat_draw(text, rect);
            }
        }
        break;

#ifdef ENABLE_FONT_FREETYPE
    case GUI_FONT_SRC_FT:
        {
            gui_font_ft_draw(text, rect);
        }
        break;
#endif

    default:
        break;
    }
}

static void gui_text_font_unload(gui_text_t *text)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            // gui_font_mem_unload(text);
        }
        break;

    case GUI_FONT_SRC_STB:
        {
            gui_font_stb_unload(text);
        }
        break;

    case GUI_FONT_SRC_IMG:
        {

        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            // gui_font_mat_unload(text);
        }
        break;

#ifdef ENABLE_FONT_FREETYPE
    case GUI_FONT_SRC_FT:
        {
            gui_font_ft_unload(text);
        }
        break;
#endif

    default:
        break;
    }
}

static void gui_text_font_destory(gui_text_t *text)
{
    if (text->animate)
    {
        gui_free(text->animate);
    }

    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_obj_destroy(text);
        }
        break;

    case GUI_FONT_SRC_STB:
        {

        }
        break;

    case GUI_FONT_SRC_IMG:
        {
            gui_font_scale_destory(text);
        }
        break;

    case GUI_FONT_SRC_MAT:
        {
            gui_font_mat_unload(text);
        }
        break;

    case GUI_FONT_SRC_FT:
        {

        }
        break;

    default:
        break;
    }
}
static void gui_text_update_att(gui_obj_t *obj)
{
    gui_text_t *this = (void *)obj;
    uint32_t cur_time_gap;

    if (this->animate && this->animate->animate)
    {
        cur_time_gap = gui_ms_get() - this->animate->cur_time_ms;
        this->animate->cur_time_ms = gui_ms_get();

        if (cur_time_gap >= 2 * this->animate->dur)
        {
            this->animate->init_time_ms += cur_time_gap;
        }

        if (this->animate->repeat_count == 0)
        {
            if ((this->animate->cur_time_ms - this->animate->init_time_ms) >= this->animate->dur)
            {
                this->animate->callback(this->animate->p, this);
                this->animate->animate = false;
                this->animate->progress_percent = 1.0f;
            }
            else
            {
                this->animate->progress_percent = (float)(this->animate->cur_time_ms -
                                                          this->animate->init_time_ms) /
                                                  (float)this->animate->dur;
            }
        }
        else if (this->animate->repeat_count < 0)
        {
            if ((this->animate->cur_time_ms - this->animate->init_time_ms) >= this->animate->dur)
            {
                this->animate->callback(this->animate->p, this);
                this->animate->init_time_ms += this->animate->dur;
                this->animate->progress_percent = 1.0f;
            }
            else
            {
                this->animate->progress_percent = (float)(this->animate->cur_time_ms -
                                                          this->animate->init_time_ms) /
                                                  (float)this->animate->dur;
            }
        }
        else if (this->animate->repeat_count > 0)
        {
            if ((this->animate->cur_time_ms - this->animate->init_time_ms -
                 this->animate->current_repeat_count * this->animate->dur) >=
                this->animate->dur)
            {
                if (this->animate->current_repeat_count < this->animate->repeat_count)
                {
                    this->animate->callback(this->animate->p, this);
                    this->animate->current_repeat_count ++;
                }
                else
                {
                    this->animate->callback(this->animate->p, this);
                    this->animate->animate = false;
                }
                this->animate->progress_percent = 1.0f;
            }
            else
            {
                this->animate->progress_percent = (float)(this->animate->cur_time_ms - this->animate->init_time_ms -
                                                          this->animate->current_repeat_count * this->animate->dur) /
                                                  (float)this->animate->dur;
            }
        }
    }
}

static void gui_text_prepare(gui_obj_t *obj)
{
    gui_text_t *this = (void *)obj;
    touch_info_t *tp = tp_get_info();
    gui_point3f_t point = {0, 0, 1};
    uint8_t last;

#if 0 // font mat scale min scale function
    if (this->base.matrix->m[0][0] < this->min_scale)
    {
        this->base.matrix->m[0][0] = this->min_scale;
        this->scale_img->base.matrix->m[0][0] = this->min_scale;
    }
    if (this->base.matrix->m[1][1] < this->min_scale)
    {
        this->base.matrix->m[1][1] = this->min_scale;
        this->scale_img->base.matrix->m[1][1] = this->min_scale;
    }
    // gui_log("text scale x %f, y %f ; img scale x %f, y %f",
    //         this->base.matrix->m[0][0],this->base.matrix->m[1][1],
    //         this->scale_img->base.matrix->m[0][0],this->scale_img->base.matrix->m[1][1]);
#endif
    if (this->font_type == GUI_FONT_SRC_IMG)
    {
        return;
    }
    if (this->base.not_show)
    {
        return;
    }

    matrix_multiply_point(obj->matrix, &point);
    this->offset_x = point.p[0];
    this->offset_y = point.p[1];

    gui_text_update_att(obj);

    if (this->mode == LEFT ||
        this->mode == CENTER ||
        this->mode == RIGHT)
    {
        this->base.h = this->font_height;
    }

    switch (tp->type)
    {
    case TOUCH_SHORT:
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
            }
        }
        break;
    default:
        break;
    }

    if (obj->not_show)
    {
        this->layout_refresh = false;
        this->content_refresh = false;
    }

    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_text_t));

    if (last != this->checksum || this->content_refresh)
    {
        gui_fb_change();
    }
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


    if (dc->section_count == 0)
    {
        gui_text_font_load(text, &draw_rect);
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

static void gui_text_end(gui_obj_t *obj)
{

}

static void gui_text_destory(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;

    gui_text_font_destory(text);
}

static void gui_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_text_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_text_draw(obj);
            break;

        case OBJ_END:
            gui_text_end(obj);
            break;

        case OBJ_DESTORY:
            gui_text_destory(obj);
            break;

        default:
            break;
        }
    }
}

void gui_text_ctor(gui_text_t *this,
                   gui_obj_t  *parent,
                   const char *name,
                   int16_t     x,
                   int16_t     y,
                   int16_t     w,
                   int16_t     h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    w = (w == 0 ? parent->w : w);
    h = (h == 0 ? parent->h : h);
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = TEXTBOX;
    root->obj_cb = gui_text_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;
    //for self
    this->mode = LEFT;
    this->inputable = false;
    this->font_mode = FONT_SRC_MEMADDR;
    this->content_refresh = false;
    this->layout_refresh = false;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_text_click(gui_text_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_TOUCH_CLICKED, parameter);
}

void gui_text_set(gui_text_t   *this,
                  void         *text,
                  FONT_SRC_TYPE text_type,
                  gui_color_t   color,
                  uint16_t      length,
                  uint8_t       font_size)
{
    this->font_type = text_type;
    this->content = (uint8_t *)text;
    this->color = color;
    this->len = length;
    this->font_height = font_size;
    this->char_width_sum = 0;
    this->char_line_sum = 0;
    this->content_refresh = true;
    this->layout_refresh = true;
}

void gui_text_set_animate(void    *o,
                          uint32_t dur,
                          int      repeat_count,
                          void    *callback,
                          void    *p)
{
    gui_animate_t *animate = ((gui_text_t *)o)->animate;
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
    ((gui_text_t *)o)->animate = animate;
}

void gui_text_mode_set(gui_text_t *this, TEXT_MODE mode)
{
    this->mode = mode;
    this->layout_refresh = true;
}

void gui_text_input_set(gui_text_t *this, bool inputable)
{
    this->inputable = inputable;
}

void gui_text_set_min_scale(gui_text_t *this, float min_scale)
{
    this->min_scale = min_scale;
}

void gui_text_move(gui_text_t *this, int16_t x, int16_t y)
{
    this->base.x = x;
    this->base.y = y;
    this->layout_refresh = true;
}

void gui_text_size_set(gui_text_t *this, uint8_t height, uint8_t width)
{
    this->font_height = height;
    this->content_refresh = true;
    this->layout_refresh = true;
}

void gui_text_font_mode_set(gui_text_t *this, FONT_SRC_MODE font_mode)
{
    this->font_mode = font_mode;
}

void gui_text_type_set(gui_text_t *this, void *font_source, FONT_SRC_MODE font_mode)
{
    if (font_mode == FONT_SRC_FTL)
    {
        gui_font_mem_init_ftl(font_source);
    }
    this->path = font_source;
    this->font_mode = font_mode;
}

void gui_text_emoji_set(gui_text_t *this, uint8_t *path, uint8_t size)
{
    this->emoji_path = path;
    this->emoji_size = size;
}

void gui_text_encoding_set(gui_text_t *this, TEXT_CHARSET charset)
{
    this->charset = charset;
}

void gui_text_content_set(gui_text_t *this, void *text, uint16_t length)
{
    this->content = (uint8_t *)text;
    this->len = length;
    this->char_width_sum = 0;
    this->char_line_sum = 0;
    this->content_refresh = true;
    this->layout_refresh = true;
}

void gui_text_convert_to_img(gui_text_t *this, GUI_FormatType font_img_type)
{
    if (this->len == 0 || this->font_height == 0)
    {
        return;
    }
    void *img;
    gui_img_t *text_img;
    int16_t img_x = 0, img_y = 0;

    gui_font_scale_destory(this);
    img = gui_text_bmp2img(this, font_img_type, &img_x, &img_y);

    if (this->scale_img == NULL)
    {
        text_img = gui_img_create_from_mem(this, "text_img", img, img_x, img_y, 0, 0);
    }
    else
    {
        text_img = this->scale_img;
        gui_img_set_attribute(text_img, "text_img", img, img_x, img_y);
        text_img->base.w = gui_img_get_width(text_img);
        text_img->base.h = gui_img_get_height(text_img);
    }

    switch (font_img_type)
    {
    case RGB565:
    case RGB888:
        {
            text_img->blend_mode = IMG_FILTER_BLACK;
        }
        break;

    case ARGB8565:
    case ARGB8888:
        {
            text_img->blend_mode = IMG_SRC_OVER_MODE;
        }
        break;

    default:
        break;
    }

    this->scale_img = text_img;
    this->font_type = GUI_FONT_SRC_IMG;
}

gui_text_t *gui_text_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    gui_text_t *text;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_TEXT";
    }

    text = gui_malloc(sizeof(gui_text_t));
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

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
