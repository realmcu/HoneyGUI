#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_obj.h"
#include "gui_win.h"
#include "gui_list.h"

#define GUI_OBJ_STACK_SIZE_MAX          10

void set_confirm_yes(void *yes_action, void *obj);
void set_confirm_no(void *no_action, void *obj);
void set_confirm_text(char *confirm_text_display, int16_t x, int16_t y, uint16_t length);
void push_current_widget(void *obj);
gui_obj_t *pop_current_widget(void);
//return child of app_watch_ui, which is showed
gui_obj_t *get_current_active_widget(void);



#ifdef __cplusplus
}
#endif
#endif