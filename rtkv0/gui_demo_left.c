/*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author
* @date
* @version
*********************************************************************************************************
*/
#include "stdio.h"
#include "wristband_gui.h"
#include "gui_common_menu.h"
#include "gui_task.h"
#include "gui_psram.h"
#include <gui_matrix.h>
#include "tiger_yellow.txt"


const static UI_WidgetTypeDef  icon[] =
{
    [0]     = {
        .x = 100,   .y = 100,    .width = 256, .hight = 236,     .addr = 0,   .widget_id_type = ICON_BACKGROUND,
    },

};

static void menu_button_cb(UI_MenuTypeDef *cur_menu, uint8_t type)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}

static void menu_touch_cb(UI_MenuTypeDef *cur_menu, RtkTouchInfo_t *info)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}

static void menu_display_time_cb(UI_MenuTypeDef *cur_menu, UI_MenuTypeDef *return_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}

static void menu_dynamic_time_cb(UI_MenuTypeDef *cur_menu, UI_MenuTypeDef *return_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}

static void menu_display_info_cb(UI_MenuTypeDef *cur_menu, UI_MenuTypeDef *return_menu, void *argv)
{
    if ((return_menu == NULL) || (cur_menu == NULL))
    {
        return;
    }
    matrix_identity(&return_menu->matrix);
    return_menu->pWidgetList[0].addr = (uint32_t)(_actiger_yellow + 8);
}

static void menu_constructor_cb(UI_MenuTypeDef *cur_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}

static void menu_destructor_cb(UI_MenuTypeDef *cur_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}


/* change Here for UI */
GUI_MENU_SECTION const UI_MenuTypeDef GUIDemoMenuLeft =
{
    /* change Here for UI */
    .name = (const char *)"GUIDemoMenuLeft",
    .button_func = menu_button_cb,
    .touch_func = menu_touch_cb,
    .dynamic_cb = menu_dynamic_time_cb,
    .display_cb = menu_display_time_cb,
    .cur_display_info = menu_display_info_cb,
    .constructor_cb = menu_constructor_cb,
    .destructor_cb = menu_destructor_cb,
    .pWidgetList = (UI_WidgetTypeDef *)icon,
    .current_max_widget = sizeof(icon) / sizeof(UI_WidgetTypeDef),
};



