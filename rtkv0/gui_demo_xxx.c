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
#include "tiger_blue.txt"

#if FEATURE_PSRAM
#define USING_PSRAM     1
#endif

const static UI_WidgetTypeDef  icon[] =
{
    [0]     = {
        .x = 0,   .y = 0,    .width = 256, .hight = 236,     .addr = 0,   .widget_id_type = ICON_BACKGROUND,
    },

};

const static UI_BMPTypeDef  bmp[] =
{
    [0]     = {
        .x = 0,    .y = 0,    .width = 66,    .hight = 66,    .addr = 0,
    },
};

static void menu_button_cb(UI_MenuTypeDef *cur_menu, uint8_t type)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_NEW_VIEW;
    p_msg.u.param = (uint32_t)gui_get_menu_addr_by_name("CPPTestView");
    post_event_to_gui_task(&p_msg, __LINE__);
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
#if 1
    rtl_gui_menu_update(cur_menu, NULL, cur_menu->MenuSub, NULL, cur_menu->MenuParent, NULL, 0, 0);
#else
    rtl_gui_prepare_frame_buffer(cur_menu, NULL, &FrameBufferOrigin, true);
    rtl_gui_update_detal(0, 0);
#endif
}

static void menu_display_info_cb(UI_MenuTypeDef *cur_menu, UI_MenuTypeDef *return_menu, void *argv)
{
    if ((return_menu == NULL) || (cur_menu == NULL))
    {
        return;
    }
    matrix_identity(&return_menu->matrix);
    matrix_rotate(45, &return_menu->matrix);
    return_menu->pWidgetList[0].addr = (uint32_t)(_actiger_blue + 8);
}

static void menu_constructor_cb(UI_MenuTypeDef *cur_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);


#if USING_PSRAM
    rtl_gui_prepare_frame_buffer(cur_menu, cur_menu, NULL, &FrameBufferOrigin);
    rtl_gui_prepare_frame_buffer(cur_menu, cur_menu, NULL, &FrameBufferLeft);
    rtl_gui_prepare_frame_buffer(cur_menu, cur_menu, NULL, &FrameBufferRight);
    rtl_gui_update_detal(-100, 0);
#else
    rtl_gui_menu_update(cur_menu, NULL, cur_menu, NULL, cur_menu, NULL, 0, 0);
#endif
    /* add user code here */
    /* add user code here */
}

static void menu_destructor_cb(UI_MenuTypeDef *cur_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);
}


/* change Here for UI */
GUI_MENU_SECTION const UI_MenuTypeDef GUIDemoMenu =
{
    /* change Here for UI */
    .name = (const char *)"GUIDemoMenu",
    .button_func = menu_button_cb,
    .touch_func = menu_touch_cb,
    .dynamic_cb = menu_dynamic_time_cb,
    .display_cb = menu_display_time_cb,
    .cur_display_info = menu_display_info_cb,
    .constructor_cb = menu_constructor_cb,
    .destructor_cb = menu_destructor_cb,
    .pWidgetList = (UI_WidgetTypeDef *)icon,
    .pBMPList = (UI_BMPTypeDef *)bmp,
    .current_max_widget = sizeof(icon) / sizeof(UI_WidgetTypeDef),
    .current_max_bmp = sizeof(bmp) / sizeof(UI_BMPTypeDef),
};



