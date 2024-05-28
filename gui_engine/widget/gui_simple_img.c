/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_simple_img.c
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
#include <string.h>
#include "gui_obj.h"
#include "draw_img.h"
#include "acc_init.h"
#include "tp_algo.h"
#include "gui_simple_img.h"

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

static void gui_simple_img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_simple_img_t *this = (gui_simple_img_t *)obj;
    touch_info_t *tp = tp_get_info();
    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    float x_min, x_max, y_min, y_max;
    int16_t m_x, m_y, m_w, m_h;
    uint8_t last;

    m00 = obj->matrix->m[0][0];
    m01 = obj->matrix->m[0][1];
    m02 = obj->matrix->m[0][2];
    m10 = obj->matrix->m[1][0];
    m11 = obj->matrix->m[1][1];
    m12 = obj->matrix->m[1][2];
    m20 = obj->matrix->m[2][0];
    m21 = obj->matrix->m[2][1];
    m22 = obj->matrix->m[2][2];

    if ((m01 == 0)
        && (m10 == 0)
        && (m20 == 0)
        && (m21 == 0)
        && (m22 == 1)) //follow parent scale and translate, no rotate
    {
        x_min = m02;
        x_max = m02 + m00 * obj->w;
        y_min = m12;
        y_max = m12 + m11 * obj->h;

        if ((x_min > (int)gui_get_screen_width())
            || (x_max < 0)
            || (y_min > (int)gui_get_screen_height())
            || (y_max < 0))
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

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == true)
    {
        gui_simple_img_t *this = (void *)obj;

        if (tp->pressed)
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
            {
                gui_obj_event_set(obj, GUI_EVENT_TOUCH_PRESSED);
                this->press_flag = true;
            }
        }

        if (this->release_flag)
        {
            this->press_flag = false;
            this->release_flag = false;
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_RELEASED);
        }

        if (tp->released && this->press_flag)
        {
            this->release_flag = true;
        }
    }

    gui_image_load_scale(this->draw_img);

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&this->draw_img->inverse);
    gui_image_new_area(this->draw_img, NULL);

    //caculate by obj matrix
    gui_obj_get_area(obj, &m_x, &m_y, &m_w, &m_h);

    this->draw_img->img_target_h = m_h;
    this->draw_img->img_target_w = m_w;

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if ((tp->type == TOUCH_SHORT) && (obj->event_dsc_cnt > 0))
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
        }
    }

    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_simple_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_simple_img_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_simple_img_t *this = (gui_simple_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();

    gui_acc_blit_to_dc(this->draw_img, dc, NULL);

}

static void gui_simple_img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_simple_img_t *this = (gui_simple_img_t *)obj;

    if (this->draw_img != NULL)
    {
        if (gui_image_acc_end_cb != NULL)
        {
            gui_image_acc_end_cb(this->draw_img);
        }

        gui_free(this->draw_img);
        this->draw_img = NULL;
    }
}

static void gui_simple_img_destory(gui_obj_t *obj)
{
    gui_simple_img_t *this = (gui_simple_img_t *)obj;

    if (this->src_mode == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        // free path transforming memory on win
        gui_free(this->data);
#endif
    }
}

static void gui_simple_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_simple_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_simple_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_simple_img_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                gui_simple_img_destory(obj);
            }
            break;

        default:
            {
            }
            break;
        }
    }
}

static void gui_simple_img_ctor(gui_simple_img_t     *this,
                                gui_obj_t            *parent,
                                const char           *name,
                                IMG_SOURCE_MODE_TYPE  src_mode,
                                void                 *path,
                                int16_t               x,
                                int16_t               y,
                                int16_t               w,
                                int16_t               h)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    this->src_mode = src_mode;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_simple_img_cb;
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

    obj->w = _UI_MIN(gui_simple_img_get_width(this), obj->w);
    obj->h = _UI_MIN(gui_simple_img_get_height(this), obj->h);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

uint16_t gui_simple_img_get_width(gui_simple_img_t *this)
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

uint16_t gui_simple_img_get_height(gui_simple_img_t *this)
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

void gui_simple_img_set_location(gui_simple_img_t *this, uint16_t x, uint16_t y)
{
    gui_obj_t *root = (gui_obj_t *)this;
    root->x = x;
    root->y = y;
}

void gui_simple_img_set_mode(gui_simple_img_t *this, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(this != NULL);

    this->blend_mode = mode;
}

void gui_simple_img_set_attribute(gui_simple_img_t *this,
                                  const char       *name,
                                  void             *path,
                                  int16_t           x,
                                  int16_t           y)
{
    GUI_ASSERT(this != NULL);

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
        this->base.name = "gui_simple_img_set_attribute";
    }

    this->data = path;
}

void gui_simple_img_set_opacity(gui_simple_img_t *this, unsigned char opacity_value)
{
    this->opacity_value = opacity_value;
}

void gui_simple_img_set_quality(gui_simple_img_t *this, bool high_quality)
{
    GUI_ASSERT(this != NULL);

    this->high_quality = high_quality;
}

gui_simple_img_t *gui_simple_img_create(void                 *parent,
                                        const char           *name,
                                        void                 *addr,
                                        int16_t               x,
                                        int16_t               y,
                                        int16_t               w,
                                        int16_t               h,
                                        IMG_SOURCE_MODE_TYPE  src_mode_type)
{
    GUI_ASSERT(parent != NULL);

    gui_simple_img_t *this = gui_malloc(sizeof(gui_simple_img_t));
    GUI_ASSERT(this != NULL);

    memset(this, 0x00, sizeof(gui_simple_img_t));
    gui_simple_img_ctor(this, (gui_obj_t *)parent, name, src_mode_type, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
