#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"

uintptr_t watchface_digit_clock_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, (void *)DCLOCK_FG_0_BIN);
        gui_widget_litegfx_set_image(obj, 1, (void *)DCLOCK_FG_1_BIN);
        gui_widget_litegfx_set_image(obj, 2, (void *)DCLOCK_FG_2_BIN);
        gui_widget_litegfx_set_image(obj, 3, (void *)DCLOCK_FG_3_BIN);
        gui_widget_litegfx_set_image(obj, 4, (void *)DCLOCK_FG_4_BIN);
        gui_widget_litegfx_set_image(obj, 5, (void *)DCLOCK_FG_5_BIN);
        gui_widget_litegfx_set_image(obj, 6, (void *)DCLOCK_FG_6_BIN);
        gui_widget_litegfx_set_image(obj, 7, (void *)DCLOCK_FG_7_BIN);
        gui_widget_litegfx_set_image(obj, 8, (void *)DCLOCK_FG_8_BIN);
        gui_widget_litegfx_set_image(obj, 9, (void *)DCLOCK_FG_9_BIN);

        gui_widget_litegfx_set_image(obj, 10, (void *)DCLOCK_BG_0_BIN);
        gui_widget_litegfx_set_image(obj, 11, (void *)DCLOCK_BG_1_BIN);
        gui_widget_litegfx_set_image(obj, 12, (void *)DCLOCK_BG_2_BIN);
        gui_widget_litegfx_set_image(obj, 13, (void *)DCLOCK_BG_3_BIN);
        gui_widget_litegfx_set_image(obj, 14, (void *)DCLOCK_BG_4_BIN);
        gui_widget_litegfx_set_image(obj, 15, (void *)DCLOCK_BG_5_BIN);
        gui_widget_litegfx_set_image(obj, 16, (void *)DCLOCK_BG_6_BIN);
        gui_widget_litegfx_set_image(obj, 17, (void *)DCLOCK_BG_7_BIN);
        gui_widget_litegfx_set_image(obj, 18, (void *)DCLOCK_BG_8_BIN);
        gui_widget_litegfx_set_image(obj, 19, (void *)DCLOCK_BG_9_BIN);

        gui_widget_litegfx_set_image(obj, 20, (void *)DCLOCK_FG_WEEK0_BIN);
        gui_widget_litegfx_set_image(obj, 21, (void *)DCLOCK_FG_WEEK1_BIN);
        gui_widget_litegfx_set_image(obj, 22, (void *)DCLOCK_FG_WEEK2_BIN);
        gui_widget_litegfx_set_image(obj, 23, (void *)DCLOCK_FG_WEEK3_BIN);
        gui_widget_litegfx_set_image(obj, 24, (void *)DCLOCK_FG_WEEK4_BIN);
        gui_widget_litegfx_set_image(obj, 25, (void *)DCLOCK_FG_WEEK5_BIN);
        gui_widget_litegfx_set_image(obj, 26, (void *)DCLOCK_FG_WEEK6_BIN);

        gui_widget_litegfx_set_image(obj, 27, (void *)DCLOCK_BG_WEEK0_BIN);
        gui_widget_litegfx_set_image(obj, 28, (void *)DCLOCK_BG_WEEK1_BIN);
        gui_widget_litegfx_set_image(obj, 29, (void *)DCLOCK_BG_WEEK2_BIN);
        gui_widget_litegfx_set_image(obj, 30, (void *)DCLOCK_BG_WEEK3_BIN);
        gui_widget_litegfx_set_image(obj, 31, (void *)DCLOCK_BG_WEEK4_BIN);
        gui_widget_litegfx_set_image(obj, 32, (void *)DCLOCK_BG_WEEK5_BIN);
        gui_widget_litegfx_set_image(obj, 33, (void *)DCLOCK_BG_WEEK6_BIN);

        gui_widget_litegfx_set_image(obj, 34, (void *)DCLOCK_BTY_BIN);
        gui_widget_litegfx_set_image(obj, 35, (void *)DCLOCK_TIME_BIN);
        gui_widget_litegfx_set_image(obj, 36, (void *)DCLOCK_DATE_BIN);
        gui_widget_litegfx_set_image(obj, 37, (void *)DCLOCK_BACKGROUND_BIN);
        gui_widget_litegfx_set_image(obj, 38, (void *)DCLOCK_BACKGROUND_SHADOW_BIN);

        gui_widget_litegfx_set_param(obj, LX_CMD_ID_BATTERY, 80);//电池电量
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_TIME_FORMAT,
                                     1);//时间制式开关，0：24小时制，1：12小时制，默认0
    }
    return 0;
}

void design_tab_watchface_digit_clock(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite digit clock",
                                                             LX_VGWIDGET_INS_DIGIT_CLOCK_00, watchface_digit_clock_user_cb, 0, 0, 454, 454);
}