/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_glass.c
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
typedef struct
{
    uint16_t spread_out;
    uint16_t switch_line_x;
    uint16_t switch_line_y;
} glass_spread_desc;

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
static void gui_glass_prepare(gui_obj_t *obj)
{
    uint8_t last;
    gui_glass_t *_this;
    touch_info_t *tp;
    GUI_UNUSED(tp);

    GUI_ASSERT(obj != NULL);
    struct gui_dispdev *dc = gui_get_dc();
    _this = (gui_glass_t *)obj;
    tp = tp_get_info();
    matrix_identity(obj->matrix);
    float offset_X = _this->t_x + _this->base.x;
    float offset_Y = _this->t_y + _this->base.y;
    if (offset_X < 0)
    {
        offset_X = 0;
    }
    else if (offset_X + _this->base.w > dc->fb_width)
    {
        offset_X = dc->fb_width - _this->base.w;
    }
    if (offset_Y < 0)
    {
        offset_Y = 0;
    }
    else if (offset_Y + _this->base.h > dc->fb_height)
    {
        offset_Y = dc->fb_height - _this->base.h;
    }
    matrix_translate(offset_X, offset_Y, obj->matrix);
    obj->x = offset_X;
    obj->y = offset_Y;
    _this->t_x = 0;
    _this->t_y = 0;
    if (gui_obj_out_screen(obj))
    {
        return;
    }

    _this->draw_img = gui_malloc(sizeof(draw_img_t));
    memset(_this->draw_img, 0x00, sizeof(draw_img_t));

    _this->draw_img->data = _this->data;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);

    memcpy(&_this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&_this->draw_img->inverse, obj->matrix, sizeof(struct gui_matrix));

    matrix_inverse(&_this->draw_img->inverse);
    draw_img_load_scale(_this->draw_img, (IMG_SOURCE_MODE_TYPE)_this->storage_type);


    draw_img_new_area(_this->draw_img, NULL);


    last = _this->checksum;
    _this->checksum = 0;
    _this->checksum = gui_obj_checksum(0, (uint8_t *)_this, sizeof(gui_glass_t));

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
    if (dc->section.y2 < y_min || dc->section.y1 > y_max || dc->section.x2 < x_min ||
        dc->section.x1 > x_max)
    {
        return;
    }
    int area_x_max = x_max > dc->section.x2 ? dc->section.x2 : x_max;
    int area_x_min = x_min < dc->section.x1 ? dc->section.x1 : x_min;
    int area_y_max = y_max > dc->section.y2 ? dc->section.y2 : y_max;
    int area_y_min = y_min < dc->section.y1 ? dc->section.y1 : y_min;
    uint32_t fb_width = dc->fb_width;
    uint32_t fb_height = dc->fb_height;
    glass_spread_desc *spread = (glass_spread_desc *)((int8_t *)image->data + 8);
    int mid_x = spread->switch_line_x;
    int mid_y = spread->switch_line_y;
    uint32_t area_x_offset_max = (area_x_max - x_min) * 2;
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
    uint32_t glass_width = head->w * 2;
    uint32_t area_x_offset = (area_x_min - x_min) * 2;
    int8_t *glass_offset = (int8_t *)image->data + 12;
    uint32_t *bg_u32 = (uint32_t *)dc->frame_buf;
    int decrease_start_y, decrease_end_y, increase_start_y, increase_end_y;
    int decrease_start_x, decrease_end_x, increase_start_x, increase_end_x;
    if (spread->spread_out)
    {
        decrease_start_y = mid_y + y_min;
        decrease_end_y = area_y_min - 1;
        increase_start_y = mid_y + y_min + 1;
        increase_end_y = area_y_max;
        if (mid_y == 0)
        {
            decrease_start_y = area_y_max;
            increase_start_y = area_y_max + 1;
        }
        decrease_start_x = mid_x + x_min;
        decrease_end_x = area_x_min - 1;
        increase_start_x = mid_x + x_min + 1;
        increase_end_x = area_x_max;
        if (mid_x == 0)
        {
            decrease_start_x = area_x_max;
            increase_start_x = area_x_max + 1;
        }
    }
    else
    {
        decrease_start_y = area_y_max;
        decrease_end_y = mid_y + y_min;
        increase_start_y = area_y_min;
        increase_end_y = mid_y + y_min;
        if (mid_y == 0)
        {
            decrease_start_y = area_y_min - 1;
            increase_end_y = area_y_max;
        }
        decrease_start_x = area_x_max;
        decrease_end_x = mid_x + x_min;
        increase_start_x = area_x_min;
        increase_end_x = mid_x + x_min;
        if (mid_x == 0)
        {
            decrease_start_x = area_x_min - 1;
            increase_end_x = area_x_max;
        }
    }

    for (int y = increase_start_y; y <= increase_end_y; y++)
    {
        uint32_t glass_y = y - y_min;
        uint32_t offset = glass_y * glass_width + area_x_offset;
        uint32_t bg_offset = (y * fb_width + area_x_min);
        for (int x = increase_start_x; x <= increase_end_x; x++)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u32[y1 * fb_width + x1] : 0;
            bg_u32[bg_offset++] = pixel;
            offset += 2;
        }
        offset = glass_y * glass_width + area_x_offset_max;
        bg_offset = (y * fb_width + area_x_max);
        for (int x = decrease_start_x; x > decrease_end_x; x--)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u32[y1 * fb_width + x1] : 0;
            bg_u32[bg_offset--] = pixel;
            offset -= 2;
        }
    }
    for (int y = decrease_start_y; y > decrease_end_y; y--)
    {
        uint32_t glass_y = y - y_min;
        uint32_t offset = glass_y * glass_width + area_x_offset;
        uint32_t bg_offset = (y * fb_width + area_x_min);
        for (int x = increase_start_x; x <= increase_end_x; x++)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u32[y1 * fb_width + x1] : 0;
            bg_u32[bg_offset++] = pixel;
            offset += 2;
        }
        offset = glass_y * glass_width + area_x_offset_max;
        bg_offset = (y * fb_width + area_x_max);
        for (int x = decrease_start_x; x > decrease_end_x; x--)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u32[y1 * fb_width + x1] : 0;
            bg_u32[bg_offset--] = pixel;
            offset -= 2;
        }
    }
}

