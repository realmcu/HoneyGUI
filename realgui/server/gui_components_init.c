/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include "gui_components_init.h"

#if defined(__EMSCRIPTEN__)

#include "gui_api_os.h"

#define GUI_INIT_MAX_ENTRIES 256

typedef struct
{
    gui_init_fn_t fn;
    char level;
} gui_init_entry_t;

static gui_init_entry_t init_entries[GUI_INIT_MAX_ENTRIES];
static int init_entry_count;
static int init_entry_dropped;

/* Called from constructors before main(); keeps entries sorted by level and,
 * within a level, in registration order (the same order the linker emits
 * a ".honeygui$initN" section on other targets). */
void gui_components_init_register(gui_init_fn_t fn, char level)
{
    if (init_entry_count >= GUI_INIT_MAX_ENTRIES)
    {
        /* Logging is not available yet; reported by gui_components_init(). */
        init_entry_dropped++;
        return;
    }
    int i = init_entry_count++;
    while (i > 0 && init_entries[i - 1].level > level)
    {
        init_entries[i] = init_entries[i - 1];
        i--;
    }
    init_entries[i].fn = fn;
    init_entries[i].level = level;
}

void gui_components_init(void)
{
    static bool inited = false;
    if (inited)
    {
        return;
    }
    inited = true;

    if (init_entry_dropped > 0)
    {
        gui_log("gui_components_init: %d init function(s) dropped, raise GUI_INIT_MAX_ENTRIES\n",
                init_entry_dropped);
    }

    for (int i = 0; i < init_entry_count; i++)
    {
        if (init_entries[i].fn) { init_entries[i].fn(); }
    }
}

#else

static const gui_init_fn_t __honeygui_init_start[] GUI_APP_SECTION(".honeygui$init0") GUI_APP_USED
GUI_FN_ALIGN = { 0 };
static const gui_init_fn_t __honeygui_init_end[]   GUI_APP_SECTION(".honeygui$initZ") GUI_APP_USED
GUI_FN_ALIGN = { 0 };



void gui_components_init(void)
{
    static bool inited = false;
    if (inited)
    {
        return;
    }
    inited = true;
    volatile const gui_init_fn_t *fn_ptr;

    // cppcheck-suppress comparePointers
    for (fn_ptr = __honeygui_init_start + 1; fn_ptr < __honeygui_init_end; ++fn_ptr)
    {
        if (*fn_ptr) { (*fn_ptr)(); }
    }
}

#endif

