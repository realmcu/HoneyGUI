/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtain.c
  * @brief create a curtain effect widget,which should be nested in a curtainview.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <string.h>
#include "gui_menu_cellular.h"
#include "gui_server.h"
#include "gui_obj.h"
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

#define CIRCLE_BOUNDARY (180.0f/454.0F*((float)gui_get_screen_width()))
#define SCREEN_W        ((float)gui_get_screen_width()-25.0f)
#define SCREEN_H        ((float)gui_get_screen_height()-25.0f)
#define SPEED_MAX       50
#define SPEED_MIN       5

/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

static int x, y ;
static int16_t speed[2];
static int16_t left, right, top, bottom;

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void gui_menu_cellular_image(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    touch_info_t *tp = tp_get_info();
    float tx, ty;

    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (obj->type == IMAGE_FROM_MEM)
        {
            if ((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_HOLD_Y))
            {

                obj->x += tp->deltaX - x;
                obj->y += tp->deltaY - y;
            }
            else
            {
                if (speed[0] != 0 || speed[1] != 0)
                {
                    obj->x += speed[0];
                    obj->y += speed[1];
                }
            }

            if (obj->x < left)
            {
                left = obj->x;
            }

            if (obj->x > right)
            {
                right = obj->x;
            }

            if (obj->y < top)
            {
                top = obj->y;
            }

            if (obj->y > bottom)
            {
                bottom = obj->y;
            }

            float x = sqrtf((SCREEN_W / 2.0f - (float)obj->x) * (SCREEN_W / 2.0f - (float)obj->x) +
                            (SCREEN_H / 2.0f - (float)obj->y) * (SCREEN_H / 2.0f - (float)obj->y));
            float scale = -1.0f / (1.8f * SCREEN_W) * x + 1.0f;

            if (_UI_ABS(x) > CIRCLE_BOUNDARY)
            {
                scale = scale - (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY);
            }

            if (scale <= 0)
            {
                scale = 0.01f;
            }

            gui_img_scale((void *)obj, scale, scale);
            gui_img_rotation((void *)obj, 0, gui_img_get_width((void *)obj) / 2,
                             gui_img_get_height((void *)obj) / 2);
            gui_img_translate((void *)obj, gui_img_get_width((void *)obj) / 2,
                              gui_img_get_height((void *)obj) / 2);

            if (_UI_ABS(x) > CIRCLE_BOUNDARY)
            {
                tx = (SCREEN_W / 2.0f - (float)obj->x) *
                     (1.0f - (scale + (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY)))
                     * (1.0f - (scale + (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY)))
                     + ((float)(gui_img_get_width((void *)obj) / 2))
                     * (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY)
                     * ((SCREEN_W / 2.0f
                         - (float)obj->x) / (SCREEN_W / 2.0f));
                ty = (SCREEN_H / 2.0f - (float)obj->y)  *
                     (1.0f - (scale + (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY)))
                     * (1.0f - (scale + (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_W / 2.0f - CIRCLE_BOUNDARY)))
                     + ((float)(gui_img_get_height((void *)obj) / 2))
                     * (_UI_ABS(x) - CIRCLE_BOUNDARY) / (SCREEN_H / 2.0f - CIRCLE_BOUNDARY)
                     * ((SCREEN_H / 2.0f
                         - (float)obj->y) / (SCREEN_H / 2.0f));
            }
            else
            {
                tx = (SCREEN_W / 2.0f - (float)obj->x) * (1.0f - (scale)) * (1.0f - (scale)) ;
                ty = (SCREEN_H / 2.0f - (float)obj->y) * (1.0f - (scale)) * (1.0f - (scale)) ;
            }

            gui_img_translate((void *)obj, tx, ty);
        }
        gui_menu_cellular_image(obj);
    }
}

