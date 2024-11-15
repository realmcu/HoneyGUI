#include "gui_app.h"
#include "gui_win.h"
#define APP_WATCHFACE_MARKET
GUI_APP_DEFINE_NAME(APP_WATCHFACE_MARKET)
GUI_APP_ENTRY(APP_WATCHFACE_MARKET)
{
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());
    extern void create_tree_nest(char *xml, void *obj);
    const char *path = gui_get_path_by_relative("app\\watchface1\\watchface1.xml");
    create_tree_nest((void *)path, GUI_APP_ROOT_SCREEN);
    gui_free((void *)path);
}