/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_glass_effect.c
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
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "gui_glass.h"

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
static void gui_glass_effect_prepare(gui_obj_t *obj)
{
    uint8_t last;
    gui_glass_effect_t *_this;
    touch_info_t *tp;
    GUI_UNUSED(tp);

    GUI_ASSERT(obj != NULL);

    _this = (gui_glass_effect_t *)obj;
    tp = tp_get_info();
    matrix_identity(obj->matrix);
    matrix_translate(_this->t_x + _this->base.x, _this->t_y + _this->base.y, obj->matrix);

    if (gui_obj_out_screen(obj))
    {
        return;
    }

    _this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(_this->draw_img, 0x00, sizeof(draw_img_t));

    _this->draw_img->data = _this->data;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_MOVE_RIGHT);
    // gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED);

    memcpy(&_this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&_this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&_this->draw_img->inverse);
    draw_img_load_scale(_this->draw_img, (IMG_SOURCE_MODE_TYPE)_this->storage_type);


    draw_img_new_area(_this->draw_img, NULL);


    last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, sizeof(gui_glass_effect_t));

    if (last != _this->checksum)
    {
        gui_fb_change();
    }
}

void gui_put_glass_on_bg(draw_img_t *image, struct gui_dispdev *dc)
{
    int32_t x_max = (image->img_target_w + image->img_target_x - 1);
    int32_t y_max = (image->img_target_h + image->img_target_y - 1);
    int32_t x_min = image->img_target_x;
    int32_t y_min = image->img_target_y;
    int32_t place_x = x_min;
    int32_t place_y = y_min;
    if (dc->section.y2 < y_min || dc->section.y1 > y_max || dc->section.x2 < x_min ||
        dc->section.x1 > x_max)
    {
        return;
    }
    int area_x_max = x_max > dc->section.x2 ? dc->section.x2 : x_max;
    int area_x_min = x_min < dc->section.x1 ? dc->section.x1 : x_min;
    int area_y_max = y_max > dc->section.y2 ? dc->section.y2 : y_max;
    int area_y_min = y_min < dc->section.y1 ? dc->section.y1 : y_min;
    int actual_w = area_x_max - area_x_min + 1;
    int actual_h = area_y_max - area_y_min + 1;
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;

    uint32_t glass_width = head->w;
    int8_t *glass_offset = (int8_t *)image->data + 8;
    uint8_t *bg = (uint8_t *)dc->frame_buf;
    uint32_t *bg_u32 = (uint32_t *)bg;
    uint16_t *temp_u32 = (uint16_t *)gui_malloc(actual_w * actual_h * 4);
    int line_offset =  0;

    for (int y = area_y_min; y <= area_y_max; y++)
    {
        uint32_t glass_y = y - place_y;
        uint32_t offset = (glass_y * glass_width + area_x_min - place_x) * 2;
        for (int x = area_x_min; x <= area_x_max; x++)
        {
            int x1 = x + glass_offset[offset++];
            int y1 = y + glass_offset[offset++];
            if (x1 < 0 || y1 < 0 || x1 >= dc->fb_width || y1 >= dc->fb_height)
            {
                temp_u32[line_offset++] = 0;
            }
            else
            {
                temp_u32[line_offset++] =  bg_u32[y1 * dc->fb_width + x1];
            }
        }
    }
    for (int y = area_y_min; y <= area_y_max; y++)
    {
        memcpy(bg + y * dc->fb_width * 4 + area_x_min * 4, temp_u32 + actual_w * (y - area_y_min),
               actual_w * 4);
    }
    gui_free(temp_u32);
}