static void gui_put_glass_on_bg_16(draw_img_t *image, struct gui_dispdev *dc)
{
    int32_t x_max = (image->img_target_w + image->img_target_x - 1);
    int32_t y_max = (image->img_target_h + image->img_target_y - 1);
    int32_t x_min = image->img_target_x;
    int32_t y_min = image->img_target_y;
    if (dc->section.y2 < y_min || dc->section.y1 > y_max || dc->section.x2 < x_min ||
        dc->section.x1 > x_max)
    {
        return;
    }
    int area_x_max = x_max > dc->section.x2 ? dc->section.x2 : x_max;
    int area_x_min = x_min < dc->section.x1 ? dc->section.x1 : x_min;
    int area_y_max = y_max > dc->section.y2 ? dc->section.y2 : y_max;
    int area_y_min = y_min < dc->section.y1 ? dc->section.y1 : y_min;
    uint32_t fb_width = dc->fb_width;
    uint32_t fb_height = dc->fb_height;
    glass_spread_desc *spread = (glass_spread_desc *)((int8_t *)image->data + 8);
    int mid_y = spread->switch_line_y;
    int mid_x = spread->switch_line_x;
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)image->data;
    uint32_t glass_width = head->w * 2;
    uint32_t area_x_offset = (area_x_min - x_min) * 2;
    int8_t *glass_offset = (int8_t *)image->data + 12;
    uint16_t *bg_u16 = (uint16_t *)dc->frame_buf;
    uint32_t area_x_offset_max = (area_x_max - x_min) * 2;
    int decrease_start_y, decrease_end_y, increase_start_y, increase_end_y;
    int decrease_start_x, decrease_end_x, increase_start_x, increase_end_x;
    if (spread->spread_out)
    {
        decrease_start_y = mid_y + y_min;
        decrease_end_y = area_y_min - 1;
        increase_start_y = mid_y + y_min + 1;
        increase_end_y = area_y_max;
        if (mid_y == 0)
        {
            decrease_start_y = area_y_max;
            increase_start_y = area_y_max + 1;
        }
        decrease_start_x = mid_x + x_min;
        decrease_end_x = area_x_min - 1;
        increase_start_x = mid_x + x_min + 1;
        increase_end_x = area_x_max;
        if (mid_x == 0)
        {
            decrease_start_x = area_x_max;
            increase_start_x = area_x_max + 1;
        }
    }
    else
    {
        decrease_start_y = area_y_max;
        decrease_end_y = mid_y + y_min;
        increase_start_y = area_y_min;
        increase_end_y = mid_y + y_min;
        if (mid_y == 0)
        {
            decrease_start_y = area_y_min - 1;
            increase_end_y = area_y_max;
        }
        decrease_start_x = area_x_max;
        decrease_end_x = mid_x + x_min;
        increase_start_x = area_x_min;
        increase_end_x = mid_x + x_min;
        if (mid_x == 0)
        {
            decrease_start_x = area_x_min - 1;
            increase_end_x = area_x_max;
        }
    }

    for (int y = increase_start_y; y <= increase_end_y; y++)
    {
        uint32_t glass_y = y - y_min;
        uint32_t offset = glass_y * glass_width + area_x_offset;
        uint32_t bg_offset = (y * fb_width + area_x_min);
        for (int x = increase_start_x; x <= increase_end_x; x++)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u16[y1 * fb_width + x1] : 0;
            bg_u16[bg_offset++] = pixel;
            offset += 2;
        }
        offset = glass_y * glass_width + area_x_offset_max;
        bg_offset = (y * fb_width + area_x_max);
        for (int x = decrease_start_x; x > decrease_end_x; x--)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u16[y1 * fb_width + x1] : 0;
            bg_u16[bg_offset--] = pixel;
            offset -= 2;
        }
    }
    for (int y = decrease_start_y; y > decrease_end_y; y--)
    {
        uint32_t glass_y = y - y_min;
        uint32_t offset = glass_y * glass_width + area_x_offset;
        uint32_t bg_offset = (y * fb_width + area_x_min);
        for (int x = increase_start_x; x <= increase_end_x; x++)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u16[y1 * fb_width + x1] : 0;
            bg_u16[bg_offset++] = pixel;
            offset += 2;
        }
        offset = glass_y * glass_width + area_x_offset_max;
        bg_offset = (y * fb_width + area_x_max);
        for (int x = decrease_start_x; x > decrease_end_x; x--)
        {
            int x1 = x + *(glass_offset + offset);
            int y1 = y + *(glass_offset + offset + 1);
            uint16_t pixel = ((uint32_t)x1 < fb_width && (uint32_t)y1 < fb_height) ?
                             bg_u16[y1 * fb_width + x1] : 0;
            bg_u16[bg_offset--] = pixel;
            offset -= 2;
        }
    }
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
 *                `gui_glass_prepare`.
 */
