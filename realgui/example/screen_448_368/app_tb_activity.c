#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"

// extern void callback_prism(void *obj, gui_event_t e);
// void page_tb_activity(void *parent)
// {
//     gui_img_create_from_mem(parent, "page-activity", ACTIVITY_BIN, 0, 0, 0, 0);
//     gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
//     gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);
// }

#define SCREEN_WIDTH 368   //410
#define SCREEN_HEIGHT 448  //502
#define SCREEN_X_OFF 0     //21
#define SCREEN_Y_OFF 0     //18

void page_tb_activity(void *parent)
{
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    extern void app_music_ui_design(gui_obj_t *parent);
    app_music_ui_design(GUI_BASE(win_function));
}