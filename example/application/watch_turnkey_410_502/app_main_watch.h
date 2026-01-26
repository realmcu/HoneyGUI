/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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
typedef enum
{
    WATCHFACE_BIG_NUM = 0,  // watchface number
    WATCHFACE_SPORT = 1,    // watchface sport
    WATCHFACE_VIDEO = 2     // watchface video
} watchface_type_t;
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCROLLBAR_S_H      10
#define SCROLLBAR_M_H      25
#define SCROLLBAR_L_H      45
#define SCROLLBAR_BG_H     60

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
/* time */
extern struct tm *timeinfo;
extern char time_str[];
extern const char *month[12];
extern const char *day[7];

/* Watchface */
extern void (*watchface_clear_mem)(void);
extern watchface_type_t current_watchface_type;

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
// Generate a pseudo-random number
uint16_t xorshift16(void);

/* Send message to regenerate current_view asynchronously */
void msg_2_regenerate_current_view(void);

#ifdef __cplusplus
}
#endif
#endif