static void gui_put_glass_on_bg_16(draw_img_t *image, struct gui_dispdev *dc)
{
    int32_t x_max = (image->img_target_w + image->img_target_x - 1);
    int32_t y_max = (image->img_target_h + image->img_target_y - 1);
    int32_t x_min = image->img_target_x;
    int32_t y_min = image->img_target_y;
    int32_t place_x = x_min;
    int32_t place_y = y_min;
    if (dc->section.y2 < y_min || dc->section.y1 > y_max || dc->section.x2 < x_min ||
        dc->section.x1 > x_max)
    {
        return;
    }
    int area_x_max = x_max > dc->section.x2 ? dc->section.x2 : x_max;
    int area_x_min = x_min < dc->section.x1 ? dc->section.x1 : x_min;
    int area_y_max = y_max > dc->section.y2 ? dc->section.y2 : y_max;
    int area_y_min = y_min < dc->section.y1 ? dc->section.y1 : y_min;
    int actual_w = area_x_max - area_x_min + 1;
    int actual_h = area_y_max - area_y_min + 1;
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
    uint32_t glass_width = head->w;
    int8_t *glass_offset = (int8_t *)image->data + 8;
    uint8_t *bg = (uint8_t *)dc->frame_buf;
    uint16_t *bg_u16 = (uint16_t *)bg;
    uint16_t *temp_u16 = (uint16_t *)gui_malloc(actual_w * actual_h * 2);
    int line_offset =  0;
    for (int y = area_y_min; y <= area_y_max; y++)
    {
        uint32_t glass_y = y - place_y;
        uint32_t offset = (glass_y * glass_width + area_x_min - place_x) * 2;
        for (int x = area_x_min; x <= area_x_max; x++)
        {
            int x1 = x + glass_offset[offset++];
            int y1 = y + glass_offset[offset++];
            if (x1 < 0 || y1 < 0 || x1 >= dc->fb_width || y1 >= dc->fb_height)
            {
                temp_u16[line_offset++] = 0;
            }
            else
            {
                temp_u16[line_offset++] =  bg_u16[y1 * dc->fb_width + x1];
            }
        }
    }
    for (int y = area_y_min; y <= area_y_max; y++)
    {
        memcpy(bg + y * dc->fb_width * 2 + area_x_min * 2, temp_u16 + actual_w * (y - area_y_min),
               actual_w * 2);
    }
    gui_free(temp_u16);
}

static void gui_glass_draw(draw_img_t *image, struct gui_dispdev *dc)
{
    if (dc->type == DC_RAMLESS)
    {
        return;
    }
    if (dc->bit_depth == 32)
    {
        gui_put_glass_on_bg(image, dc);
    }
    else if (dc->bit_depth == 16)
    {
        gui_put_glass_on_bg_16(image, dc);
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
 *                `gui_glass_effect_prepare`.
 */
static void gui_glass_effect_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_glass_effect_t *_this = (gui_glass_effect_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();
    gui_glass_draw(_this->draw_img, dc);
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
static void gui_glass_effect_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_glass_effect_t *_this = (gui_glass_effect_t *)obj;

    if (_this->draw_img != NULL)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(_this->draw_img);
        }
        gui_free(_this->draw_img);
        _this->draw_img = NULL;
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
static void gui_glass_effect_destroy(gui_obj_t *obj)
{
    gui_glass_effect_t *_this = (gui_glass_effect_t *)obj;
    GUI_UNUSED(_this);
}

static void gui_glass_effect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_glass_effect_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_glass_effect_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_glass_effect_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_glass_effect_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_glass_effect_ctor(gui_glass_effect_t            *_this,
                                  gui_obj_t            *parent,
                                  const char           *name,
                                  IMG_SOURCE_MODE_TYPE  storage_type,
                                  void                 *path,
                                  int16_t               x,
                                  int16_t               y,
                                  int16_t               w,
                                  int16_t               h)
{
    gui_obj_t *obj = (gui_obj_t *)_this;

    _this->storage_type = storage_type;
    _this->t_x = 0;
    _this->t_y = 0;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_glass_effect_cb;
    obj->has_input_prepare_cb = false;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
    obj->type = IMAGE_FROM_MEM;

    if (storage_type == IMG_SRC_FILESYS)
    {
#ifdef _WIN32
        path = gui_filepath_transforming(path);
#endif
        _this->data = (void *)path;
        _this->filename = (void *)path;
    }
    else if (storage_type == IMG_SRC_MEMADDR)
    {
        _this->data = (void *)path;
    }
    else if (storage_type == IMG_SRC_FTL)
    {
        _this->data = (void *)path;
        _this->ftl = (void *)path;
    }



    obj->w = gui_glass_effect_get_width(_this);
    obj->h = gui_glass_effect_get_height(_this);
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_glass_effect_t *gui_glass_effect_create_from_mem(void       *parent,
                                                     const char *name,
                                                     void       *addr,
                                                     int16_t     x,
                                                     int16_t     y,
                                                     int16_t     w,
                                                     int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_effect_t *_this = gui_malloc(sizeof(gui_glass_effect_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_effect_t));
    gui_glass_effect_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}

gui_glass_effect_t *gui_glass_effect_create_from_ftl(void       *parent,
                                                     const char *name,
                                                     void       *ftl,
                                                     int16_t     x,
                                                     int16_t     y,
                                                     int16_t     w,
                                                     int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_effect_t *_this = gui_malloc(sizeof(gui_glass_effect_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_effect_t));
    gui_glass_effect_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FTL, ftl, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}


gui_glass_effect_t *gui_glass_effect_create_from_fs(void       *parent,
                                                    const char *name,
                                                    void       *file,
                                                    int16_t     x,
                                                    int16_t     y,
                                                    int16_t     w,
                                                    int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_effect_t *_this = gui_malloc(sizeof(gui_glass_effect_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_effect_t));
    gui_glass_effect_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, file, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}

uint16_t gui_glass_effect_get_width(gui_glass_effect_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.w = 0;
        int fd = gui_fs_open(_this->data,  0);

        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)_this->data);
        }

        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
        return head.w;
    }
    else if (_this->storage_type == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uint32_t)(uintptr_t)_this->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
        return head.w;
    }
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)_this->data;
        return head->w;
    }

    return 0;
}

