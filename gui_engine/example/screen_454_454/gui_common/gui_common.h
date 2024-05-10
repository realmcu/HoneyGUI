#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_obj.h"
#include "gui_win.h"
#include "gui_list.h"
#ifdef _ENABLE_RTK_SOC_WATCH_
#include "gui_interface.h"

//for bt status
#include "app_main.h"
#include "app_cfg.h"

typedef enum
{
    AUDIO_SUPPORT_INVALID,
    AUDIO_SUPPORT_SINK,
    AUDIO_SUPPORT_SOURCE,
    AUDIO_SUPPORT_BOTH
} T_AUDIO_SUPPORT;
#endif

#define GUI_OBJ_STACK_SIZE_MAX          10

//for win_confirm
void set_confirm_yes(void yes_action(void *), void *obj);
void set_confirm_no(void no_action(void *), void *obj);
void set_confirm_text(char *confirm_text_display, int16_t x, int16_t y, uint16_t length);

void push_current_widget(void *obj);
gui_obj_t *pop_current_widget(void);
//return child of app_watch_ui, which is showed
gui_obj_t *get_current_active_widget(void);

#ifdef _ENABLE_RTK_SOC_WATCH_
//for curtain_menu
T_STATE gui_get_bt_status(void);
T_AUDIO_SUPPORT gui_get_audio_support(void);

//for win_buds
void gui_set_create_connect_by_user(bool flag);
bool gui_get_create_connect_by_user(void);
void gui_set_create_connect_by_user_addr(uint8_t *addr);
uint8_t *gui_get_create_connect_by_user_addr(void);

uint32_t unicode_to_utf8(uint8_t *utf8, uint32_t len, uint16_t *unicode_array,
                         uint32_t unicode_buf_len);
#endif

#ifdef __cplusplus
}
#endif
#endif