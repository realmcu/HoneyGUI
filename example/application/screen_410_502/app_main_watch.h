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
#include "gui_obj_focus.h"
#include "gui_components_init.h"
#include "gui_view_instance.h"

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
#define GUI_OBJ_GET_HANDLE(pointer, name, root_widget) gui_obj_t *pointer = gui_obj_get_handle((void *)root_widget, name);

/*============================================================================*
 *                            Public Functions
 *============================================================================*/
uint16_t xorshift16(void); // Generate a pseudo-random number
void click_button_back_2_watchface_or_menu(void *obj, gui_event_t *e);
void slide_back_2_menu(void *obj, gui_event_t *e);


#ifdef __cplusplus
}
#endif
#endif


