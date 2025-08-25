/*
 * File      : gui_server.h
 */
#ifndef __GUI_SERVER_H__
#define __GUI_SERVER_H__


#include <guidef.h>
#include <gui_app.h>
#include <gui_obj.h>
#include <gui_message.h>
#ifdef __cplusplus
extern "C" {
#endif
#define GUI_SERVER_THREAD_NAME "gui_s"
typedef void (* touch_sync_cb)(void);
int gui_server_init(void);
void gui_debug_sethook(void (*hook)(void));
bool gui_server_dlps_check(void);
uint32_t gui_spf(void);
void js_run_file_on_server(const char *file, gui_app_t *app);
void gui_server_msg_handler(gui_msg_t *msg);
void gui_server_msg_init(void);
bool gui_send_msg_to_server(gui_msg_t *msg);
void gui_recv_msg_to_server(void);
void gui_server_exec_cb(gui_msg_cb cb);
void gui_task_ext_execution_sethook(void (*hook)(void));
/**
 * @brief Sets the sleep flag for the GUI server, indicating that it should sleep.
 *
 * When the sleep flag is set, the GUI server will not process any messages or
 * update the screen until the flag is cleared by calling gui_server_turnon().
 */
void gui_server_turnoff(void);
/**
 * @brief Turns on the GUI server and sends a display-on event.
 *
 * This function clears the sleep flag for the GUI server and sends a
 * GUI_EVENT_DISPLAY_ON message to the server to indicate that the display
 * should be turned on.
 */

void gui_server_turnon(void);
#ifdef __cplusplus
}
#endif

#endif
