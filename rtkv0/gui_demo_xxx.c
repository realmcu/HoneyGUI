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
#include "tiger_grey.txt"
#include "tiger_yellow.txt"
#include "tiger_white.txt"
#include "tiger_laven.txt"
#include "tiger_turk.txt"


const static UI_WidgetTypeDef  icon[] =
{
    // [0]     = {
    //     .x = 0,   .y = 0,    .width = 256, .hight = 236,     .addr = 0,   .widget_id_type = ICON_BACKGROUND,
    // },

};

const static UI_3DTypeDef  test_3d[] =
{
    [0]     = {
        .x = 100,   .y = 100,    .width = 256, .hight = 236,     .addr = 0,
    },
    [1]     = {
        .x = 100,   .y = 100,    .width = 256, .hight = 236,     .addr = 0,
    },
    [2]     = {
        .x = 100,   .y = 100,    .width = 256, .hight = 236,     .addr = 0,
    },
    [3]     = {
        .x = 100,   .y = 100,    .width = 256, .hight = 236,     .addr = 0,
    },
    [4]     = {
        .x = 200,   .y = 200,    .width = 256, .hight = 236,     .addr = 0,
    },
    [5]     = {
        .x = 200,   .y = 200,    .width = 256, .hight = 236,     .addr = 0,
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

    // matrix_rotate(45, &return_menu->matrix);
    // matrix_translate(100, 0, &return_menu->matrix);
    return_menu->pWidgetList[0].addr = (uint32_t)(_actiger_blue + 8);

    return_menu->p3DList[0].addr = (uint32_t)(_actiger_blue + 8);
    return_menu->p3DList[1].addr = (uint32_t)(_actiger_grey + 8);
    return_menu->p3DList[2].addr = (uint32_t)(_actiger_yellow + 8);
    return_menu->p3DList[3].addr = (uint32_t)(_actiger_white + 8);
    return_menu->p3DList[4].addr = (uint32_t)(_actiger_turk + 8);
    return_menu->p3DList[5].addr = (uint32_t)(_actiger_laven + 8);

    matrix_identity(&return_menu->p3DList[0].matrix);
    matrix_identity(&return_menu->p3DList[1].matrix);
    matrix_identity(&return_menu->p3DList[2].matrix);
    matrix_identity(&return_menu->p3DList[3].matrix);
    matrix_identity(&return_menu->p3DList[4].matrix);
    matrix_identity(&return_menu->p3DList[5].matrix);


    // gui_v0_cube(100, 240, 240, \
    //             40, 50, 60, \
    //             256, 236, \
    //             &return_menu->p3DList[0].matrix, \
    //             &return_menu->p3DList[1].matrix, \
    //             &return_menu->p3DList[2].matrix, \
    //             &return_menu->p3DList[3].matrix, \
    //             &return_menu->p3DList[4].matrix, \
    //             &return_menu->p3DList[5].matrix);

    // gui_v0_perspective(100, \
    //             256, 236, \
    //             &return_menu->p3DList[0].matrix, \
    //             &return_menu->p3DList[1].matrix, \
    //             &return_menu->p3DList[2].matrix, \
    //             &return_menu->p3DList[3].matrix, \
    //             &return_menu->p3DList[4].matrix, \
    //             &return_menu->p3DList[5].matrix);
}

static void menu_constructor_cb(UI_MenuTypeDef *cur_menu, void *argv)
{
    gui_log("file = %s, line = %d \n", __FILE__, __LINE__);


#if RTK_LEGCAY_GUI_USING_PSRAM
    UI_MenuTypeDef *ui_local_left = (UI_MenuTypeDef *)gui_get_menu_addr_by_name("GUIDemoMenuLeft");
    rtl_gui_prepare_frame_buffer(cur_menu, cur_menu, NULL, &FrameBufferOrigin);
    rtl_gui_prepare_frame_buffer(cur_menu, ui_local_left, NULL, &FrameBufferLeft);
    rtl_gui_prepare_frame_buffer(cur_menu, ui_local_left, NULL, &FrameBufferRight);
    rtl_gui_update_detal(-300, 0);
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
    .p3DList = (UI_3DTypeDef *)test_3d,
    .current_max_widget = sizeof(icon) / sizeof(UI_WidgetTypeDef),
    .current_max_3d = sizeof(test_3d) / sizeof(UI_3DTypeDef),
};



