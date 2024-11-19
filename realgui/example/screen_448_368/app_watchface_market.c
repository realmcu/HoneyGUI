#include "gui_app.h"
#include "gui_win.h"

gui_win_t *win_market;
void app_watchface_market(void *parent)
{
    win_market = gui_win_create(parent, "win_market", 0, 0, gui_get_screen_width(),
                                gui_get_screen_height());
    extern void create_tree_nest(char *xml, void *obj);
#if _WIN32
    defaultPath = "realgui\\example\\screen_448_368\\root_image_hongkong\\root\\";
    create_tree_nest("realgui\\example\\screen_448_368\\root_image_hongkong\\root\\watchface\\watchface410x502.xml",
                     win_market);
#else
    create_tree_nest("stop_watch\\calnedar.xml", win_market);
#endif
}