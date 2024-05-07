/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.c
  * @brief image widget
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
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
#include <guidef.h>
#include <gui_img.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <acc_init.h>
#include <tp_algo.h>
#include "acc_engine.h"


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
void gui_img_set_animate(gui_img_t *this, uint32_t dur, int repeat_count, void *callback,
                         void *p)
{

    gui_animate_t *animate = this->animate;
    if (!(animate))
    {
        animate = gui_malloc(sizeof(gui_animate_t));
    }
    memset((animate), 0, sizeof(gui_animate_t));
    animate->animate = true;
    animate->dur = dur;
    animate->callback = (void (*)(void *))callback;
    animate->repeat_count = repeat_count;
    animate->p = p;
    this->animate = animate;
}
static void img_update_att(gui_obj_t *o)
{
    gui_img_t *obj = (void *)o;
    if (obj->animate && obj->animate->animate)
    {
        uint32_t cur_time_gap = gui_ms_get() - obj->animate->cur_time_ms;
        obj->animate->cur_time_ms = gui_ms_get();
        if (cur_time_gap >= 2 * obj->animate->dur)
        {
            obj->animate->init_time_ms += cur_time_gap;
        }
        if (obj->animate->repeat_count == 0)
        {
            if ((obj->animate->cur_time_ms - obj->animate->init_time_ms) >= obj->animate->dur)
            {
                obj->animate->callback(obj->animate->p);
                obj->animate->animate = false;
                obj->animate->progress_percent = 1.0f;
            }
            else
            {
                obj->animate->progress_percent = (float)(obj->animate->cur_time_ms - obj->animate->init_time_ms) /
                                                 (float)obj->animate->dur;
            }
        }
        else if (obj->animate->repeat_count < 0)
        {
            if ((obj->animate->cur_time_ms - obj->animate->init_time_ms) >= obj->animate->dur)
            {
                obj->animate->callback(obj->animate->p);
                obj->animate->init_time_ms += obj->animate->dur;
                obj->animate->progress_percent = 1.0f;
            }
            else
            {
                obj->animate->progress_percent = (float)(obj->animate->cur_time_ms - obj->animate->init_time_ms) /
                                                 (float)obj->animate->dur;
            }
        }
        else if (obj->animate->repeat_count > 0)
        {
            if (obj->animate->cur_time_ms - obj->animate->init_time_ms -
                obj->animate->current_repeat_count * obj->animate->dur >=
                obj->animate->dur)
            {
                if (obj->animate->current_repeat_count < obj->animate->repeat_count)
                {
                    obj->animate->callback(obj->animate->p);
                    obj->animate->current_repeat_count ++;
                }
                else
                {
                    obj->animate->callback(obj->animate->p);
                    obj->animate->animate = false;
                }
                obj->animate->progress_percent = 1.0f;
            }
            else
            {
                obj->animate->progress_percent = (float)(obj->animate->cur_time_ms - obj->animate->init_time_ms -
                                                         obj->animate->current_repeat_count * obj->animate->dur) /
                                                 (float)obj->animate->dur;
            }
        }
    }
}

static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *this = (gui_img_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    matrix_translate(this->t_x, this->t_y, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    matrix_translate(-this->c_x, -this->c_y, obj->matrix);
    float m00 = obj->matrix->m[0][0];
    float m01 = obj->matrix->m[0][1];
    float m02 = obj->matrix->m[0][2];
    float m10 = obj->matrix->m[1][0];
    float m11 = obj->matrix->m[1][1];
    float m12 = obj->matrix->m[1][2];
    float m20 = obj->matrix->m[2][0];
    float m21 = obj->matrix->m[2][1];
    float m22 = obj->matrix->m[2][2];

    if ((m01 == 0) && \
        (m10 == 0) && \
        (m20 == 0) && \
        (m21 == 0) && \
        (m22 == 1)) //scale and translate, no rotate
    {
        float x_min = m02;
        float x_max = m02 + m00 * obj->w;
        float y_min = m12;
        float y_max = m12 + m11 * obj->h;
        if ((x_min > (int)gui_get_screen_width()) || \
            (x_max < 0) || \
            (y_min > (int)gui_get_screen_height()) || \
            (y_max < 0))
        {
            return;
        }
    }

    this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(this->draw_img, 0x00, sizeof(draw_img_t));
    this->draw_img->data = this->data;
    this->draw_img->blend_mode = this->blend_mode;
    this->draw_img->high_quality = this->high_quality;
    this->draw_img->opacity_value = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

    img_update_att(obj);

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_img_t *b = (void *)obj;
        if (tp->pressed)
        {
            if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);
                b->press_flag = true;
            }
        }

        if (b->release_flag)
        {
            b->press_flag = false;
            b->release_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
        }
        if (tp->released && b->press_flag)
        {
            b->release_flag = true;
        }
    }

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&this->draw_img->inverse);
    gui_image_load_scale(this->draw_img);
    gui_image_new_area(this->draw_img, NULL);

    int16_t m_x, m_y, m_w, m_h;//caculate by obj matrix
    gui_obj_get_area(obj, &m_x, &m_y, &m_w, &m_h);
    this->draw_img->img_target_h = m_h;
    this->draw_img->img_target_w = m_w;


    if (gui_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->type == TOUCH_SHORT) && (obj->event_dsc_cnt > 0))
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
        }

    }


    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}


