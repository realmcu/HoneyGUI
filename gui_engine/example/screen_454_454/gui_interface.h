#ifndef __GUI_INTERFACE_H__
#define __GUI_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_win.h"
#include "gui_tab.h"
#include "gui_event.h"
#include "player_mgr.h"
#include "call_mgr.h"

typedef enum
{
    GUI_EVENT_USER_INIT = GUI_EVENT_USER_DEFINE,
    GUI_EVENT_CALL_INCOMING,
    GUI_EVENT_CALL_OUTGOING,
    GUI_EVENT_CALL_ACTIVE,
    GUI_EVENT_CALL_VOLUME,
    GUI_EVENT_CALL_END,
    GUI_EVENT_PLAYER,
    GUI_EVENT_PHONE,
    GUI_EVENT_BUDS,
    GUI_EVENT_BUDS_DEVICES,
    GUI_EVENT_BUDS_SEARCH,
    GUI_EVENT_BENCHMARK_START,
    GUI_EVENT_BENCHMARK_NEXT_SCENRAIO,
    GUI_EVENT_BENCHMARK_UPDATE_FRAME_COST,
    GUI_EVENT_RECORD,
    GUI_EVENT_WATCHFACE_UPDATE,
} gui_event_user_t;


typedef struct
{
    uint8_t bd_addr[6];
    uint8_t nam_len;
    uint16_t device_name[20];
    uint32_t cod;
} T_SEARCH_RESULT;

void gui_update_by_event(gui_event_user_t event, void *data, bool force_update);

#ifdef __cplusplus
}
#endif
#endif
