#include "gui_view_macro.h"
#include "gui_text.h"

static void anti_aliasing_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch in\n");
    gui_view_switch_on_event(view, "video_support_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, "fps_show_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    {
        gui_text_t *text = gui_text_create(view, 0, 0, 200, 500, 120);
        gui_text_set(text, "THEQUICK", GUI_FONT_SRC_BMP, gui_color_css("white"), strlen("THEQUICK"), 110);
        gui_text_type_set(text, (void *)FILE_POINTER(ALFASLABONEREGULAR_SIZE110_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    {
        gui_text_t *text = gui_text_create(view, 0, 0, 300, 500, 120);
        gui_text_set(text, "THEQUICK", GUI_FONT_SRC_BMP, gui_color_css("white"), strlen("THEQUICK"), 110);
        gui_text_type_set(text, (void *)FILE_POINTER(MOMOSIGNATUREREGULAR_SIZE110_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    {
        gui_text_t *text = gui_text_create(view, 0, 0, 10 + 55, 500, 14);
        gui_text_set(text, "The quick brown fox jumps over the lazy dog.", GUI_FONT_SRC_BMP,
                     gui_color_css("white"), strlen("The quick brown fox jumps over the lazy dog."), 12);
        gui_text_type_set(text, (void *)FILE_POINTER(ALFASLABONEREGULAR_SIZE12_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    {
        gui_text_t *text = gui_text_create(view, 0, 0, 24 + 55, 500, 24);
        gui_text_set(text, "The quick brown fox jumps over the lazy dog.", GUI_FONT_SRC_BMP,
                     gui_color_css("white"), strlen("The quick brown fox jumps over the lazy dog."), 20);
        gui_text_type_set(text, (void *)FILE_POINTER(ALFASLABONEREGULAR_SIZE20_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    {
        gui_text_t *text = gui_text_create(view, 0, 0, 48 + 55, 500, 36);
        gui_text_set(text, "The quick brown fox jumps over the lazy dog.", GUI_FONT_SRC_BMP,
                     gui_color_css("white"), strlen("The quick brown fox jumps over the lazy dog."), 32);
        gui_text_type_set(text, (void *)FILE_POINTER(ALFASLABONEREGULAR_SIZE32_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    {
        gui_text_t *text = gui_text_create(view, 0, 0, 84 + 55, 500, 70);
        gui_text_set(text, "The quick brown.", GUI_FONT_SRC_BMP,
                     gui_color_css("white"), strlen("The quick brown"), 64);
        gui_text_type_set(text, (void *)FILE_POINTER(ALFASLABONEREGULAR_SIZE64_BITS4_FONT_BIN),
                          FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
}

static void anti_aliasing_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("anti_aliasing_view switch out\n");
}

GUI_VIEW_INSTANCE("anti_aliasing_view", 1, anti_aliasing_switch_in, anti_aliasing_switch_out);

