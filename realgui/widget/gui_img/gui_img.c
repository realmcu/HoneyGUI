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
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "acc_init.h"
#include "tp_algo.h"
#include "gui_fb.h"

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
 * @brief Prepares the GUI image associated with the specified object.
 *
 * This function performs necessary initialization and setup for an image
 * within a GUI object. It may involve configuring settings to ensure the
 * image is ready for display.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object for which the image
 *                preparation is conducted. This object must be properly
 *                initialized before passing to this function.
 */
static void gui_img_prepare(gui_obj_t *obj)
{
    uint8_t last;
    gui_img_t *this;
    touch_info_t *tp;
    GUI_UNUSED(tp);

    GUI_ASSERT(obj != NULL);

    this = (gui_img_t *)obj;
    tp = tp_get_info();

    matrix_translate(this->t_x, this->t_y, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    matrix_translate(-this->f_x, -this->f_y, obj->matrix);

    if (gui_obj_out_screen(obj))
    {
        return;
    }

    this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(this->draw_img, 0x00, sizeof(draw_img_t));

    this->draw_img->data = this->data;
    this->draw_img->blend_mode = this->blend_mode;
    this->draw_img->high_quality = this->high_quality;
    this->draw_img->opacity_value = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;


    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT);
    // gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED);

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&this->draw_img->inverse);
    draw_img_load_scale(this->draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode);


    draw_img_new_area(this->draw_img, NULL);


    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_img_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

/**
 * @brief Callback function for drawing the GUI image.
 *
 * This function is called to render the image within the specified GUI object.
 * It handles all the necessary operations to display the image.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object that contains the image
 *                to be drawn. The image should be prepared in advance using
 *                `gui_img_prepare`.
 */
static void gui_img_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_img_t *this = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();

    // cache img to buffer

    draw_img_cache(this->draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode);

    if (this->need_clip)
    {
        gui_rect_t rect = {0};
        gui_obj_get_clip_rect(obj, &rect);
        gui_acc_blit_to_dc(this->draw_img, dc, &rect);
    }
    else
    {
        gui_rect_t rect = {0};
        gui_obj_get_clip_rect(obj, &rect);
        gui_acc_blit_to_dc(this->draw_img, dc, &rect);
        // gui_acc_blit_to_dc(this->draw_img, dc, NULL);
    }


    // release img if cached
    draw_img_free(this->draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode);

}
/**
 * @brief Cleans up resources associated with the GUI image.
 *
 * This function releases any resources, such as memory or handles, that were
 * allocated for the image within the specified GUI object.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object associated with the image
 *                whose resources are being cleaned up.
 */
static void gui_img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_img_t *this = (gui_img_t *)obj;

    if (this->draw_img != NULL)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(this->draw_img);
        }
        gui_free(this->draw_img);
        this->draw_img = NULL;
    }
}


/**
 * @brief Destroys the GUI image and cleans up associated resources.
 *
 * This function is responsible for destroying the image associated with
 * the specified GUI object. It ensures that any resources allocated for
 * the image, such as memory, handles, or buffers, are properly released
 * to prevent memory leaks.
 *
 * @param[in] obj A pointer to the `gui_obj_t` object whose image is to
 *                be destroyed. The object should have been previously
 *                prepared and possibly displayed using related functions.
 *
 * @note After calling this function, the image data associated with the
 *       object is no longer valid and should not be used. This function
 *       should be called when the image is no longer needed.
 */
static void gui_img_destroy(gui_obj_t *obj)
{
    gui_img_t *this = (gui_img_t *)obj;
    GUI_UNUSED(this);
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
    else if (this->src_mode == IMG_SRC_FTL)
    {
        uint32_t base = (uint32_t)(uintptr_t)this->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
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
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_img_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_img_ctor(gui_img_t            *this,
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
    this->f_x = 0;
    this->f_y = 0;
    this->t_x = 0;
    this->t_y = 0;
    this->scale_x = 1;
    this->scale_y = 1;
    this->degrees = 0;
    this->high_quality = false;
    this->need_clip = false;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_img_cb;
    obj->has_input_prepare_cb = false;
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
        this->filename = (void *)path;
    }
    else if (src_mode == IMG_SRC_MEMADDR)
    {
        this->data = (void *)path;
    }
    else if (src_mode == IMG_SRC_FTL)
    {
        this->data = (void *)path;
        this->ftl = (void *)path;
    }

    this->opacity_value = 255;
    this->blend_mode = IMG_FILTER_BLACK;
    this->opacity_value = UINT8_MAX;


    obj->w = gui_img_get_width(this);
    obj->h = gui_img_get_height(this);

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
gui_img_t *gui_img_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_img_t *this = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_img_t));
    gui_img_ctor(this, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;

    return this;
}

gui_img_t *gui_img_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_img_t *this = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_img_t));
    gui_img_ctor(this, (gui_obj_t *)parent, name, IMG_SRC_FTL, ftl, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;

    return this;
}


gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_img_t *this = gui_malloc(sizeof(gui_img_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_img_t));
    gui_img_ctor(this, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, file, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;

    return this;
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
    else if (this->src_mode == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uint32_t)(uintptr_t)this->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
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
    else if (this->src_mode == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uint32_t)(uintptr_t)this->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
        return head.h;
    }
    else if (this->src_mode == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->data;
        return head->h;
    }

    return 0;
}

void gui_img_refresh_size(gui_img_t *this)
{
    // gui_image_load_scale(this->draw_img);
    this->base.w = gui_img_get_width(this);
    this->base.h = gui_img_get_height(this);
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

void gui_img_set_attribute(gui_img_t  *this,
                           const char *name,
                           void       *path,
                           int16_t     x,
                           int16_t     y)
{
    GUI_ASSERT(this != NULL);

    if ((!name) && (!path))
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
void gui_img_set_image_data(gui_img_t  *this, const uint8_t *file_pointer)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(file_pointer != NULL);
    this->data = (void *)file_pointer;
}
const uint8_t *gui_img_get_image_data(gui_img_t  *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->data;
}
void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value)
{
    this->opacity_value = opacity_value;
}


void gui_img_set_focus(gui_img_t *this, float f_x, float f_y)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);

    this->f_x = f_x;
    this->f_y = f_y;
}


void gui_img_rotation(gui_img_t *this, float degrees)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);

    this->degrees = degrees;
}

void gui_img_scale(gui_img_t *this, float scale_x, float scale_y)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    this->scale_x = scale_x;
    this->scale_y = scale_y;
}

void gui_img_translate(gui_img_t *this, float t_x, float t_y)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    this->t_x = t_x;
    this->t_y = t_y;
}
float gui_img_get_scale_x(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->scale_x;
}
float gui_img_get_scale_y(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->scale_y;
}
float gui_img_get_degrees(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->degrees;
}
float gui_img_get_c_x(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->f_x;

}
float gui_img_get_c_y(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->f_y;
}
float gui_img_get_t_x(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->t_x;
}
float gui_img_get_t_y(gui_img_t *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    return this->t_y;
}

void gui_img_skew_x(gui_img_t *this, float degrees)
{

}

void gui_img_skew_y(gui_img_t *this, float degrees)
{

}