static void img_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *this = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();

    if (gui_get_acc() != NULL)
    {
        gui_acc_blit_to_dc(this->draw_img, dc, NULL);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}

static void img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    if (img->draw_img != NULL)
    {
        if (img->draw_img->line != NULL)
        {
            gui_free(img->draw_img->line);
            img->draw_img->line = NULL;
        }
        gui_free(img->draw_img);
        img->draw_img = NULL;
    }

}

static void img_destory(gui_obj_t *obj)
{
    gui_img_t *this = (gui_img_t *)obj;

    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }

    if (this->src_mode == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        // free path transforming memory on win
        gui_free(this->data);
#endif
    }
}

static gui_rgb_data_head_t gui_img_get_header(gui_img_t *this)
{
    struct gui_rgb_data_head head = {0};

    if (this->src_mode == IMG_SRC_FILESYS)
    {
        int fd = gui_fs_open(this->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)this->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        memcpy(&head, this->data, sizeof(head));
    }

    return head;
}

static void gui_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            img_prepare(obj);
            break;

        case OBJ_DRAW:
            img_draw_cb(obj);
            break;

        case OBJ_END:
            img_end(obj);
            break;

        case OBJ_DESTORY:
            img_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_img_ctor(gui_img_t *this,
                         gui_obj_t *parent,
                         const char *name,
                         IMG_SOURCE_MODE_TYPE src_mode,
                         void *path,
                         int16_t x,
                         int16_t y,
                         int16_t w,
                         int16_t h)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = (gui_obj_t *)this;

    this->src_mode = src_mode;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_img_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
    obj->type = IMAGE_FROM_MEM;

    if (src_mode == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        path = gui_filepath_transforming(path);
#endif
        this->data = (void *)path;
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        this->data = (void *)path;
    }

    this->opacity_value = 255;
    this->blend_mode = IMG_FILTER_BLACK;
    this->opacity_value = UINT8_MAX;

    this->scale_x = 1.0f;
    this->scale_y = 1.0f;

    obj->w = _UI_MIN(gui_img_get_width(this), obj->w);
    obj->h = _UI_MIN(gui_img_get_height(this), obj->h);

    gui_rgb_data_head_t head = gui_img_get_header(this);
    if (head.resize == 0)
    {
        gui_img_scale(this, 1, 1);
    }
    else if (head.resize == 1) //unsigned char resize:2;//0-no resize;1-50%(x&y);2-70%;3-80%
    {
        gui_img_scale(this, 2, 2);
        gui_log("resize image!! \n");
    }
    else if (head.resize == 2)
    {
        gui_img_scale(this, 10.0f / 7.0f, 10.0f / 7.0f);
        gui_log("resize image!! \n");
    }
    else if (head.resize == 3)
    {
        gui_img_scale(this, 10.0f / 8.0f, 10.0f / 8.0f);
        gui_log("resize image!! \n");
    }
}




/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_img_t *gui_img_create_from_mem(void *parent,
                                   const char *name,
                                   void *addr,
                                   int16_t x,
                                   int16_t y,
                                   int16_t w,
                                   int16_t h)
{
    GUI_ASSERT(parent != NULL);
    gui_img_t *img = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_img_t));

    gui_img_ctor(img, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}

gui_img_t *gui_img_create_from_fs(void *parent,
                                  const char *name,
                                  void *file,
                                  int16_t x,
                                  int16_t y,
                                  int16_t w,
                                  int16_t h)
{
    GUI_ASSERT(parent != NULL);
    gui_img_t *img = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_img_t));

    gui_img_ctor(img, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, file, x, y, w, h);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}

uint16_t gui_img_get_width(gui_img_t *this)
{

    if (this->src_mode == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.w = 0;
        int fd = gui_fs_open(this->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)this->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
        return head.w;
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->data;
        return head->w;
    }
    return 0;
}

uint16_t gui_img_get_height(gui_img_t *this)
{
    if (this->src_mode == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.h = 0;
        int fd = gui_fs_open(this->data,  0);
        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)this->data);
        }
        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
        return head.h;
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->data;
        return head->h;
    }
    return 0;
}

void gui_img_set_location(gui_img_t *img, uint16_t x, uint16_t y)
{
    gui_obj_t *root = (gui_obj_t *)img;
    root->x = x;
    root->y = y;
}

void gui_img_set_mode(gui_img_t *this, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(this != NULL);
    this->blend_mode = mode;
}

