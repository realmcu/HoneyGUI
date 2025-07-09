/*
 * File      : gui_message.h
 */
#ifndef __GUI_MESSAGE_H__
#define __GUI_MESSAGE_H__


#include <guidef.h>
#include <gui_obj.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GUI_EVENT_DISPLAY_ON              = 0x0016,
    GUI_EVENT_DISPLAY_OFF             = 0x0017,
    GUI_EVENT_EXTERN_IO_JS            = 0x0018,  // extern io event: route to js middle layer
    GUI_EVENT_FREE_ALL                = 0x0019,
    GUI_EVENT_RESET_ACTIVE_TIME       = 0x0020,

    GUI_EVENT_USER_DEFINE             = 0x0100,
} gui_msg_event_t;

bool gui_send_msg_to_server(gui_msg_t *msg);
void gui_recv_msg_to_server(void);
void gui_server_msg_handler(gui_msg_t *msg);
void gui_server_msg_init(void);

void gui_set_keep_active_time(uint32_t active_time);

#ifdef __cplusplus
}
#endif

#endif
