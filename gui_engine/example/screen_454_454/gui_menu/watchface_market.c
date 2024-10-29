#include "gui_app.h"
#include "gui_win.h"
#define APP_WATCHFACE_MARKET
GUI_APP_DEFINE_NAME(APP_WATCHFACE_MARKET)
GUI_APP_ENTRY(APP_WATCHFACE_MARKET)
{
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());
    extern void create_tree_nest(char *xml, void *obj);
#if _WIN32
    defaultPath = "gui_engine\\example\\screen_454_454\\root_image\\root\\";
    create_tree_nest("gui_engine\\example\\screen_454_454\\root_image\\root\\watchface_market\\watchface1\\watchface1.xml",
                     GUI_APP_ROOT_SCREEN);
#else
    create_tree_nest("stop_watch\\calnedar.xml", GUI_APP_ROOT_SCREEN);
#endif
}