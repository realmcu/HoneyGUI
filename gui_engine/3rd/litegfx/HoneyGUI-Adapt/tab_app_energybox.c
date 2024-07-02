#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"

uintptr_t app_energy_box_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        //index前4个保留为专用图片 , 4之后为面板图片
        gui_widget_litegfx_set_image(obj, 0, (void *)EBOX_FACE0_BIN);
        gui_widget_litegfx_set_image(obj, 1, (void *)EBOX_LIGHT0_BIN);
    }
    return 0;
}

void design_tab_app_energbox(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite energbox",
                                                             LX_VGWIDGET_INS_ENERGY_BOX, app_energy_box_user_cb, 0, 0, 454, 454);
}