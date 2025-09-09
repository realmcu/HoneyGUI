#ifndef __APP_MAIN_DASHBOARD_H__
#define __APP_MAIN_DASHBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "root_image_800_480/ui_resource.h"
#include "gui_view.h"
#include "gui_components_init.h"

/*============================================================================*
 *                           Variables
 *============================================================================*/

uint8_t scene_flag;
gui_win_t *win_map;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT(obj != NULL);GUI_ASSERT((GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}

/**
* @brief Macro to get a widget pointer by name from the root of a GUI object tree.
*
* This macro assigns a widget pointer by searching for a widget with the specified name within the given root.
* If the widget is found, the pointer is set to it; otherwise, it remains NULL. Additionally, the macro handles
* exceptions if the widget retrieval fails.
*
* @param pointer The name of the pointer that will hold the widget.
* @param name The name of the widget to search for.
* @param root_widget The root widget of the GUI object tree to search within.
*/
#define GUI_WIDGET_POINTER_BY_NAME_ROOT(pointer, name, root_widget) gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_name(gui_obj_t *, const char *, gui_obj_t **);\
    gui_obj_tree_get_widget_by_name((void *)root_widget, name, &pointer);


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
// Map
void map_design(void);
void exit_map(void);
void change_pos_indicator_color(void);

#ifdef __cplusplus
}
#endif
#endif