uint16_t gui_glass_effect_get_height(gui_glass_effect_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.h = 0;
        int fd = gui_fs_open(_this->data,  0);

        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)_this->data);
        }

        gui_fs_read(fd, &head, sizeof(head));
        gui_fs_close(fd);
        return head.h;
    }
    else if (_this->storage_type == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uint32_t)(uintptr_t)_this->data;
        gui_ftl_read(base, (uint8_t *)&head, sizeof(gui_rgb_data_head_t));
        return head.h;
    }
    else if (_this->storage_type == IMG_SRC_MEMADDR)
    {
        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)_this->data;
        return head->h;
    }

    return 0;
}

void gui_glass_effect_refresh_size(gui_glass_effect_t *_this)
{
    _this->base.w = gui_glass_effect_get_width(_this);
    _this->base.h = gui_glass_effect_get_height(_this);
}

void gui_glass_effect_refresh_draw_data(gui_glass_effect_t  *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(this->data != NULL);
    this->draw_img->data = this->data;
}

void gui_glass_effect_set_attribute(gui_glass_effect_t  *_this,
                                    const char *name,
                                    void       *path,
                                    int16_t     x,
                                    int16_t     y)
{
    GUI_ASSERT(_this != NULL);

    if ((!name) && (!path))
    {
        return;
    }

    _this->base.x = x;
    _this->base.y = y;

    if (name != NULL)
    {
        _this->base.name = name;
    }
    else
    {
        _this->base.name = "gui_glass_effect_set_attribute";
    }

    _this->data = path;
}
void gui_glass_effect_set_image_data(gui_glass_effect_t  *_this, const uint8_t *file_pointer)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(file_pointer != NULL);
    _this->data = (void *)file_pointer;
}
const uint8_t *gui_glass_effect_get_image_data(gui_glass_effect_t  *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->data;
}

void gui_glass_effect_translate(gui_glass_effect_t *_this, float t_x, float t_y)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    _this->t_x = t_x;
    _this->t_y = t_y;
}

float gui_glass_effect_get_t_x(gui_glass_effect_t *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->t_x;
}
float gui_glass_effect_get_t_y(gui_glass_effect_t *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->t_y;
}
