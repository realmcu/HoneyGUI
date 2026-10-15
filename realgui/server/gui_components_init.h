/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_COMPONENTS_H__
#define __GUI_COMPONENTS_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef int (*gui_init_fn_t)(void);


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_APP_USED                     __attribute__((used))
#define GUI_APP_SECTION(x)                  __attribute__((section(x)))
#define GUI_FN_ALIGN                    __attribute__((aligned(sizeof(void *))))


#if defined(__EMSCRIPTEN__)
/* WebAssembly has no linker scripts, so the ".honeygui$init*" section table
 * cannot be collected. Each export registers itself from a constructor
 * instead; gui_components_init() then runs the entries in level order.
 *
 * Callers may write "static GUI_INIT_xxx(fn);", so the expansion must start
 * with the variable declaration that a leading "static" applies to. The
 * constructor is declared after it; the trailing declaration consumes the
 * caller's semicolon. */
void gui_components_init_register(gui_init_fn_t fn, char level);

#define GUI_APP_INIT_EXPORT(fn, level)                                                       \
    GUI_APP_USED const gui_init_fn_t __honeygui_init_##fn = fn;                              \
    __attribute__((constructor)) static void __honeygui_init_register_##fn(void)            \
    {                                                                                        \
        gui_components_init_register(fn, (level)[0]);                                        \
    }                                                                                        \
    extern const gui_init_fn_t __honeygui_init_##fn
#else
#define GUI_APP_INIT_EXPORT(fn, level)                                                       \
    GUI_APP_USED const gui_init_fn_t __honeygui_init_##fn GUI_APP_SECTION(".honeygui$init" level) GUI_FN_ALIGN= fn
#endif

/* board init routines will be called in board_init() function */
#define GUI_INIT_BOARD_EXPORT(fn)           GUI_APP_INIT_EXPORT(fn, "1")

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initialization) */
#define GUI_INIT_PREV_EXPORT(fn)            GUI_APP_INIT_EXPORT(fn, "2")
/* device initialization */
#define GUI_INIT_DEVICE_EXPORT(fn)          GUI_APP_INIT_EXPORT(fn, "3")
/* components initialization (dfs, lwip, ...) */
#define GUI_INIT_VIEW_DESCRIPTOR_REGISTER(fn)         GUI_APP_INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
#define GUI_INIT_VIEW_DESCRIPTOR_GET(fn)            GUI_APP_INIT_EXPORT(fn, "5")

#define GUI_INIT_VIEW_CREATE(fn)            GUI_APP_INIT_EXPORT(fn, "6")
/* appliation initialization (rtgui application etc ...) */
#define GUI_INIT_APP_EXPORT(fn)             GUI_APP_INIT_EXPORT(fn, "7")


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/


#ifdef __cplusplus
}
#endif
#endif















