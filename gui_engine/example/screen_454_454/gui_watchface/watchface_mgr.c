#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "watchface_mgr.h"

gui_tabview_t *tabview_watchface_mgr = NULL;

const tab_info_t watchface_tab_infos[WATCHFACE_MAX] =
{
    {WATCHFACE_MAIN,                0, 0, "watchface_main",         design_tab_watchface_main, NULL},
    {WATCHFACE_BUDS,                1, 0, "watchface_bud",          design_tab_buds, NULL},
#ifndef _WIN32
    {WATCHFACE_BUDS_DEVICE,         2, 0, "watchface_bud_device",   design_tab_buds_device, NULL},
    {WATCHFACE_BUDS_SEARCH,         3, 0, "watchface_bud_search",   design_tab_buds_search, NULL},
#endif
    {WATCHFACE_PHONE,               4, 0, "watchface_phone",        design_tab_phone, NULL},
#ifndef _WIN32
    {WATCHFACE_PHONE_PAIR,          5, 0, "watchface_phone_pair",   design_tab_phone_pair, NULL},
#endif
    {WATCHFACE_SETTING,             6, 0, "watchface_setting",      design_tab_setting, NULL},
    {WATCHFACE_CONFIRM,             7, 0, "watchface_confirm",      design_tab_confirm, NULL},
    {WATCHFACE_CUBE,                8, 0, "watchface_cube",         design_tab_cube, NULL},
    {WATCHFACE_PERSPECTIVE,         9, 0, "watchface_perspective",  design_tab_perspective, NULL},
};

app_tab_t watchface_tabs[WATCHFACE_MAX] =
{
    {NULL, &watchface_tab_infos[WATCHFACE_MAIN]},
    {NULL, &watchface_tab_infos[WATCHFACE_BUDS]},
    {NULL, &watchface_tab_infos[WATCHFACE_BUDS_DEVICE]},
    {NULL, &watchface_tab_infos[WATCHFACE_BUDS_SEARCH]},
    {NULL, &watchface_tab_infos[WATCHFACE_PHONE]},
    {NULL, &watchface_tab_infos[WATCHFACE_PHONE_PAIR]},
    {NULL, &watchface_tab_infos[WATCHFACE_SETTING]},
    {NULL, &watchface_tab_infos[WATCHFACE_CONFIRM]},
    {NULL, &watchface_tab_infos[WATCHFACE_CUBE]},
    {NULL, &watchface_tab_infos[WATCHFACE_PERSPECTIVE]},

};

int app_watchface_switch_tabs(watchface_ids id)
{
    if (watchface_tabs[id].p_tab == NULL)
    {
        app_tab_manager_ctor(tabview_watchface_mgr, &watchface_tabs[id]);
    }
    if (!app_tab_manager_switch(&watchface_tabs[id]))
    {
        if (id == WATCHFACE_MAIN)
        {
            gui_tabview_tp_disable(tabview_main, false);
        }
        else
        {
            gui_tabview_tp_disable(tabview_main, true);
        }
        for (uint16_t i = 0; i < WATCHFACE_MAX; i++)
        {
            if (i != id)
            {
                app_tab_manager_free(&watchface_tabs[i]);
            }
        }
    }
    else
    {
        gui_log("watchface switch to id %d fail!", id);
        return -1;
    }

    return 0;
}

void design_watchface_mgr(void *parent)
{
    tabview_watchface_mgr = gui_tabview_create(parent, "tabview watchface mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_watchface_mgr, true);

    app_tab_manager_ctor(tabview_watchface_mgr, &watchface_tabs[WATCHFACE_MAIN]);
}

void free_watchface_mgr(void)
{
    for (uint16_t i = 0; i < WATCHFACE_MAX; i++)
    {
        app_tab_manager_free(&watchface_tabs[i]);
    }

    gui_obj_tree_free(tabview_watchface_mgr);
}

