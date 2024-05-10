#include "gui_obj.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_common.h"
#include "benchmark_common.h"
#include "benchmark_mgr.h"


static void win_benchmark_start_cb(void *obj, uint16_t event)
{
    switch (event)
    {
    case GUI_EVENT_TOUCH_LONG:
    case GUI_EVENT_BENCHMARK_START:
        {
            app_benchmark_start();
        }
        break;
    default:
        break;
    }
}

void design_tab_benchmark_main(void *parent)
{
    char *text_display_info = "long press to start benchmark";
    gui_win_t *win = gui_win_create(parent, "win_benchmark_main", 0, 0, 0, 0);
    gui_obj_add_event_cb(win, (gui_event_cb_t)win_benchmark_start_cb,
                         (gui_event_t)GUI_EVENT_BENCHMARK_START, NULL);
    gui_win_long(win, win_benchmark_start_cb, NULL);

    gui_text_t *text = gui_text_create(parent, "benchmark_start_hint", 0, 227, 454, 32);
    gui_text_set(text, text_display_info, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_display_info),
                 32);
    gui_text_mode_set(text, CENTER);
}
