#include "gui_common.h"
#include "app_gui_main.h"
#ifndef _WIN32
#include "gui_interface.h"
#include "app_gap.h"
#include "gui_server.h"
#endif


#ifndef _WIN32

static T_AUDIO_SUPPORT gui_audio_support_cfg;
static T_STATE gui_bt_status;

T_STATE gui_get_bt_status(void)
{
    gui_bt_status = app_db.bt_state;
    return gui_bt_status;
}

T_AUDIO_SUPPORT gui_get_audio_support(void)
{
    gui_audio_support_cfg = (T_AUDIO_SUPPORT)(app_audio_cfg.support_sink |
                                              (app_audio_cfg.support_local_source << 1));
    return gui_audio_support_cfg;
}
#endif

void gui_update_by_event(gui_event_user_t event, void *data, bool force_update)
{
    extern bool send_msg_to_gui_server(gui_msg_t *msg);
    extern bool gui_server_dlps_check(void);

    if (force_update)
    {
        if (gui_server_dlps_check())
        {
            gui_msg_t p_msg_on;
            p_msg_on.type = GUI_EVENT_DISPLAY_ON;
            send_msg_to_gui_server(&p_msg_on);
        }

        gui_msg_t p_msg;
        p_msg.type = event;
        p_msg.u.payload = data;
        send_msg_to_gui_server(&p_msg);
    }
    else
    {
        gui_msg_t p_msg;
        p_msg.type = event;
        p_msg.u.payload = data;
        send_msg_to_gui_server(&p_msg);
    }
}

int app_tab_manager_ctor(void *parent, app_tab_t *tab)
{
    if (tab == NULL)
    {
        return -1;
    }

    if (tab->p_tab)
    {
        return 0;
    }

    tab->p_tab = gui_tab_create(parent, tab->tab_info->tab_name, 0, 0, 0, 0, tab->tab_info->idx,
                                tab->tab_info->idy);

    if (tab->tab_info->ctor)
    {
        tab->tab_info->ctor(tab->p_tab);
    }

    return 0;
}

int app_tab_manager_free(app_tab_t *tab)
{
    if (tab == NULL)
    {
        return -1;
    }

    if (tab->p_tab == NULL)
    {
        return 0;
    }
    gui_obj_tree_free(tab->p_tab);
    tab->p_tab = NULL;

    return 0;
}

int app_tab_manager_switch(app_tab_t *tab)
{
    if (tab == NULL)
    {
        return -1;
    }

    if (tab->p_tab == NULL)
    {
        return -1;
    }
    gui_tabview_t *tab_view = (gui_tabview_t *)tab->p_tab->base.parent;
    gui_tabview_jump_tab(tab_view, tab->tab_info->idx, tab->tab_info->idy);

    return 0;
}

