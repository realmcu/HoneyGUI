/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_slider.c
  * @brief  slider design
  * @details slider widget
  * @author sienna_shen@realsil.com.cn
  * @date 2024/8/20
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gui_slider.h>
#include "def_list.h"
#include "tp_algo.h"
#include "gui_img.h"



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
#define TEXT_LENGTH 50

/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */
char show_text[TEXT_LENGTH];

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_slider_prepare(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_slider_t *this = (gui_slider_t *)obj;

    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()) == false)
    {
        return;
    }

    if ((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_HOLD_Y) || tp->pressed || tp->pressing)
    {
        if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
        {
            this->dragging = true;
        }
    }
    else
    {
        this->dragging = false;
    }

    if (this->dragging)
    {
        int newSliderX = tp->x + tp->deltaX - this->x - this->deltax - this->slider_size / 2;
        int newSliderY = tp->y + tp->deltaY - this->y - this->deltay - this->slider_size / 2;

        newSliderX = newSliderX > (this->w - this->slider_size) ? (this->w - this->slider_size) :
                     newSliderX;
        newSliderX = newSliderX < 0 ? 0 : newSliderX;

        newSliderY = newSliderY > (this->h - this->slider_size) ? (this->h - this->slider_size) :
                     newSliderY;
        newSliderY = newSliderY < 0 ? 0 : newSliderY;

        this->sliderX = newSliderX;
        this->sliderY = newSliderY;
        this->currentValue = this->minValue + this->sliderX * (this->maxValue - this->minValue) /
                             (this->w - this->slider_size);

        gui_img_set_location(this->slider_image, this->sliderX, this->sliderY);

        snprintf(show_text, TEXT_LENGTH, "current value: %d", this->currentValue);
        gui_text_set(this->currentValue_text, show_text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                                                                                   UINT8_MAX,
                                                                                   UINT8_MAX), strlen(show_text), this->text_size);

        if (this->preValue != this->currentValue)
        {
            gui_obj_event_set(GUI_BASE(this), GUI_EVENT_1);
        }
        this->preValue = this->currentValue;
    }
}



static void gui_slider_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_slider_prepare(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_slider_ctor(gui_slider_t  *this,
                            gui_obj_t     *parent,
                            void          *bg_img,
                            int16_t        x,
                            int16_t        y,
                            int16_t        w,
                            int16_t        h,
                            uint16_t       minValue,
                            uint16_t       maxValue,
                            void          *slider_img,
                            int16_t        deltax,
                            int16_t        deltay,
                            int16_t        slider_size,
                            int16_t        text_size)
{
    //for base class
    gui_obj_ctor(&this->base, parent, "slider", x, y, w, h);

    //for root class
    GET_BASE(this)->type = SLIDER;
    GET_BASE(this)->obj_cb = gui_slider_cb;
    GET_BASE(this)->has_prepare_cb = true;

    this->background_image_addr = bg_img;
    this->slider_image_addr = slider_img;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->minValue = minValue;
    this->maxValue = maxValue;
    this->sliderX = deltax;
    this->sliderY = deltay;
    this->slider_size = slider_size;

    this->currentValue = minValue;
    this->preValue = this->currentValue;
    this->text_size = text_size;

}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/


gui_slider_t *gui_slider_create(void          *parent,
                                void          *bg_img,
                                int16_t        x,
                                int16_t        y,
                                int16_t        w,
                                int16_t        h,
                                uint16_t       minValue,
                                uint16_t       maxValue,
                                void          *slider_img,
                                int16_t        deltax,
                                int16_t        deltay,
                                int16_t        slider_size,
                                int16_t        text_size)
{
    GUI_ASSERT(parent != NULL);
    gui_slider_t *slider = gui_malloc(sizeof(gui_slider_t));
    GUI_ASSERT(slider != NULL);
    memset(slider, 0x00, sizeof(gui_slider_t));

    gui_slider_ctor(slider, (gui_obj_t *)parent, bg_img, x, y, w, h, minValue, maxValue, slider_img,
                    deltax, deltay, slider_size, text_size);

    gui_list_init(&(GET_BASE(slider)->child_list));
    if ((GET_BASE(slider)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(slider)->parent)->child_list),
                               &(GET_BASE(slider)->brother_list));
    }

    slider->background_image = gui_img_create_from_mem(slider, "slider_bg",
                                                       slider->background_image_addr, 0, 0, slider->w, slider->h);
    gui_img_set_mode(slider->background_image, IMG_BYPASS_MODE);

    slider->slider_image = gui_img_create_from_mem(slider, "slider_img", slider->slider_image_addr,
                                                   slider->deltax, slider->deltay, slider->slider_size, slider->slider_size);
    gui_img_set_mode(slider->slider_image, IMG_SRC_OVER_MODE);

    slider->currentValue_text = gui_text_create(slider, "currentValue", 0, slider->h, slider->w, 28);
    snprintf(show_text, TEXT_LENGTH, "current value: %d", slider->currentValue);
    gui_text_set(slider->currentValue_text, show_text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                                                                                 UINT8_MAX,
                                                                                 UINT8_MAX), strlen(show_text), slider->text_size);
    GET_BASE(slider)->create_done = true;
    return slider;
}

void gui_slider_value_change(gui_slider_t *this, gui_event_cb_t event_cb, void *parameter)
{
    gui_obj_add_event_cb(this, event_cb, GUI_EVENT_1, parameter);
}

uint16_t gui_slider_get_currentValue(gui_slider_t *this)
{
    return this->currentValue;
}

_GUI_API_ASSIGN(gui_slider_t)
.on_change = gui_slider_value_change,
 .get_currentValue = gui_slider_get_currentValue,
};


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





