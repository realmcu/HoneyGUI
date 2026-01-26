/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_VIEW_MACRO_H
#define GUI_VIEW_MACRO_H

#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
/**
 * @brief Macro to simplify view definition
 *
 * This macro can quickly create a view, users only need to provide the following four parameters:
 * - view_name: View name (string)
 * - view_keep: Whether to keep the view state when switching out (bool value)
 * - on_switch_in_func: Callback function name when view switches in
 * - on_switch_out_func: Callback function name when view switches out
 *
 * Usage example:
 * static void my_switch_in(gui_view_t *view)
 * {
 *     GUI_UNUSED(view);
 *     gui_log("my_view switch in\n");
 * }
 *
 * static void my_switch_out(gui_view_t *view)
 * {
 *     GUI_UNUSED(view);
 *     gui_log("my_view switch out\n");
 * }
 *
 * GUI_VIEW_INSTANCE("my_view", false, my_switch_in, my_switch_out);
 */
// Helper macros to ensure proper macro parameter expansion
#define _GUI_CONCAT(a, b) a##b
#define _GUI_CONCAT_EXPANDED(a, b) _GUI_CONCAT(a, b)

// Intermediate macro implementation
#define _GUI_VIEW_INSTANCE_IMPL(view_name, view_keep, on_switch_in_func, on_switch_out_func, line) \
    static gui_view_t *_GUI_CONCAT_EXPANDED(current_view_line_, line) = NULL; \
    \
    static const gui_view_descriptor_t _GUI_CONCAT_EXPANDED(descriptor_line_, line) = \
            { \
              .name          = (const char *)view_name, \
              .pView         = &_GUI_CONCAT_EXPANDED(current_view_line_, line), \
              .on_switch_in  = on_switch_in_func, \
              .on_switch_out = on_switch_out_func, \
              .keep          = view_keep \
            }; \
    \
    static int _GUI_CONCAT_EXPANDED(gui_view_descriptor_register_init_line_, line)(void) \
    { \
        gui_view_descriptor_register(&_GUI_CONCAT_EXPANDED(descriptor_line_, line)); \
        return 0; \
    } \
    static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(_GUI_CONCAT_EXPANDED(gui_view_descriptor_register_init_line_, line));

// Public macro interface
#define GUI_VIEW_INSTANCE(view_name, view_keep, on_switch_in_func, on_switch_out_func) \
    _GUI_VIEW_INSTANCE_IMPL(view_name, view_keep, on_switch_in_func, on_switch_out_func, __LINE__)
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
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
#endif /* GUI_VIEW_MACRO_H */