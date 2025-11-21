#ifndef GUI_VIEW_INSTANCE_H
#define GUI_VIEW_INSTANCE_H

#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_api.h"

/**
 * @brief instance to simplify view definition
 *
 * This instance can quickly create a view, users only need to provide the following four parameters:
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
    static gui_view_t *current_view = NULL; \
    \
    static const gui_view_descriptor_t _GUI_CONCAT_EXPANDED(descriptor_line_, line) = \
            { \
              .name          = (const char *)view_name, \
              .pView         = &current_view, \
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

// Public instance interface
#define GUI_VIEW_INSTANCE(view_name, view_keep, on_switch_in_func, on_switch_out_func) \
    _GUI_VIEW_INSTANCE_IMPL(view_name, view_keep, on_switch_in_func, on_switch_out_func, __LINE__)

#endif /* GUI_VIEW_INSTANCE_H */