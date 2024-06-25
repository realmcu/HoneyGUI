#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"

uintptr_t app_prism_refl_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        //index前4个保留为专用图片 , 4之后为面板图片
        gui_widget_litegfx_set_image(obj, 4, (void *)PRSIM_CLOCK_BIN);
        gui_widget_litegfx_set_image(obj, 5, (void *)PRSIM_HEARTRATE_BIN);
        gui_widget_litegfx_set_image(obj, 6, (void *)PRSIM_SLEEP_BIN);
        gui_widget_litegfx_set_image(obj, 7, (void *)PRSIM_ACTIVITYGOAL_BIN);
        gui_widget_litegfx_set_image(obj, 8, (void *)PRSIM_WEATHER_BIN);
        gui_widget_litegfx_set_image(obj, 9, (void *)PRSIM_ALIPAY_BIN);

        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SET_NUMBER,
                                     6);//设置面数，最小3，最大12，默认为6
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SET_SELECTED,
                                     0);//设置当前选中了第几面，默认为0
    }
    return 0;
}

void design_tab_app_prsim_refl(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite prism refl",
                                                             LX_VGWIDGET_INS_PRISM_REFL, app_prism_refl_user_cb, 0, 0, 454, 454);
}