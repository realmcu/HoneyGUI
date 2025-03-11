/*
 * File      : gui_message.h
 */
#ifndef __GUI_MESSAGE_H__
#define __GUI_MESSAGE_H__


#include <guidef.h>
#include <gui_app.h>
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


#ifdef __cplusplus
}
#endif

#endif
