#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "player_mgr.h"

gui_tabview_t *tabview_player_mgr = NULL;

const tab_info_t player_tab_infos[PLAYER_MAX] =
{
    {PLAYER_MAIN, 0, 0, "player_main", design_tab_player_main, NULL},
    {PLAYER_LIST, 1, 0, "player_list", design_tab_song_lists, NULL},
};

app_tab_t player_tabs[PLAYER_MAX] =
{
    {NULL, &player_tab_infos[PLAYER_MAIN]},
    {NULL, &player_tab_infos[PLAYER_LIST]}
};

int app_player_switch_tabs(player_ids id)
{
    if (player_tabs[id].p_tab == NULL)
    {
        app_tab_manager_ctor(tabview_player_mgr, &player_tabs[id]);
    }
    if (!app_tab_manager_switch(&player_tabs[id]))
    {
        if (id == PLAYER_MAIN)
        {
            gui_tabview_tp_disable(tabview_main, false);
        }
        else
        {
            gui_tabview_tp_disable(tabview_main, true);
        }
        for (uint16_t i = 0; i < PLAYER_MAX; i++)
        {
            if (i != id)
            {
                app_tab_manager_free(&player_tabs[i]);
            }
        }
    }
    else
    {
        gui_log("player switch to id %d fail!", id);
        return -1;
    }

    return 0;
}

void design_player_mgr(void *parent)
{
    tabview_player_mgr = gui_tabview_create(parent, "tabview player mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_player_mgr, true);

    app_tab_manager_ctor(tabview_player_mgr, &player_tabs[PLAYER_MAIN]);
}

void free_player_mgr(void)
{
    for (uint16_t i = 0; i < PLAYER_MAX; i++)
    {
        app_tab_manager_free(&player_tabs[i]);
    }

    gui_obj_tree_free(tabview_player_mgr);
}
