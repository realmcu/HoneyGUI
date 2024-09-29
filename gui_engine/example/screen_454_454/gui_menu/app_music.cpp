/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_music.cpp
  * @brief Music player using C++ style
  * @date 2024/09/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <memory> // for smart pointers
#include "gui_switch.h"
#include "../root_image/ui_resource.h"
#include "gui_canvas_rect.h"
#include "gui_button.h"



namespace gui_music_app
{
constexpr int SWITCH_W = 160;
constexpr int SWITCH_H = 120;
static void switchOnCallback(void *null1, void *null2, void *param)
{
    gui_music_play("gui_engine\\example\\screen_454_454\\root_image\\root\\music_player\\sample_3s.mp3");
}

static void switchOffCallback(void)
{
    gui_music_stop();
}

class MusicPlayer
{
public:
    MusicPlayer(gui_obj_t *parent)
        : SCREEN_W(static_cast<int>(gui_get_screen_width())),
          SCREEN_H(static_cast<int>(gui_get_screen_height()))
    {
        setupUI(parent);
    }

private:
    const int SCREEN_W;
    const int SCREEN_H;
    gui_switch_t *sw;
    gui_button_t *skipBack;


    static void onCompletion(void *p, void *this_widget, gui_animate_t *animate)
    {
        if (gui_music_completion_status())
        {
            GUI_API(gui_switch_t).turn_off_no_event(static_cast<gui_switch_t *>(this_widget));
        }
    }

    void setupUI(gui_obj_t *parent)
    {
        gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, APP_COLOR_SILVER);
        sw = (gui_switch_create(parent, SCREEN_W / 2 - SWITCH_W / 2,
                                SCREEN_H / 2 - SWITCH_H / 2, SWITCH_W, SWITCH_H, PLAY_BIN, PAUSE_BIN));

        GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)switchOnCallback, nullptr);
        GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t)switchOffCallback, nullptr);
        gui_img_set_mode(static_cast<gui_img_t *>(sw->switch_picture), IMG_SRC_OVER_MODE);
        GUI_API(gui_switch_t).animate(sw, 1000, -1, (void *)onCompletion, nullptr);

        skipBack = (gui_button_create(parent, 100, 400, 28, 28, SKIPBACK_BIN, SKIPBACKHL_BIN, nullptr,
                                      BUTTON_BG_ICON, 0));
    }
};

void appMusicUIDesign(gui_obj_t *parent)
{
    MusicPlayer player(parent);
}
}
extern "C" {
    void app_music_ui_design(gui_obj_t *parent)
    {
        gui_music_app::appMusicUIDesign(parent);
    }
}
