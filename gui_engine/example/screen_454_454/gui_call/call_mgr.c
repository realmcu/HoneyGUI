#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "call_mgr.h"

gui_tabview_t *tabview_call_mgr = NULL;

const tab_info_t call_tab_infos[CALL_MAX] =
{
    {CALL_MAIN, 0, 0, "call_main", design_tab_call_main, NULL},
    {CALL_DIAL, 1, 0, "call_dail", design_tab_call_dial, NULL},
    {CALL_INCOMMING, 2, 0, "call_incomming", design_tab_call_incomming, NULL},
    {CALL_OUTGOING, 3, 0, "call_outgoing", design_tab_call_outgoing, NULL},
    {CALL_ACTIVE, 4, 0, "call_active", design_tab_call_active, NULL},
};

app_tab_t call_tabs[CALL_MAX] =
{
    {NULL, &call_tab_infos[CALL_MAIN]},
    {NULL, &call_tab_infos[CALL_DIAL]},
    {NULL, &call_tab_infos[CALL_INCOMMING]},
    {NULL, &call_tab_infos[CALL_OUTGOING]},
    {NULL, &call_tab_infos[CALL_ACTIVE]},
};


//for app task
#ifndef _WIN32
char app_dial_num[20];

void set_dial_num(char *dial_num, uint8_t length)
{
    if (length <= 19)
    {
        memset(app_dial_num, '\0', sizeof(app_dial_num));
        memcpy(app_dial_num, dial_num, length);
        app_dial_num[length] = '\0';
    }
    else
    {
        memcpy(app_dial_num, dial_num, 19);
        app_dial_num[19] = '\0';
    }

}
char *get_dial_num(void)
{
    return app_dial_num;
}
#endif

int app_call_switch_tabs(call_ids id)
{
    gui_log("call switch to id %d ", id);
    if (call_tabs[id].p_tab == NULL)
    {
        app_tab_manager_ctor(tabview_call_mgr, &call_tabs[id]);
    }
    if (!app_tab_manager_switch(&call_tabs[id]))
    {
        if (id == CALL_INCOMMING || id == CALL_OUTGOING || id == CALL_ACTIVE)
        {
            gui_tabview_jump_tab(tabview_main, 2, 0);
        }
        if (id == CALL_MAIN)
        {
            gui_tabview_tp_disable(tabview_main, false);
        }
        else
        {
            gui_tabview_tp_disable(tabview_main, true);
        }
        for (uint16_t i = 0; i < CALL_MAX; i++)
        {
            if (i != id)
            {
                app_tab_manager_free(&call_tabs[i]);
            }
        }
    }
    else
    {
        gui_log("call switch to id %d fail!", id);
        return -1;
    }
    return 0;
}

void design_call_mgr(void *parent)
{
    tabview_call_mgr = gui_tabview_create(parent, "tabview call mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_call_mgr, true);
    app_tab_manager_ctor(tabview_call_mgr, &call_tabs[CALL_MAIN]);
}

void free_call_mgr(void)
{
    for (uint16_t i = 0; i < CALL_MAX; i++)
    {
        app_tab_manager_free(&call_tabs[i]);
    }

    gui_obj_tree_free(tabview_call_mgr);
}
