#include <gui_obj.h>
#include <gui_app.h>
#include "gui_tab.h"

extern void *get_app_home(void);
extern void *get_app_tablist(void);
extern void *get_app_applist(void);

uint8_t LAST_TABLIST_X = 0;
uint8_t LAST_TABHOME_X = 0;
extern gui_tabview_t *home_tab;
extern gui_tabview_t *tablist_tab;

void gui_load_bee4_hk(void)
{
    gui_app_startup(get_app_home());
}

void enter_tablist(void *obj, gui_event_t e)
{
    gui_log("enter_tablist \n");
    LAST_TABHOME_X = home_tab->cur_id.x;
    gui_switch_app(get_app_home(), get_app_tablist());
    tablist_tab->cur_id.x = LAST_TABHOME_X - 1;
}

void exit_tablist(void *obj, gui_event_t e)
{
    gui_log("exit_tablist \n");
    LAST_TABLIST_X = tablist_tab->cur_id.x;
    gui_switch_app(get_app_tablist(), get_app_home());
    home_tab->cur_id.x = LAST_TABLIST_X + 1;
}