static void gui_glass_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_glass_t *_this = (gui_glass_t *)obj;
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
static void gui_glass_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    gui_glass_t *_this = (gui_glass_t *)obj;

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

static int16_t previous_deltaX, previous_deltaY;
static void gui_glass_pressing_cb(void *obj, gui_event_t e, void *param)
{
    gui_glass_t *glass = (gui_glass_t *)obj;
    (void)e;
    (void)param;
    // gui_log("g_Offset %d\n", g_Offset);
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    (void)dc;
    (void)tp;
    switch (e)
    {
    case GUI_EVENT_TOUCH_PRESSED:
        previous_deltaX = tp->deltaX;
        previous_deltaY = tp->deltaY;
        break;
    case GUI_EVENT_TOUCH_RELEASED:
        glass->t_x = 0;
        glass->t_y = 0;
        break;
    case GUI_EVENT_TOUCH_PRESSING:
        glass->t_x = tp->deltaX - previous_deltaX;
        glass->t_y = tp->deltaY - previous_deltaY;
        previous_deltaX = tp->deltaX;
        previous_deltaY = tp->deltaY;
        break;
    default:
        break;
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
static void gui_glass_destroy(gui_obj_t *obj)
{
    gui_glass_t *_this = (gui_glass_t *)obj;
    obj->x = _this->history_x;
    obj->y = _this->history_y;
    GUI_UNUSED(_this);
}

static void gui_glass_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_glass_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_glass_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_glass_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                gui_glass_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

void gui_glass_enable_pressing_envent(gui_glass_t *_this)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    gui_obj_add_event_cb(obj, gui_glass_pressing_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_add_event_cb(obj, gui_glass_pressing_cb, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_add_event_cb(obj, gui_glass_pressing_cb, GUI_EVENT_TOUCH_PRESSING, NULL);
}

void gui_glass_enable_click_event(gui_glass_t *_this)
{
    gui_obj_t *obj = (gui_obj_t *)_this;
    gui_obj_add_event_cb(obj, gui_glass_pressing_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void gui_glass_ctor(gui_glass_t            *_this,
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

    obj->obj_cb = gui_glass_cb;
    obj->has_input_prepare_cb = false;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
    obj->type = IMAGE_FROM_MEM;
    _this->history_x = obj->x;
    _this->history_y = obj->y;
    if (storage_type == IMG_SRC_FILESYS)
    {
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

    obj->w = gui_glass_get_width(_this);
    obj->h = gui_glass_get_height(_this);
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_glass_t *gui_glass_create_from_mem(void       *parent,
                                       const char *name,
                                       void       *addr,
                                       int16_t     x,
                                       int16_t     y,
                                       int16_t     w,
                                       int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_t *_this = gui_malloc(sizeof(gui_glass_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_t));
    gui_glass_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_MEMADDR, addr, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}

gui_glass_t *gui_glass_create_from_ftl(void       *parent,
                                       const char *name,
                                       void       *ftl,
                                       int16_t     x,
                                       int16_t     y,
                                       int16_t     w,
                                       int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_t *_this = gui_malloc(sizeof(gui_glass_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_t));
    gui_glass_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FTL, ftl, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}


gui_glass_t *gui_glass_create_from_fs(void       *parent,
                                      const char *name,
                                      void       *file,
                                      int16_t     x,
                                      int16_t     y,
                                      int16_t     w,
                                      int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    gui_glass_t *_this = gui_malloc(sizeof(gui_glass_t));
    GUI_ASSERT(_this != NULL);
    memset(_this, 0x00, sizeof(gui_glass_t));
    gui_glass_ctor(_this, (gui_obj_t *)parent, name, IMG_SRC_FILESYS, file, x, y, w, h);

    gui_list_init(&(GET_BASE(_this)->child_list));
    if ((GET_BASE(_this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(_this)->parent)->child_list), &(GET_BASE(_this)->brother_list));
    }
    GET_BASE(_this)->create_done = true;

    return _this;
}

uint16_t gui_glass_get_width(gui_glass_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.w = 0;
        int fd = gui_open(_this->data,  0);

        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)_this->data);
        }

        gui_read(fd, &head, sizeof(head));
        gui_close(fd);
        return head.w;
    }
    else if (_this->storage_type == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uintptr_t)_this->data;
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

uint16_t gui_glass_get_height(gui_glass_t *_this)
{
    if (_this->storage_type == IMG_SRC_FILESYS)
    {
        struct gui_rgb_data_head head;
        head.h = 0;
        int fd = gui_open(_this->data,  0);

        if (fd <= 0)
        {
            gui_log("open file fail:%s !\n", (char *)_this->data);
        }

        gui_read(fd, &head, sizeof(head));
        gui_close(fd);
        return head.h;
    }
    else if (_this->storage_type == IMG_SRC_FTL)
    {
        struct gui_rgb_data_head head;
        uint32_t base = (uintptr_t)_this->data;
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

void gui_glass_refresh_size(gui_glass_t *_this)
{
    _this->base.w = gui_glass_get_width(_this);
    _this->base.h = gui_glass_get_height(_this);
}

void gui_glass_refresh_draw_data(gui_glass_t  *this)
{
    GUI_ASSERT(GUI_BASE(this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(this->data != NULL);
    this->draw_img->data = this->data;
}

void gui_glass_set_attribute(gui_glass_t  *_this,
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
        _this->base.name = "gui_glass_set_attribute";
    }

    _this->data = path;
}
void gui_glass_set_image_data(gui_glass_t  *_this, const uint8_t *file_pointer)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    GUI_ASSERT(file_pointer != NULL);
    _this->data = (void *)file_pointer;
}
const uint8_t *gui_glass_get_image_data(gui_glass_t  *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->data;
}

void gui_glass_translate(gui_glass_t *_this, float t_x, float t_y)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    _this->t_x = t_x;
    _this->t_y = t_y;
}

float gui_glass_get_t_x(gui_glass_t *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->t_x;
}
float gui_glass_get_t_y(gui_glass_t *_this)
{
    GUI_ASSERT(GUI_BASE(_this)->type == IMAGE_FROM_MEM);
    return _this->t_y;
}