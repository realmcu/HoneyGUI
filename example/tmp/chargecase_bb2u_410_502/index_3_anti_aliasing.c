#include "gui_view_macro.h"
#include "gui_text.h"

static void anti_aliasing_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch in\n");
    gui_view_switch_on_event(view, gui_view_descriptor_get("video_support_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, gui_view_descriptor_get("fps_show_view"),
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_text_t *text = gui_text_create(view, 0, 0, 100, 320, 32);
    gui_text_set(text, "Honeygui", GUI_FONT_SRC_BMP, gui_color_css("white"), strlen("Honeygui"), 32);
    gui_text_type_set(text, (void *)FILE_POINTER(CAPTION_1_BIN), FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
}

static void anti_aliasing_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch out\n");
}

GUI_VIEW_INSTANCE("anti_aliasing_view", false, anti_aliasing_switch_in, anti_aliasing_switch_out);

