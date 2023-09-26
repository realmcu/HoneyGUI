#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_magic_img.h>

void design_tab_home(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", HOME_BACKGROUND_BIN, 0,
                                                             0, 0, 0);
}
