#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#if 0

static uintptr_t watchface_flowerfall_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, FLOWERFALL_DOT01_BIN);
        gui_widget_litegfx_set_image(obj, 1, FLOWERFALL_DOT02_BIN);
        gui_widget_litegfx_set_image(obj, 2, FLOWERFALL_LEAF01_BIN);
        gui_widget_litegfx_set_image(obj, 3, FLOWERFALL_LEAF02_BIN);
        gui_widget_litegfx_set_image(obj, 4, FLOWERFALL_LEAF03_BIN);
        gui_widget_litegfx_set_image(obj, 5, FLOWERFALL_LEAF04_BIN);
        gui_widget_litegfx_set_image(obj, 6, FLOWERFALL_LEAF05_BIN);
        gui_widget_litegfx_set_image(obj, 7, FLOWERFALL_LEAF06_BIN);
        gui_widget_litegfx_set_image(obj, 8, FLOWERFALL_BACKGROUND_BIN);
        gui_widget_litegfx_set_image(obj, 9, FLOWERFALL_BRANCH01_BIN);
        gui_widget_litegfx_set_image(obj, 10, FLOWERFALL_BRANCH02_BIN);
        gui_widget_litegfx_set_image(obj, 11, (void *)COMMON_FONT32_BIN);
        gui_widget_litegfx_set_image(obj, 12, (void *)COMMON_FONT96_BIN);
        gui_widget_litegfx_set_image(obj, 13, (void *)COMMON_UNFIXED_BIN);
        gui_widget_litegfx_set_image(obj, 14, (void *)COMMON_FIXED_BIN);

        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SHOW_TIME,
                                     (uint32_t)false);//时间显示开关，默认开
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_TIME_FORMAT,
                                     1);//时间制式开关，0：24小时制，1：12小时制，默认0
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_FIXED_BUTTON,
                                     (uint32_t)false);//表盘固定按钮开关，默认关
    }
}

void design_tab_watchface_flowerfall(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite flower fall",
                                                             LX_VGWIDGET_INS_FLOWER_FALL, watchface_flowerfall_user_cb, 0, 0, 454, 454);
}
#endif