void gui_img_set_quality(gui_img_t *this, bool high_quality)
{
    GUI_ASSERT(this != NULL);
    this->high_quality = high_quality;
}

void gui_img_set_attribute(gui_img_t *img,
                           const char *name,
                           void *path,
                           int16_t x,
                           int16_t y)
{
    GUI_ASSERT(img != NULL);
    gui_img_t *this = img;

    if (!name && !path)
    {
        return;
    }
    this->base.x = x;
    this->base.y = y;
    if (name != NULL)
    {
        this->base.name = name;
    }
    else
    {
        this->base.name = "gui_img_set_attribute";
    }
    this->data = path;

}

void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value)
{
    this->opacity_value = opacity_value;
}

void gui_img_rotation(gui_img_t *img, float degrees, float c_x, float c_y)
{
    GUI_ASSERT(img != NULL);
    img->degrees = degrees;
    img->c_x = c_x;
    img->c_y = c_y;
}

void gui_img_scale(gui_img_t *img, float scale_x, float scale_y)
{
    GUI_ASSERT(img != NULL);
    if (scale_x > 0 && scale_y > 0)
    {
        img->scale_x = scale_x;
        img->scale_y = scale_y;
    }
}

void gui_img_translate(gui_img_t *img, float t_x, float t_y)
{
    GUI_ASSERT(img != NULL);
    img->t_x = t_x;
    img->t_y = t_y;
}

void gui_img_skew_x(gui_img_t *img, float degrees)
{

}

void gui_img_skew_y(gui_img_t *img, float degrees)
{

}


static void rect_copy(uint8_t *target, uint8_t *source, uint32_t x, uint32_t y, uint32_t w,
                      uint32_t h, gui_dispdev_t *dc)
{
    uint16_t byte = dc->bit_depth / 8;
    for (uint32_t i = 0; i < h; i++)
    {
        memcpy(target + i * w * byte, source + i * dc->fb_width * byte, w * byte);
    }
}
static void virtual_dc_update(struct gui_dispdev *dc)
{
    if (dc->virtual_lcd_update != NULL)
    {
        dc->virtual_lcd_update(dc);
        return;
    }
    float scale = 0.7f;
    uint16_t w = dc->fb_width * scale;
    uint16_t h = dc->fb_height * scale;
    uint16_t byte = dc->bit_depth / 8;
    uint32_t total_section_cnt = (dc->screen_height / dc->fb_height + ((
                                                                           dc->screen_height % dc->fb_height) ? 1 : 0));
    if (dc->type == DC_SINGLE)
    {
        for (uint16_t y = 0; y < h; y++)
        {
            memcpy(dc->shot_buf + 8 + y * w * byte, dc->frame_buf + y * dc->fb_width * byte, w * byte);
        }
        gui_log("[GUI warning] please use user method for improve! \n");
    }
    else if (dc->type == DC_RAMLESS)
    {
        if (dc->section_count == 0)
        {
            uint8_t *dst = 8 + dc->shot_buf + w *  dc->fb_height * dc->section_count * byte;
            rect_copy(dst, dc->frame_buf, 0, 0, w, dc->fb_height, dc);
        }
        else if (dc->section_count == total_section_cnt - 1)
        {
            uint32_t last_height = dc->screen_height - dc->section_count * dc->fb_height;
            uint8_t *dst = 8 + dc->shot_buf + w *  dc->fb_height * dc->section_count * byte;
            rect_copy(dst, dc->frame_buf, 0, 0, w, last_height, dc);
            gui_log("[GUI warning] please use user method for improve! \n");
        }
        else
        {
            uint8_t *dst = 8 + dc->shot_buf + w *  dc->fb_height * dc->section_count * byte;
            rect_copy(dst, dc->frame_buf, 0, 0, w, dc->fb_height, dc);
        }
    }


}
void gui_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_dispdev_t *dc_bak = gui_malloc(sizeof(gui_dispdev_t));
    gui_matrix_t *matrix_bak = gui_malloc(sizeof(gui_matrix_t));
    memcpy(dc_bak, dc, sizeof(gui_dispdev_t));
    memcpy(matrix_bak, obj->matrix, sizeof(gui_matrix_t));
    matrix_scale(0.7f, 0.7f, obj->matrix);

    dc->bit_depth = 16;

    dc->lcd_update = virtual_dc_update;
    dc->shot_buf = shot_buf;


    gui_fb_change();
    gui_fb_disp(obj);
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)(dc->shot_buf);

    head->scan = 0;
    head->align = 0;
    head->resize = 2;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = 0;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = obj->w * 0.7f;
    head->h = obj->h * 0.7f;

    memcpy(dc, dc_bak, sizeof(gui_dispdev_t));
    memcpy(obj->matrix, matrix_bak, sizeof(gui_matrix_t));
    gui_free(dc_bak);
    gui_free(matrix_bak);

}



/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
