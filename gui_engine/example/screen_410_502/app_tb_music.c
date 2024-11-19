#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"

void page_tb_music(void *parent)
{
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 0, 0);
    extern void app_music_ui_design(gui_obj_t *parent);
    app_music_ui_design(GUI_BASE(win_function));
}
