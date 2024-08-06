#ifndef _GUI_COMMON_MEEEEENU_H_
#define _GUI_COMMON_MEEEEENU_H_


#include "gui_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_MENU_SECTION     __attribute__((section(".app.flash.gui_menu.1")))  __attribute__((used))/*!< data ram code */


void gui_main(void);

#ifdef __cplusplus
}
#endif
#endif /* _GUI_COMMON_MEEEEENU_H_ */