static void gui_menu_cellular_wincb(gui_win_t *win)
{
    typedef int (*overwrite)(gui_obj_t *obj);
    touch_info_t *tp = tp_get_info();
    static bool move_flag = 1;
    static int16_t recode[2][5];
    int recode_num = sizeof(recode[0]) / sizeof(int16_t) - 1;
    int rst;
    left = INT16_MAX;
    top = INT16_MAX;
    right = 0;
    bottom = 0;

    if (GUI_TYPE(gui_menu_cellular_t, win)->overwrite)
    {
        rst = ((overwrite)(GUI_TYPE(gui_menu_cellular_t, win)->overwrite))((void *)win);

        if (rst != 0)
        {
            x = tp->deltaX;
            y = tp->deltaY;
            speed[0] = 0;
            speed[1] = 0;
        }
    }

    if (move_flag)
    {
        gui_menu_cellular_image((void *)win);
    }
    else
    {
        move_flag = 1;
    }

    x = tp->deltaX;
    y = tp->deltaY;

    if ((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_HOLD_Y))
    {
        for (size_t i = 0; i < recode_num; i++)
        {
            recode[0][i] = recode[0][i + 1];
            recode[1][i] = recode[1][i + 1];
        }

        recode[0][recode_num] = tp->deltaX;
        recode[1][recode_num] = tp->deltaY;
        speed[0] = recode[0] [recode_num] - recode[0][0];
        speed[1] = recode[1] [recode_num] - recode[1][0];

        for (size_t i = 0; i < 2; i++)
        {
            if (speed[i] > SPEED_MAX)
            {
                speed[i] = SPEED_MAX;
            }
            else if (speed[i] < -SPEED_MAX)
            {
                speed[i] = -SPEED_MAX;
            }

            if ((speed[i] > 0) && (speed[i] < SPEED_MIN))
            {
                speed[i] = SPEED_MIN;
            }
            else if ((speed[i] < 0) && (speed[i] > -SPEED_MIN))
            {
                speed[i] = -SPEED_MIN;
            }
        }
    }
    else
    {
        if ((left > SCREEN_W / 2)
            || (right < SCREEN_W / 2)
            || (top > SCREEN_H / 2)
            || (bottom < SCREEN_H / 2))
        {
            for (size_t i = 0; i < recode_num + 1; i++)
            {
                recode[0][i] = 0;
                recode[1][i] = 0;
            }

            speed[0] = 0;
            speed[1] = 0;
            move_flag = 0;
            return;
        }

        for (size_t i = 0; i < recode_num + 1; i++)
        {
            recode[0][i] = 0;
            recode[1][i] = 0;
        }

        if (speed[0] > 0)
        {
            speed[0]--;
        }
        else if (speed[0] < 0)
        {
            speed[0]++;
        }

        if (speed[1] > 0)
        {
            speed[1]--;
        }
        else if (speed[1] < 0)
        {
            speed[1]++;
        }
    }
}

static void gui_menu_cellular_ctor(gui_menu_cellular_t *this, gui_obj_t *parent)
{
    extern void gui_win_ctor(gui_win_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                             int16_t y, int16_t w, int16_t h);
    gui_win_ctor((void *)this, parent, 0, 0, 0, gui_get_screen_width(), gui_get_screen_height());
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_menu_cellular_t *gui_menu_cellular_create(void     *parent,
                                              int       icon_size,
                                              uint32_t *icon_array[],
                                              int       array_size)
{
#define _GUI_NEW_GUI_MENU_CELLULAR_PARAM this, parent
    GUI_CREATE_HELPER(gui_menu_cellular_t, gui_menu_cellular_ctor, _GUI_NEW_GUI_MENU_CELLULAR_PARAM)
    gui_win_t *win = &(this->base);
    gui_win_set_animate(win, 1000, -1, gui_menu_cellular_wincb, win);
#define ICON_SIZE (icon_size)
#define WIDTH_GAP (ICON_SIZE)
#define HEIGHT_GAP (ICON_SIZE-ICON_SIZE/7)
#define INIT_OFFSET_X (0)
#define INIT_OFFSET_Y (0)

    for (size_t i = 0; i < array_size; i++)
    {
        if (i < 3)
        {
            gui_img_create_from_mem(win, 0, icon_array[i], INIT_OFFSET_X + WIDTH_GAP * 2, INIT_OFFSET_Y, 0, 0);
        }
        else if (i < 9)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 3) + (WIDTH_GAP / 2),
                                    INIT_OFFSET_Y + HEIGHT_GAP, 0, 0);
        }
        else if (i < 16)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 9) + (WIDTH_GAP / 2) * 0,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 2, 0, 0);
        }
        else if (i < 24)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 17) + (WIDTH_GAP / 2) * 1,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 3, 0, 0);
        }
        else if (i < 31)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 24) + (WIDTH_GAP / 2) * 0,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 4, 0, 0);
        }
        else if (i < 37)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 31) + (WIDTH_GAP / 2) * 1,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 5, 0, 0);
        }
        else if (i < 42)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 36) + (WIDTH_GAP / 2) * 0,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 6, 0, 0);
        }
        else if (i < 46)
        {
            gui_img_create_from_mem(win, 0, icon_array[i],
                                    INIT_OFFSET_X + WIDTH_GAP * (i - 41) + (WIDTH_GAP / 2) * 1,
                                    INIT_OFFSET_Y + HEIGHT_GAP * 7, 0, 0);
        }
    }
    return (void *)win;
}

void gui_menu_cellular_offset(gui_obj_t *menu_cellular, int offset_x, int offset_y)
{
    gui_list_t *node = NULL;

    gui_list_for_each(node, &menu_cellular->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);

        if (obj->type == IMAGE_FROM_MEM)
        {
            obj->x += offset_x;
            obj->y += offset_y;
        }

        gui_menu_cellular_offset(obj, offset_x, offset_y);
    }
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
