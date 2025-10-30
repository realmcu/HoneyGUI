#ifndef __APP_MAIN_WATCH_H__
#define __APP_MAIN_WATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_view.h"
#include "gui_components_init.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    MESSAGE = 0,
    OS,
} app_name;

typedef struct information
{
    const char *informer;
    const char *content;
    const char *time;
    app_name app;
} information_t;

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
 *                            Variables
 *============================================================================*/
extern struct tm *timeinfo;
extern const char *month[12];
extern const char *day[7];


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
// Generate a pseudo-random number
uint16_t xorshift16(void);

#ifdef __cplusplus
}
#endif
#endif


