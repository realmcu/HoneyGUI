/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_progressbar.c
  * @brief progressbar widget
 * @details progressbar widget
  * @author luke_sun@realsil.com.cn
  * @date 2023/11/07
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
#include "gui_progressbar.h"
#include "gui_obj.h"
#include "gui_img.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define IMAGE_ARRAY 1
#define IMAGE_SCOPE 2
#define IMAGE_THUMB 3
/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_progressbar_movie_ctor_core(gui_progressbar_t     *this,
                                            gui_obj_t             *parent,
                                            void                 **picture_array,
                                            uint16_t               array_number,
                                            int16_t                x,
                                            int16_t                y,
                                            IMG_SOURCE_MODE_TYPE   src_mode)
{
    gui_obj_ctor(&(this->base), parent, "progressbar_movie", x, y, 0, 0);

    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = array_number - 1;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }

    this->src_mode = src_mode;

    if (src_mode == IMG_SRC_FILESYS)
    {
        this->c = (void *)gui_img_create_from_fs(this, "pro", picture_array[0], 0, 0, 0, 0);
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        this->c = (void *)gui_img_create_from_mem(this, "pro", picture_array[0], 0, 0, 0, 0);
    }
    this->image_type = IMAGE_ARRAY;
    this->color_hl = (uint32_t)(uintptr_t)picture_array;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress)
{
    this->per = ((float)progress) / ((float)this->max);
    if (this->per != this->per_history)
    {
        this->per_history = this->per;
        //gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_5);
        GUI_ASSERT(0);
    }

    if (this->image_type == IMAGE_SCOPE)
    {
        if (this->color == 1)
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = progress;
        }
        else
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = (1.0f - this->per) * this->max;
        }
    }
    else if (this->image_type == IMAGE_ARRAY)
    {
        size_t p = progress;

        GUI_TYPE(gui_img_t, this->c)->data = ((void **)(uintptr_t)(this->color_hl))[p];
        GET_BASE(this->c)->w = gui_img_get_width((void *)(this->c)) + 1;
        GET_BASE(this->c)->h = gui_img_get_height((void *)(this->c));

    }
    else if (this->image_type == IMAGE_THUMB)
    {
        size_t p = progress;


        GET_BASE(this->c)->x = p;

    }
}

size_t gui_progressbar_get_progress(gui_progressbar_t *this)
{
    return (size_t)(this->per  * ((float)this->max));
}

void gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage)
{
    this->per = percentage;
    if (this->per != this->per_history)
    {
        this->per_history = this->per;
        //gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_5);
        GUI_ASSERT(0);
    }

    if (this->image_type == IMAGE_SCOPE)
    {
        if (this->color == 1)
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = percentage * this->max;
        }
        else
        {
            GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = (1.0f - percentage) * this->max;
        }
    }
    else if (this->image_type == IMAGE_ARRAY)
    {
        size_t p = percentage * (this->max - 1);

        GUI_TYPE(gui_img_t, this->c)->data = ((void **)(uintptr_t)(this->color_hl))[p];
        GET_BASE(this->c)->w = gui_img_get_width((void *)(this->c)) + 1;
        GET_BASE(this->c)->h = gui_img_get_height((void *)(this->c));
    }
    else if (this->image_type == IMAGE_THUMB)
    {
        size_t p = percentage * (this->max);


        GET_BASE(this->c)->x = p;

    }
}

float gui_progressbar_get_percentage(gui_progressbar_t *this)
{
    return this->per;
}

size_t gui_progressbar_get_max(gui_progressbar_t *this)
{
    return this->max;
}

void gui_progressbar_ctor(gui_progressbar_t *this,
                          gui_obj_t         *parent,
                          const char        *filename,
                          int16_t            x,
                          int16_t            y,
                          int16_t            w,
                          int16_t            h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);

    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = w;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
}

