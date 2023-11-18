#ifndef __GUI_COMMON_H__
#define __GUI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_obj.h"
#include "gui_win.h"

#define GUI_OBJ_STACK_SIZE_MAX          10

void set_confirm_yes(void *yes_action, void *obj);
void set_confirm_no(void *no_action, void *obj);
void set_confirm_text(void *text_display, void *obj);

void push_current_widget(void *obj);
gui_obj_t *pop_current_widget(void);



#ifdef __cplusplus
}
#endif
#endif