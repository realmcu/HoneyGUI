/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_music.cpp
  * @brief Music player using C++ style
  * @link UI: https://pixso.cn/community/file/w8E6cEgxRlMNE0-faxhJjw
  * @date 2024/09/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include<memory>
#include "gui_switch.h"
#include "root_image_hongkong/ui_resource.h"
#include "gui_canvas_rect.h"
#include "gui_button.h"
#include "gui_img.h"
#include "gui_win.h"
#include "tp_algo.h"

#include "gui_page.h"
#include <vector>
#include <string>
namespace std
{
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}
namespace gui_music_app
{
constexpr int SWITCH_W = 28;
constexpr int SWITCH_H = 28;
constexpr int BUTTON_Y = 400;
constexpr int COVER_Y = 10;
constexpr int COVER_W = 300;
constexpr int MINIMIZE_WINDOW_HEIGHT = 60;
constexpr int MINIMIZE_COVER_HEIGHT = 50;
constexpr int LIST_COVER_H = 120;
constexpr int LIST_SONG_H = 65;
class MusicPlayer
{
public:
    MusicPlayer(gui_obj_t *parent)
        : SCREEN_W(static_cast<int>(gui_get_screen_width())),
          SCREEN_H(static_cast<int>(gui_get_screen_height())),
          SWITCH_X(SCREEN_W / 2 - SWITCH_W / 2),
          COVER_X(SCREEN_W / 2 - COVER_W / 2)
    {
        setupUI(parent);
    }
    void win_cover_animation(void *p, void *this_widget, gui_animate_t *animate)
    {
        touch_info_t *touch = tp_get_info();
        gui_win_t *win = GUI_TYPE(gui_win_t, this_widget);
        gui_obj_t *obj = GUI_BASE(this_widget);
        //std::cout << "win_cover_gesture:" << win_cover_gesture << std::endl;
        if (!win_cover_gesture)
        {
            //std::cout << "1:" << std::endl;
            if (touch->pressed && touch->y > SCREEN_H - MINIMIZE_WINDOW_HEIGHT)
            {
                win_cover_gesture = true; //std::cout << "2:" << std::endl;
            }
            else
            {
                //std::cout << "3:" << std::endl;
                return;
            }

        }


        if (touch->pressed)
        {
            win_cover_y_history = win_cover_y;
        }

        if (touch->pressing)
        {
            win_cover_y = touch->deltaY + win_cover_y_history;
        }

        if (touch->released)
        {
            inertia = true;
            release_win_cover_y = win_cover_y;
            offset = 0;
            speed = 30;
            if (win_cover_y <= threshold)
            {
                speed = -speed;
            }
        }

        if (inertia)
        {
            gui_inertial(&speed, 0, &offset);
            win_cover_y = offset + release_win_cover_y;
            if (win_cover_y >= SCREEN_H - MINIMIZE_WINDOW_HEIGHT)
            {
                win_cover_y = SCREEN_H - MINIMIZE_WINDOW_HEIGHT;
                inertia = false;
                threshold = SCREEN_H * 2 / 3;
                win_cover_gesture = false;
            }
            else if (win_cover_y <= 0)
            {
                win_cover_y = 0;
                inertia = false;
                threshold = SCREEN_H / 3;
            }

            gui_log("gui_inertial:%d\n", offset);

        }

        if (win_cover_y > SCREEN_H - MINIMIZE_WINDOW_HEIGHT)
        {
            win_cover_y = SCREEN_H - MINIMIZE_WINDOW_HEIGHT;
        }
        obj->y = win_cover_y;
        GUI_WIDGET_TRY_EXCEPT(img_cover)
        float progress = ((float)win_cover_y / (float)(SCREEN_H - MINIMIZE_WINDOW_HEIGHT));

        int cover_w = lerp(COVER_W, MINIMIZE_COVER_HEIGHT, progress);
        int cover_x = calculate_center(WINDOW_WIGTH, cover_w);
        int cover_y = lerp(COVER_Y, MINIMIZE_COVER_TAREGT_Y, progress);
        float scale = (float)cover_w / (float)COVER_W;
        gui_set_location(GUI_BASE(img_cover), cover_x, cover_y);
        gui_img_scale(img_cover, scale, scale);
        float progress3 = progress * 3;
        if (progress3 > 1)
        {
            progress3 = 1;
        }
        int sw_x = MINIMIZE_SWITCH_TARGT_X;
        int sw_y = lerp(BUTTON_Y, MINIMIZE_SWITCH_TARGT_Y, progress3);
        if (sw_y < cover_y + calculate_center(cover_w, SWITCH_W))
        {
            sw_y = cover_y + calculate_center(cover_w, SWITCH_W);
        }
        gui_set_location(GUI_BASE(sw), sw_x, sw_y);


    }
    void win_list_animation(void *p, void *this_widget, gui_animate_t *animate)
    {
        // std::cout << win_list_gesture_page_y << ";" << GUI_BASE(list_page)->y << std::endl;
        GUI_BASE(list_page)->gesture = win_cover_gesture;


        if (GUI_BASE(list_page)->y > win_list_gesture_page_y && !win_list_gesture)
        {
            win_list_gesture = true;
        }

        if (!win_list_gesture)
        {
            return;
        }

        if (win_cover_y > 0)
        {
            gui_obj_hidden(GUI_BASE(list_cover_win), false);
            win_list_cover_y = GUI_BASE(list_page)->y;
            if (win_list_cover_y > 0)
            {
                win_list_cover_y = 0;
            }
            else if (win_list_cover_y < -LIST_COVER_H)
            {
                win_list_cover_y = -LIST_COVER_H;
                win_list_gesture = false;
                win_list_gesture_page_y = GUI_BASE(list_page)->y;
            }

            gui_set_location(GUI_BASE(list_cover_win), GUI_BASE(list_cover_win)->x, win_list_cover_y);
        }
        else
        {
            gui_obj_hidden(GUI_BASE(list_cover_win), true);
        }



    }
    void win_cover_head_cb(void *obj, gui_event_t e, void *param)
    {
        //gui_win_start_animation(win_cover);
    }

private:
    const int SCREEN_W;
    const int SCREEN_H;
    const int SWITCH_X;
    const int COVER_X;
    const int WINDOW_WIGTH = SCREEN_W;
    const int MINIMIZE_COVER_TAREGT_X = (WINDOW_WIGTH - MINIMIZE_COVER_HEIGHT) / 2;
    const int MINIMIZE_COVER_TAREGT_Y = (MINIMIZE_WINDOW_HEIGHT - MINIMIZE_COVER_HEIGHT) / 2;
    const int MINIMIZE_SWITCH_TARGT_X = MINIMIZE_COVER_TAREGT_X + calculate_center(
                                            MINIMIZE_COVER_HEIGHT, SWITCH_W);
    const int MINIMIZE_SWITCH_TARGT_Y = MINIMIZE_COVER_TAREGT_Y + calculate_center(
                                            MINIMIZE_COVER_HEIGHT, SWITCH_W);;
    gui_switch_t *sw;
    gui_button_t *skipBack;
    gui_button_t *skipFwd;
    gui_img_t *img_cover;
    gui_win_t *win_cover;
    std::string artistName;
    std::string songName;
    std::string albumName;
    gui_color_t color = APP_COLOR_WHITE;
    gui_color_t color_artistName = gui_rgb(230, 230, 230);
    const gui_color_t color_list = gui_rgb(100, 100, 100);
    const gui_color_t color_theme = gui_color_css("#892FE0");
    int progressbar_width = 50;
    int win_cover_y = 0;
    int win_list_cover_y = 0;
    int win_cover_y_history = 0;
    int win_list_cover_y_history = 0;
    bool inertia = false;
    int release_win_cover_y = 0;;
    int speed = 30;
    int offset = 0;
    int threshold = SCREEN_H / 3;
    gui_win_t *list_win;
    gui_win_t *list_cover_win;
    bool win_cover_gesture = true;
    bool win_list_gesture = true;
    int win_list_gesture_page_y = 0;
    gui_page_t *list_page;
    int value;
    static void switchOnCallback(void *null1, void *null2, void *param)
    {
        gui_music_play("gui_engine\\example\\screen_454_454\\root_image\\root\\music_player\\sample_3s.mp3");
    }
    template<typename T>
    T lerp(T start, T end, float progress) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        return (1 - progress) * start + progress * end;
    }
    template<typename T>
    T calculate_center(T container_width, T item_width) const
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        return (container_width - item_width) / 2;
    }
    static void switchOffCallback(void)
    {
        gui_music_stop();
    }
    static void onCompletion(void *p, void *this_widget, gui_animate_t *animate)
    {
        if (gui_music_completion_status())
        {
            GUI_API(gui_switch_t).turn_off_no_event(static_cast<gui_switch_t *>(this_widget));
        }
    }
    static void song_win_press(void *obj, gui_event_t e, void *param)
    {
        gui_obj_hidden(GUI_BASE(param), false);
    }
    static void song_win_release(void *obj, gui_event_t e, void *param)
    {
        gui_obj_hidden(GUI_BASE(param), true);
    }
    static void song_win_click(void *obj, gui_event_t e, void *param)
    {
        //gui_obj_hidden(param, false);
    }
    void displayText(gui_obj_t *parent,  const char *text_string, int x, int y,
                     gui_color_t &color) const
    {
        // This function is to create text GUI element
        const char *text = text_string;
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  x, y,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }

    static void cover_animation(void *p, void *this_widget, gui_animate_t *animate);
    static void list_animation(void *p, void *this_widget, gui_animate_t *animate);
    static void win_cover_head_callback(void *obj, gui_event_t e, void *param);
    void setupUI(gui_obj_t *parent)
    {
        /*LIST WINDOW*/
        list_win = gui_win_create(parent, "win list", 0, 0, SCREEN_W, SCREEN_H);
        gui_win_set_animate(list_win, 1000, -1, list_animation, list_win);
        gui_canvas_rect_create(GUI_BASE(list_win), 0, 0, 0, SCREEN_W, SCREEN_H, color_list);
        list_page = gui_page_create(list_win, "list_page", 0, 0, SCREEN_W, 0);
        std::vector<void *> img_file_vec = {GROUP1000000745_BIN, GROUP1000000744_BIN, GROUP1000000746_BIN, GROUP1000000747_BIN, GROUP1000000748_BIN, GROUP1000000749_BIN};
        int list_count = 20;
        for (size_t i = 0; i < list_count; i++)
        {
            gui_win_t *win = gui_win_create(list_page, 0, 0, 65 * i + LIST_COVER_H + 50, SCREEN_W, LIST_SONG_H);
            gui_canvas_rect_t *rect = gui_canvas_rect_create(GUI_BASE(win), 0, 0, 0, SCREEN_W, LIST_SONG_H,
                                                             color_theme);
            gui_obj_hidden(GUI_BASE(rect), true);
            gui_img_t *img = gui_img_create_from_mem(GUI_BASE(win), 0, img_file_vec[i % img_file_vec.size()],
                                                     calculate_center(SCREEN_W, 342), 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_win_press(win, song_win_press, rect);
            gui_win_release(win, song_win_release, rect);
            gui_win_click(win, song_win_click, rect);
        }
        gui_win_create(list_page, 0, 0, LIST_SONG_H * list_count + LIST_COVER_H + 50, SCREEN_W,
                       MINIMIZE_WINDOW_HEIGHT);
        list_cover_win = gui_win_create(list_win, "win list cover", 0, 0, SCREEN_W, SCREEN_H);
        gui_obj_hidden(GUI_BASE(list_cover_win), true);
        {
            gui_img_t *img = gui_img_create_from_mem(list_cover_win, "list",  LISTCOVER_BIN, 0, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        displayText(GUI_BASE(list_cover_win), "Playlist", calculate_center(SCREEN_W, 80), LIST_COVER_H,
                    color);
        /*COVER WINDOW*/
        win_cover = gui_win_create(parent, "win cover", 0, 0, SCREEN_W, SCREEN_H);
        gui_win_set_animate(win_cover, 1000, -1, cover_animation, win_cover);
        parent = GUI_BASE(win_cover);
        gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, APP_COLOR_SILVER);
        img_cover = (gui_img_create_from_mem(parent, "cover", RECTANGLE86_BIN, COVER_X, COVER_Y, 0, 0));
        gui_img_set_mode(img_cover, IMG_SRC_OVER_MODE);
        sw = (gui_switch_create(parent, SWITCH_X,
                                BUTTON_Y, SWITCH_W, SWITCH_H, PLAY_BIN, PAUSE_BIN));

        GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)switchOnCallback, nullptr);
        GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t)switchOffCallback, nullptr);
        gui_img_set_mode(static_cast<gui_img_t *>(sw->switch_picture), IMG_SRC_OVER_MODE);
        GUI_API(gui_switch_t).animate(sw, 1000, -1, (void *)onCompletion, nullptr);

        skipBack = (gui_button_create(parent, SWITCH_X - SCREEN_W / 4, BUTTON_Y, 28, 28, SKIPBACK_BIN,
                                      SKIPBACKHL_BIN, nullptr,
                                      BUTTON_BG_ICON, 0));
        skipFwd = (gui_button_create(parent, SWITCH_X + SCREEN_W / 4, BUTTON_Y, 28, 28, SKIPFWD_BIN,
                                     SKIPFWDHL_BIN, nullptr,
                                     BUTTON_BG_ICON, 0));
        gui_win_t *win_cover_head = gui_win_create(parent, "win_cover_head", 0, 0, SCREEN_W,
                                                   MINIMIZE_WINDOW_HEIGHT);
        gui_win_press(win_cover_head, win_cover_head_callback, win_cover_head);
        // Display artist, song, and album name

        artistName = "Doja Cat,";
        albumName = "The Weekend";
        songName = "You Right";
        displayText(parent, songName.c_str(), COVER_X, COVER_Y + COVER_W, color);
        displayText(parent, artistName.c_str(), COVER_X, COVER_Y + COVER_W + 18, color_artistName);
        displayText(parent, albumName.c_str(), COVER_X + 80, COVER_Y + COVER_W + 18, color_artistName);
        gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16, COVER_W, 3,
                               color_artistName);
        gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16, progressbar_width, 3,
                               color);
        displayText(parent, "1:24", COVER_X, COVER_Y + COVER_W + 18 + 15 + 16 + 4, color);
        displayText(parent, "3:58", COVER_X + COVER_W - 30, COVER_Y + COVER_W + 18 + 15 + 16 + 4, color);
        gui_img_scale(img_cover, 0.5, 0.5);
    }


};
std::unique_ptr<MusicPlayer> player = nullptr;
void MusicPlayer::cover_animation(void *p, void *this_widget, gui_animate_t *animate)
{
    player->win_cover_animation(p, this_widget, animate);
}
void MusicPlayer::list_animation(void *p, void *this_widget, gui_animate_t *animate)
{
    player->win_list_animation(p, this_widget, animate);
}
void MusicPlayer::win_cover_head_callback(void *obj, gui_event_t e, void *param)
{
    player->win_cover_head_cb(obj, e, param);
}
void appMusicUIDesign(gui_obj_t *parent)
{
    player = std::make_unique<MusicPlayer>(parent);
}
}

extern "C" {
    void app_music_ui_design(gui_obj_t *parent)
    {
        gui_music_app::appMusicUIDesign(parent);
    }
}
