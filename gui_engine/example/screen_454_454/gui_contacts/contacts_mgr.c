#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "contacts_mgr.h"

gui_tabview_t *tabview_contacts_mgr = NULL;

const tab_info_t contacts_tab_infos[CONTACTS_MAX] =
{
    {CONTACTS_MAIN, 0, 0, "contacts_main", design_tab_contacts_main, NULL},
    {CONTACTS_LIST, 1, 0, "contacts_list", design_tab_contacts_list, NULL},
    {CONTACTS_DETAILS, 2, 0, "contacts_details", design_tab_contacts_details, NULL},
};

app_tab_t contacts_tabs[CONTACTS_MAX] =
{
    {NULL, &contacts_tab_infos[CONTACTS_MAIN]},
    {NULL, &contacts_tab_infos[CONTACTS_LIST]},
    {NULL, &contacts_tab_infos[CONTACTS_DETAILS]}
};

int app_contacts_switch_tabs(contacts_ids id)
{
    if (contacts_tabs[id].p_tab == NULL)
    {
        app_tab_manager_ctor(tabview_contacts_mgr, &contacts_tabs[id]);
    }
    if (!app_tab_manager_switch(&contacts_tabs[id]))
    {
        if (id == CONTACTS_MAIN)
        {
            gui_tabview_tp_disable(tabview_main, false);
        }
        else
        {
            gui_tabview_tp_disable(tabview_main, true);
        }
        for (uint16_t i = 0; i < CONTACTS_MAX; i++)
        {
            if (i != id)
            {
                app_tab_manager_free(&contacts_tabs[i]);
            }
        }
    }
    else
    {
        gui_log("contacts switch to id %d fail!", id);
        return -1;
    }

    return 0;
}

void design_contacts_mgr(void *parent)
{
    tabview_contacts_mgr = gui_tabview_create(parent, "tabview contacts mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_contacts_mgr, true);

    app_tab_manager_ctor(tabview_contacts_mgr, &contacts_tabs[CONTACTS_MAIN]);
}
