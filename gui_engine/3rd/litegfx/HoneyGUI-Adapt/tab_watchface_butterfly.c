#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"

gui_text_t *watchface_time_tips = NULL;

static uintptr_t watchface_butterfly_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, (void *)BUTTERFLY_BODYHALF_BIN);
        gui_widget_litegfx_set_image(obj, 1, (void *)BUTTERFLY_ANTENNA_BIN);
        gui_widget_litegfx_set_image(obj, 2, (void *)BUTTERFLY_SWINGUP_BIN);
        gui_widget_litegfx_set_image(obj, 3, (void *)BUTTERFLY_SWINGDOWN_BIN);
        gui_widget_litegfx_set_image(obj, 4, (void *)BUTTERFLY_DOT0_BIN);
        gui_widget_litegfx_set_image(obj, 5, (void *)BUTTERFLY_DOT1_BIN);
        gui_widget_litegfx_set_image(obj, 6, (void *)COMMON_FONT32_BIN);
        gui_widget_litegfx_set_image(obj, 7, (void *)COMMON_FONT96_BIN);
        gui_widget_litegfx_set_image(obj, 8, (void *)COMMON_UNFIXED_BIN);
        gui_widget_litegfx_set_image(obj, 9, (void *)COMMON_FIXED_BIN);

        gui_widget_litegfx_set_param(obj, LX_CMD_ID_COLOR, 0x82bbfc);//设置蝴蝶配套的粒子颜色
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SHOW_TIME,
                                     (uint32_t)false);//时间显示开关，默认开
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_TIME_FORMAT,
                                     1);//时间制式开关，0：24小时制，1：12小时制，默认0
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_FIXED_BUTTON,
                                     (uint32_t)false);//表盘固定按钮开关，默认关
    }
    return 0;
}

void design_tab_watchface_butterfly(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite butterfly",
                                                             LX_VGWIDGET_INS_BUTTERFLY, watchface_butterfly_user_cb, 0, 0, 454, 454);

    char *string_time_tips = "12:34";
    watchface_time_tips = gui_text_create(parent, "watchface_time_tips", 200, 40,
                                          strlen(string_time_tips) * FONT_NUM_ALPHA_W, FONT_H_32);
    gui_text_set(watchface_time_tips, string_time_tips, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_time_tips), FONT_H_32);
}