#include "gui_common.h"
#include "app_gui_main.h"
#ifndef _WIN32
#include "gui_interface.h"
#include "app_gap.h"
#include "gui_server.h"
#include "gui_event.h"
#endif

gui_obj_t *obj_stack[GUI_OBJ_STACK_SIZE_MAX] = {NULL};
int8_t obj_stack_index = 0;

gui_obj_t *get_current_active_widget(void)
{
    gui_app_t *app = get_app_watch_ui();
    gui_list_t *node = NULL;

    gui_list_for_each(node, &(app->screen.child_list))
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->not_show == false)
        {
            gui_log("find_obj = 0x%x, find_obj.name = %s\n", find_obj, find_obj->name);
            return find_obj;
        }
    }
    return NULL;
}

void push_current_widget(void *obj)
{

    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX)
    {
        obj_stack[obj_stack_index] = (gui_obj_t *)obj;
    }
    else
    {
        GUI_ASSERT(obj_stack_index >= GUI_OBJ_STACK_SIZE_MAX);
    }
    obj_stack_index++;

    gui_log("push_current_widget: obj_stack = 0x%x obj_stack_index = %d\n",
            obj_stack[obj_stack_index - 1], obj_stack_index);
}

gui_obj_t *pop_current_widget(void)
{
    obj_stack_index--;
    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX && obj_stack_index >= 0)
    {
        gui_log("pop_current_widget obj_stack = %x, name = %s, obj_stack_index = %d\n",
                obj_stack[obj_stack_index], obj_stack[obj_stack_index]->name,
                obj_stack_index);
        gui_fb_change();
        return obj_stack[obj_stack_index];
    }
    else
    {
        GUI_ASSERT(obj_stack_index < 0);
    }
    return NULL;
}

#ifndef _WIN32
uint32_t unicode_to_utf8(uint8_t *utf8, uint32_t len, uint16_t *unicode_array,
                         uint32_t unicode_buf_len)
{
    uint32_t k = 0;
    for (uint32_t i = 0; i < unicode_buf_len; i++)
    {
        if (unicode_array[i] <= 0x7F)
        {
            if (k < len)
            {
                utf8[k] = unicode_array[i];
                k++;
            }
            else
            {
                return k;
            }
        }
        else if (unicode_array[i] >= 0x80 && unicode_array[i] <= 0x7FF)
        {
            if (k + 1 < len)
            {
                utf8[k] = 0xC0 | ((0x7C0 & unicode_array[i]) >> 6);
                utf8[k + 1] = 0x80 | (0x3F & unicode_array[i]);
                k += 2;
            }
            else
            {
                return k;
            }
        }
        else if (unicode_array[i] >= 0x800 && unicode_array[i] <= 0xFFFF)
        {
            if (k + 2 < len)
            {
                utf8[k] = 0xE0 | ((0xF000 & unicode_array[i]) >> 12);
                utf8[k + 1] = 0x80 | ((0xFC0 & unicode_array[i]) >> 6);
                utf8[k + 2] = 0x80 | (0x3F & unicode_array[i]);
                k += 3;
            }
            else
            {
                return k;
            }
        }
    }
    return k;
}

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

void gui_update_by_event2(void *obj, T_GUI_UPDATE_EVENT *gui_update_event, bool force_update)
{
    gui_obj_t *update_obj = (gui_obj_t *)obj;
    if (update_obj)
    {
        if (force_update)
        {
            if (update_obj->type == WINDOW)
            {
                gui_win_t *win = (gui_win_t *)update_obj;
                gui_animate_t *animate = win->animate;
                if (animate)
                {
                    win->animate->animate = true;
                }
            }
        }
        else
        {
            switch (gui_update_event->type)
            {
            case GUI_UPDATE_TYPE_NORMAL:
                {
                    if (update_obj->type == WINDOW)
                    {
                        gui_win_t *win = (gui_win_t *)update_obj;
                        gui_animate_t *animate = win->animate;
                        if (animate)
                        {
                            win->animate->animate = true;
                        }
                    }
                }
                break;
            case GUI_UPDATE_INCOMING_CALL:
                {
                    gui_incoming_call_create();
                }
                break;
            case GUI_FREE_CALLING:
                {
                    //todo: restruct as gui_view_destruct()
                    //do free, pop and design
                    if (update_obj == (gui_obj_t *)win_calling)
                    {
                        gui_win_calling_destruct();
                    }
                }
                break;
            case GUI_UPDATE_PLAYER:
                {
                    extern void tab_player_update_cb(void *p);
                    tab_player_update_cb(NULL);
                }
                break;
            default:
                break;
            }
        }
    }
    else
    {
        //todo:?
    }
}

void gui_update_by_event(gui_event_t event, void *data, bool force_update)
{
    extern bool send_msg_to_gui_server(gui_msg_t *msg);

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

//for app task
char app_dial_num[20];

void set_dial_num(char *dial_num)
{
    if (strlen(dial_num) <= 20)
    {
        memcpy(app_dial_num, dial_num, strlen(dial_num) + 1);
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

