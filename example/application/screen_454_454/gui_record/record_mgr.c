#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "record_mgr.h"

gui_tabview_t *tabview_record_mgr = NULL;

const tab_info_t record_tab_infos[RECORD_MAX] =
{
    {RECORD_MAIN, 0, 0, "record_main", design_tab_record_main, NULL},
    {RECORD_FILES, 1, 0, "record_play", design_tab_record_files, NULL},
    {RECORD_PLAY, 2, 0, "record_play", design_tab_record_play, NULL},
};

app_tab_t record_tabs[RECORD_MAX] =
{
    {NULL, &record_tab_infos[RECORD_MAIN]},
    {NULL, &record_tab_infos[RECORD_FILES]},
    {NULL, &record_tab_infos[RECORD_PLAY]}
};

int app_record_switch_tabs(record_ids id)
{
    if (record_tabs[id].p_tab == NULL)
    {
        app_tab_manager_ctor(tabview_record_mgr, &record_tabs[id]);
    }
    if (!app_tab_manager_switch(&record_tabs[id]))
    {
        if (id == RECORD_MAIN)
        {
            gui_tabview_tp_disable(tabview_main, false);
        }
        else
        {
            gui_tabview_tp_disable(tabview_main, true);
        }
        for (uint16_t i = 0; i < RECORD_MAX; i++)
        {
            if (i != id)
            {
                app_tab_manager_free(&record_tabs[i]);
            }
        }
    }
    else
    {
        gui_log("record switch to id %d failed!", id);
        return -1;
    }

    return 0;
}

void design_record_mgr(void *parent)
{
    tabview_record_mgr = gui_tabview_create(parent, "tabview record mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_record_mgr, true);

    app_tab_manager_ctor(tabview_record_mgr, &record_tabs[RECORD_MAIN]);
}

void free_record_mgr(void)
{
    for (uint16_t i = 0; i < RECORD_MAX; i++)
    {
        app_tab_manager_free(&record_tabs[i]);
    }

    gui_obj_tree_free(tabview_record_mgr);
}
