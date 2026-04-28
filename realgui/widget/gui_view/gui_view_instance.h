/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_VIEW_INSTANCE_H
#define GUI_VIEW_INSTANCE_H

#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_api.h"

/**
 * @brief instance to simplify view definition
 *
 * This instance can quickly create a view, users only need to provide the following parameters:
 * - view_name: View name (string)
 * - view_keep: Whether to keep the view state when switching out (bool value)
 * - on_switch_in_func: Callback function name when view switches in
 * - on_switch_out_func: Callback function name when view switches out
 * - snap_shot: (optional) Whether to use snapshot during view transitions; defaults to false
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
 * GUI_VIEW_INSTANCE("my_view", false, my_switch_in, my_switch_out);           // snap_shot defaults to false
 * GUI_VIEW_INSTANCE("my_view", false, my_switch_in, my_switch_out, true);     // snap_shot enabled
 */
// Helper macros to ensure proper macro parameter expansion
#define _GUI_CONCAT(a, b) a##b
#define _GUI_CONCAT_EXPANDED(a, b) _GUI_CONCAT(a, b)

// Argument-count helpers for optional snap_shot parameter
#define _GUI_NARG_(_1, _2, _3, _4, _5, N, ...) N
#define _GUI_NARG(...) _GUI_NARG_(__VA_ARGS__, 5, 4, 3, 2, 1, 0)

// Intermediate macro implementation
#define _GUI_VIEW_INSTANCE_IMPL(view_name, view_keep, on_switch_in_func, on_switch_out_func, snap_shot, line) \
    static gui_view_t *_GUI_CONCAT_EXPANDED(current_view_line_, line) = NULL; \
    \
    static const gui_view_descriptor_t _GUI_CONCAT_EXPANDED(descriptor_line_, line) = \
            { \
              .name          = (const char *)view_name, \
              .pView         = &_GUI_CONCAT_EXPANDED(current_view_line_, line), \
              .on_switch_in  = on_switch_in_func, \
              .on_switch_out = on_switch_out_func, \
              .keep          = view_keep, \
              .use_snap_shot = snap_shot\
            }; \
    \
    static int _GUI_CONCAT_EXPANDED(gui_view_descriptor_register_init_line_, line)(void) \
    { \
        gui_view_descriptor_register(&_GUI_CONCAT_EXPANDED(descriptor_line_, line)); \
        return 0; \
    } \
    static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(_GUI_CONCAT_EXPANDED(gui_view_descriptor_register_init_line_, line));

// Public instance interface - must be used at file scope, not inside a function.
// snap_shot parameter is optional; omitting it defaults to false (backward compatible).
#define _GUI_VIEW_INSTANCE_4(name, keep, sw_in, sw_out) \
    _GUI_VIEW_INSTANCE_IMPL(name, keep, sw_in, sw_out, false, __LINE__)
#define _GUI_VIEW_INSTANCE_5(name, keep, sw_in, sw_out, snap_shot) \
    _GUI_VIEW_INSTANCE_IMPL(name, keep, sw_in, sw_out, snap_shot, __LINE__)
#define _GUI_VIEW_INSTANCE_DISPATCH(N, ...) _GUI_CONCAT_EXPANDED(_GUI_VIEW_INSTANCE_, N)(__VA_ARGS__)
#define GUI_VIEW_INSTANCE(...) _GUI_VIEW_INSTANCE_DISPATCH(_GUI_NARG(__VA_ARGS__), __VA_ARGS__)
#endif /* GUI_VIEW_INSTANCE_H */
