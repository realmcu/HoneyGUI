#include "app_launcher.h"
/*clock_rotation function*/


extern void *get_app_album(void);
extern void *get_app_setting(void);
extern void *get_app_flybird(void);
void album_icon_cb(void *obj, gui_event_t e)
{
    gui_app_t *from = get_launcher_app();
    gui_app_t *to = get_app_album();

    gui_switch_app(from, to);
}
void setting_icon_cb(void *obj, gui_event_t e)
{
    gui_app_t *from = get_launcher_app();
    gui_app_t *to = get_app_setting();

    gui_switch_app(from, to);
}
void heartrate_icon_cb(void *obj, gui_event_t e)
{
    extern gui_tabview_t *tabview_0;
    gui_tab_jump(tabview_0, 2, 0);
    gui_log("heartrate_icon_cb");
}
void iconbox_icon_cb(void *obj, gui_event_t e)
{
    extern gui_tabview_t *tabview_0;
    gui_tab_jump(tabview_0, 0, 1);
    gui_log("iconbox_icon_cb");
}
/*icon callback handler 2*/
void btcall_icon_cb(void *obj, gui_event_t e)
{
    gui_app_t *from = get_launcher_app();
    gui_app_t *to = get_app_dialing();

    gui_switch_app(from, to);
}
void home_icon_cb(void *obj, gui_event_t e)
{
    extern gui_tabview_t *tabview_0;
    gui_tab_jump(tabview_0, 0, 0);
    gui_log("home_icon_cb");
}
void iconlist_win_cb(void *obj, gui_event_t e)
{
    home_icon_cb(NULL, GUI_EVENT_INVALIDE);
}
void flappybird_iocn_cb(void *obj, gui_event_t e)
{
    gui_app_t *from = get_launcher_app();
    gui_app_t *to = get_app_flybird();

    gui_switch_app(from, to);
}