void gui_progressbar_h_img_ctor(gui_progressbar_t *this,
                                gui_obj_t         *parent,
                                void              *picture,
                                int16_t            x,
                                int16_t            y)
{

    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;
    this->color = 1;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->c = (void *)gui_img_scope_create(this, "progressbar_h", picture, 0, 0);
    this->image_type = IMAGE_SCOPE;
    int w = gui_img_get_width((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = gui_img_get_height((void *)this->c);
    GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = this->max * this->per;
    GUI_TYPE(gui_img_scope_t, this->c)->scope_y2 = GET_BASE(this)->h;
}
void gui_progressbar_h_thumb_ctor(gui_progressbar_t *this,
                                  gui_obj_t         *parent,
                                  void              *picture,
                                  int16_t            x,
                                  int16_t            y,
                                  uint16_t bar_width
                                 )
{

    gui_obj_ctor(&(this->base), parent, "h_thumb_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;

    this->color = 1;
    this->image_type = IMAGE_THUMB;
    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->c = (void *)gui_img_create_from_mem(this, "_thumb", picture, 0, 0, 0, 0);
    int w = gui_img_get_width((void *)this->c);
    this->max = bar_width - w;
    GET_BASE(this)->w = bar_width;
    this->thumb_width = w;
    GET_BASE(this)->h = gui_img_get_height((void *)this->c);
}
void gui_progressbar_h_img_ctor_from_ftl(gui_progressbar_t *this,
                                         gui_obj_t         *parent,
                                         void              *picture,
                                         int16_t            x,
                                         int16_t            y)
{

    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;
    this->color = 1;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->c = (void *)gui_img_scope_create_from_ftl(this, "progressbar_h", picture, 0, 0);
    this->image_type = IMAGE_SCOPE;
    int w = gui_img_get_width((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = w;
    GET_BASE(this)->h = gui_img_get_height((void *)this->c);
    GUI_TYPE(gui_img_scope_t, this->c)->scope_x2 = this->max * this->per;
    GUI_TYPE(gui_img_scope_t, this->c)->scope_y2 = GET_BASE(this)->h;
}



void gui_progressbar_v_img_ctor(gui_progressbar_t *this,
                                gui_obj_t         *parent,
                                void              *picture,
                                int16_t            x,
                                int16_t            y)
{
    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->c = (void *)gui_img_scope_create(this, "progressbar_v", picture, 0, 0);
    this->image_type = IMAGE_SCOPE;
    int w = gui_img_get_height((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = gui_img_get_width((void *)this->c);
    GET_BASE(this)->h = w;
    GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = this->max * (1.0f - this->per);
}

void gui_progressbar_v_img_ctor_from_ftl(gui_progressbar_t *this,
                                         gui_obj_t         *parent,
                                         void              *picture,
                                         int16_t            x,
                                         int16_t            y)
{
    gui_obj_ctor(&(this->base), parent, "h_img_pro", x, y, 0, 0);
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = 0;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
    this->c = (void *)gui_img_scope_create_from_ftl(this, "progressbar_v", picture, 0, 0);
    this->image_type = IMAGE_SCOPE;
    int w = gui_img_get_height((void *)this->c);
    this->max = w;
    GET_BASE(this)->w = gui_img_get_width((void *)this->c);
    GET_BASE(this)->h = w;
    GUI_TYPE(gui_img_scope_t, this->c)->scope_y1 = this->max * (1.0f - this->per);
}



gui_progressbar_t *gui_progressbar_img_h_create(void    *parent,
                                                void    *picture,
                                                int16_t  x,
                                                int16_t  y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_h_img_ctor(this, parent, picture, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_progressbar_t *gui_progressbar_img_h_create_from_ftl(void    *parent,
                                                         void    *picture,
                                                         int16_t  x,
                                                         int16_t  y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_h_img_ctor_from_ftl(this, parent, picture, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_progressbar_t *gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_v_img_ctor(this, parent, picture, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_progressbar_t *gui_progressbar_img_v_create_from_ftl(void *parent, void *picture, int16_t x,
                                                         int16_t y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_v_img_ctor_from_ftl(this, parent, picture, x, y);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

void gui_progressbar_v_ctor(gui_progressbar_t *this,
                            gui_obj_t         *parent,
                            const char        *filename,
                            int16_t            x,
                            int16_t            y,
                            int16_t            w,
                            int16_t            h)
{

    gui_obj_ctor(&(this->base), parent, filename, x, y, w, h);

    this->color = 0xffffffff;
    this->per = 0.3f;
    this->base.type = PROGRESSBAR;
    this->max = h;

    gui_list_init(&(((gui_obj_t *)this)->child_list));
    if ((((gui_obj_t *)this)->parent) != ((void *)0))
    {
        gui_list_insert_before(&((((gui_obj_t *)this)->parent)->child_list),
                               &(((gui_obj_t *)this)->brother_list));
    }
}

void gui_progressbar_movie_ctor(gui_progressbar_t  *this,
                                gui_obj_t          *parent,
                                void              **picture_array,
                                uint16_t            array_number,
                                int16_t             x,
                                int16_t             y)
{
    gui_progressbar_movie_ctor_core(this, parent, picture_array, array_number, x, y, IMG_SRC_MEMADDR);
}

void gui_progressbar_movie_ctor_fs(gui_progressbar_t *this, gui_obj_t *parent,
                                   void  **picture_array,
                                   uint16_t array_number,
                                   int16_t x,
                                   int16_t y)
{
    gui_progressbar_movie_ctor_core(this, parent, picture_array, array_number, x, y, IMG_SRC_FILESYS);
}

gui_progressbar_t *gui_progressbar_movie_create(void      *parent,
                                                void     **picture_array,
                                                uint16_t   array_length,
                                                int16_t    x,
                                                int16_t    y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_movie_ctor_core(this, parent, picture_array, array_length, x, y, IMG_SRC_MEMADDR);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}

gui_progressbar_t *gui_progressbar_movie_create_from_fs(void      *parent,
                                                        void     **picture_array,
                                                        uint16_t   array_length,
                                                        int16_t    x,
                                                        int16_t    y)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_movie_ctor_core(this, parent, picture_array, array_length, x, y, IMG_SRC_FILESYS);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}
gui_progressbar_t *gui_progressbar_thumb_h_create(void    *parent,
                                                  void    *picture_pointer,
                                                  int16_t  x,
                                                  int16_t  y,
                                                  uint16_t bar_width)
{
    gui_progressbar_t *this = gui_malloc(sizeof(gui_progressbar_t));
    memset(this, 0, sizeof(gui_progressbar_t));
    gui_progressbar_h_thumb_ctor(this, parent, picture_pointer, x, y, bar_width);
    ((gui_obj_t *)this)->create_done = 1;

    return this;
}
