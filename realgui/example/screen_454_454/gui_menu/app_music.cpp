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

extern "C" {


#include "guidef.h"
    /**
     * @brief Play the specified music file.
     *
     * This function starts playing the music file specified by the path `music_file`.
     *
     * @param[in] music_file The path to the music file to play.
     *
     * @return #GUI_SUCCESS if the operation was successful.
     */
    gui_error_t gui_music_play(const char *music_file);

    /**
     * @brief Stop the currently playing music.
     *
     * This function stops playback of any music currently being played.
     *
     * @return #GUI_SUCCESS if the music was successfully stopped.
     */
    gui_error_t gui_music_stop(void);

    /**
     * @brief Check if the music playback has completed.
     *
     * This function checks if the currently playing music has finished playing.
     *
     * @return true if the music playback has completed.
     * @return false if the music is still playing.
     */
    bool gui_music_completion_status(void);
    /**
     * @brief Get the length of the loaded music track.
     *
     * This function returns the total length of the currently loaded music track in seconds.
     *
     * @return The total length of the music track, in seconds. Returns 0 if no music is loaded.
     */
    float gui_music_length(void);

    /**
     * @brief Get the current playback time of the music track.
     *
     * This function returns the current playback position of the loaded music track in seconds.
     *
     * @return The current playback position, in seconds. Returns 0 if no music is loaded.
     */
    float gui_music_current_time(void);

    /**
     * @brief Load a music file.
     *
     * This function loads a music file for playback. The file specified by `music_file` will
     * be loaded and can be played using other functions in this module.
     *
     * @param music_file The path to the music file to be loaded.
     * @return A gui_error_t error code.
     *         - GUI_SUCCESS if the file is loaded successfully.
     *         - GUI_ERROR_NULL if the `music_file` parameter is null.
     *         - GUI_ERROR_FORMAT if the file format is unsupported.
     *         - GUI_ERROR for any other generic error condition.
     */
    gui_error_t gui_music_load(const char *music_file);
    gui_error_t gui_music_play(const char *music_file)
    {
#if _WIN32
        extern int win32_play_music(const char *music_file);
        win32_play_music(music_file);
#endif
        return GUI_SUCCESS;
    }
    gui_error_t gui_music_stop()
    {
#if _WIN32
        extern int win32_stop_music(void);
        win32_stop_music();
#endif
        return GUI_SUCCESS;
    }
    bool gui_music_completion_status()
    {
#if _WIN32
        extern bool win32_music_completion_status(void);
        return win32_music_completion_status();
#endif
        return 0;
    }
    float gui_music_length()
    {
        float length = 0;
#if _WIN32
        extern double win32_music_get_music_length();
        length = (float)win32_music_get_music_length();
#endif
        return length;
    }
    float gui_music_current_time()
    {
        float time = 0;
#if _WIN32
        extern  double win32_music_get_music_current_time();
        time = (float)win32_music_get_music_current_time();
#endif
        return time;
    }
    gui_error_t gui_music_load(const char *music_file)
    {
        gui_error_t e = GUI_ERROR;
#if _WIN32
        extern int win32_load_music(const char *music_file);
        int rst = win32_load_music(music_file);
        if (rst == 0)
        {
            e = GUI_SUCCESS;
        }
#endif
        return e;
    }
}
#include<memory>
#include "gui_switch.h"
#include "../root_image/ui_resource.h"
#include "gui_canvas_rect.h"
#include "gui_button.h"
#include "gui_img.h"
#include "gui_win.h"
#include "tp_algo.h"
#include "gui_pagelist_new.h"
#include "gui_page.h"
#include "gui_text.h"
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip> // for std::setw and std::setfill
#if __cplusplus <= 201103L
namespace std
{
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}
#endif
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
std::string mp3_file =
    "realgui\\example\\screen_454_454\\root_image\\SDCARD\\music\\Free_Test_Data_5MB_MP3.mp3";
gui_text_t *current_time;
gui_canvas_rect_t *current_time_bar;
gui_pagelist_new_t *pl;
int page_list_item_space;
int lyrics_array_length;
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
        if (!win_lrc_gesture)
        {
            //std::cout << "1:" << std::endl;
            if (touch->pressed && touch->y < MINIMIZE_WINDOW_HEIGHT)
            {
                win_lrc_gesture = true; //std::cout << "2:" << std::endl;
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



        if (win_cover_y >= 0)
        {
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
            {


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
                gui_log("y:%d\n", win_cover_y);

            }

        }
        if (win_cover_y <= 0)
        {
            if (touch->released)
            {
                inertia = true;
                release_win_cover_y = win_cover_y;
                offset = 0;
                speed = 30;
                if (!(-win_cover_y <= threshold))
                {
                    speed = -speed;
                }
            }
            {


                if (inertia)
                {
                    gui_inertial(&speed, 0, &offset);
                    win_cover_y = offset + release_win_cover_y;
                    if (-win_cover_y >= SCREEN_H - MINIMIZE_WINDOW_HEIGHT)
                    {
                        win_cover_y = -(SCREEN_H - MINIMIZE_WINDOW_HEIGHT);
                        inertia = false;
                        threshold = SCREEN_H * 2 / 3;
                        win_lrc_gesture = false;
                    }
                    else if (win_cover_y >= 0)
                    {
                        win_cover_y = 0;
                        inertia = false;
                        threshold = SCREEN_H / 3;
                    }

                    gui_log("gui_inertial:%d\n", offset);

                }

                if (-win_cover_y > SCREEN_H - MINIMIZE_WINDOW_HEIGHT)
                {
                    win_cover_y = -(SCREEN_H - MINIMIZE_WINDOW_HEIGHT);
                }
                obj->y = win_cover_y;
                GUI_WIDGET_TRY_EXCEPT(img_cover)
                float progress = (-(float)win_cover_y / (float)(SCREEN_H - MINIMIZE_WINDOW_HEIGHT));

                int cover_w = lerp(COVER_W, MINIMIZE_COVER_HEIGHT, progress);
                int cover_x = calculate_center(WINDOW_WIGTH, cover_w);
                int cover_y = lerp(COVER_Y, MINIMIZE_COVER_TAREGT_Y_LRC, progress);
                float scale = (float)cover_w / (float)COVER_W;
                gui_set_location(GUI_BASE(img_cover), cover_x, cover_y);
                gui_img_scale(img_cover, scale, scale);
                float progress3 = progress * 3;
                if (progress3 > 1)
                {
                    progress3 = 1;
                }
                int sw_x = MINIMIZE_SWITCH_TARGT_X;
                int sw_y = lerp(BUTTON_Y, MINIMIZE_SWITCH_TARGT_Y_LRC, progress3);
                if (sw_y < cover_y + calculate_center(cover_w, SWITCH_W))
                {
                    sw_y = cover_y + calculate_center(cover_w, SWITCH_W);
                }
                gui_set_location(GUI_BASE(sw), sw_x, sw_y);
                gui_log("y:%d,%d,%d\n", win_cover_y, sw_x, sw_y);
                if (win_cover_y == 0)
                {
                    gui_obj_hidden(GUI_BASE(lrc_win), 1);
                }
                else
                {
                    gui_obj_hidden(GUI_BASE(lrc_win), 0);
                }
                GUI_BASE(pl)->gesture = win_lrc_gesture;

            }

        }

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
            gui_obj_hidden(GUI_BASE(list_win), false);
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
            gui_obj_hidden(GUI_BASE(list_win), true);
        }
        gui_log("GUI_BASE(list_cover_win)->not_show:%d,%d\n", win_cover_y, GUI_BASE(list_win)->not_show);



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
                                            MINIMIZE_COVER_HEIGHT, SWITCH_W);
    const int MINIMIZE_COVER_TAREGT_Y_LRC = SCREEN_H - ((MINIMIZE_WINDOW_HEIGHT -
                                                         MINIMIZE_COVER_HEIGHT) / 2 + MINIMIZE_COVER_HEIGHT);
    const int MINIMIZE_SWITCH_TARGT_Y_LRC = MINIMIZE_COVER_TAREGT_Y_LRC + calculate_center(
                                                MINIMIZE_COVER_HEIGHT, SWITCH_H);
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
    gui_win_t *lrc_win;
    bool win_cover_gesture = true;
    bool win_lrc_gesture = true;
    bool win_list_gesture = true;
    int win_list_gesture_page_y = 0;
    bool playing_flag;
    gui_page_t *list_page;
    int value;


//static void ticket(void *obj, gui_event_t e, void *param);
// static const void *item_click_function_array[] =
// {
//     ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket,
//     ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket, ticket,
// };
    const char *item_text_array;
// Example LRC content
    std::string lrcContent = R"(
[ti:Sample Song]
[ar:Sample Artist]
[al:Sample Album]
[by:Generated Lyrics]

[00:00.00]01xxxxABCDEFG
[00:04.00]02xxxxABCDEF
[00:08.00]03xxxxABCDE
[00:12.00]04xxxxABCD
[00:16.00]05xxxxABC
[00:20.00]06xxxxAB
[00:24.00]07xxxxA
[00:28.00]08xxxxAB
[00:32.00]09xxxxABC
[00:36.00]10xxxxABCD
[00:40.00]11xxxxABCDE
[00:44.00]12xxxxABCDEF
[00:48.00]13xxxxABCDEFG
[00:52.00]14xxxxABCDEF
[00:56.00]15xxxxABCDE
[01:00.00]16xxxxABCD
[01:04.00]17xxxxABC
[01:08.00]18xxxxAB
[01:12.00]19xxxxA
[01:16.00]20xxxxAB
[01:20.00]21xxxxABC
[01:24.00]22xxxxABCD
[01:28.00]23xxxxABCDE
[01:32.00]24xxxxABCDEF
[01:36.00]25xxxxABCDEFG
[01:40.00]26xxxxABCDEF
[01:44.00]27xxxxABCDE
[01:48.00]28xxxxABCD
[01:52.00]29xxxxABC
[01:56.00]30xxxxAB
[02:00.00]31xxxxA
[02:04.00]32xxxxAB
[02:08.00]33xxxxABC
[02:12.00]34xxxxABCD
[02:16.00]35xxxxABCDE
[02:20.00]36xxxxABCDEF
[02:24.00]37xxxxABCDEFG
[02:28.00]38xxxxABCDEFG
[02:32.00]39xxxxABCDEFG
[02:36.00]40xxxxABCDEFG
[02:40.00]41xxxxABCDEFG
[02:44.00]42xxxxABCDEFG
[02:48.00]43xxxxABCDEFG
[02:52.00]44xxxxABCDEFG
[02:56.00]45xxxxABCDEFG
[03:00.00]46xxxxABCDEFG
[03:04.00]47xxxxABCDEFG
[03:08.00]48xxxxABCDEFG
[03:12.00]49xxxxABCDEFG
[03:16.00]50xxxxABCDEFG
[03:20.00]51xxxxABCDEFG
[03:24.00]52xxxxABCDEFG
[03:28.00]53xxxxABCDEFG
[03:32.00]54xxxxABCDEFG
[03:36.00]55xxxxABCDEFG
[03:40.00]56xxxxABCDEFG
[03:44.00]57xxxxABCDEFG
[03:48.00]58xxxxABCDEFG
[03:52.00]59xxxxABCDEFG
[03:56.00]60xxxxABCDEFG
    )";


    static void switchOnCallback(void *null1, void *null2, void *param)
    {
        //gui_music_play("realgui\\example\\screen_454_454\\root_image\\root\\music_player\\sample_3s.mp3");
        gui_music_play(mp3_file.c_str());

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
        static char current_time_array[6];
        memset(current_time_array, 0, 6);
        float time = gui_music_current_time();
        float duration = gui_music_length();
        //gui_log("gui_music_current_time():%f\n", time);
        std::string current_time_string = formatTime(time);

        memcpy(current_time_array, current_time_string.c_str(), 6);
        //gui_log("current_time_array:%s\n", current_time_array);
        gui_text_content_set(current_time, current_time_array, 5);
        gui_canvas_rect_set_size(current_time_bar, time / duration * COVER_W, 3);
        pl->app_y = -(time / duration * (page_list_item_space * lyrics_array_length -
                                         gui_get_screen_height() / 2));
        pl->app_take_over = true;
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
    gui_text_t *displayText(gui_obj_t *parent,  const char *text_string, int x, int y,
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
        return t;
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
        gui_obj_hidden(GUI_BASE(list_win), true);
        {
            gui_img_t *img = gui_img_create_from_mem(list_cover_win, "list",  LISTCOVER_BIN, 0, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        displayText(GUI_BASE(list_cover_win), "Playlist", calculate_center(SCREEN_W, 80), LIST_COVER_H,
                    color);
        /*LYRIC WINDOW*/
        lrc_win = gui_win_create(parent, "LYRIC", 0, 0, SCREEN_W, SCREEN_H);
        gui_canvas_rect_create(GUI_BASE(lrc_win), 0, 0, 0, SCREEN_W, SCREEN_H, gui_color_css("GhostWhite"));
        int arrayLength = 0;
        char **allLyricsArray = getLyricAtTime(arrayLength, lrcContent);
        lyrics_array_length = arrayLength;
        page_list_item_space = 40;

        // std::cout << "Lyrics from 0 to 60 seconds:" << std::endl;
        for (int i = 0; allLyricsArray[i] != nullptr; ++i)
        {
            // std::cout << lyricsArray[i] << std::endl;
            gui_log("%s\n", allLyricsArray[i]);
            //delete[] lyricsArray[i];  // Free each string
        }




        pl = gui_pagelist_new_create(
                 lrc_win,
                 0, 0, WINDOW_WIGTH, page_list_item_space, (const uint8_t *)SKIPBACK_BIN,
                 (const uint8_t *)SKIPBACK_BIN,
                 IMG_SRC_OVER_MODE,
                 (const uint8_t *)MICHROMA_REGULAR_SIZE16_BITS4_FONT_BIN,
                 16, APP_COLOR_RED

             );
        gui_page_list_new_render(pl, arrayLength, 0, (const char **)allLyricsArray);





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
        gui_music_load(mp3_file.c_str());
        float length = gui_music_length();
        artistName = "FreeTestData.com";
        albumName = "Sample Audio";
        songName = "Free_Test_Data_5MB_MP3";
        std::string length_string = formatTime(length);
        displayText(parent, songName.c_str(), COVER_X, COVER_Y + COVER_W, color);
        displayText(parent, artistName.c_str(), COVER_X, COVER_Y + COVER_W + 18, color_artistName);
        displayText(parent, albumName.c_str(), COVER_X + 180, COVER_Y + COVER_W + 18, color_artistName);
        gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16, COVER_W, 3,
                               color_artistName);
        current_time_bar = gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16,
                                                  progressbar_width, 3,
                                                  color);
        current_time = displayText(parent, "0:00", COVER_X, COVER_Y + COVER_W + 18 + 15 + 16 + 4, color);
        displayText(parent, gui_strdup(length_string.c_str()), COVER_X + COVER_W - 30,
                    COVER_Y + COVER_W + 18 + 15 + 16 + 4, color);
        gui_img_scale(img_cover, 0.5, 0.5);

    }


// Helper function to convert time string to seconds
    int convertTimeToSeconds(const std::string &timePart)
    {
        int minutes = 0, seconds = 0;
        char dummy = ':';
        std::istringstream timeStream(timePart);
        timeStream >> minutes >> dummy >> seconds;
        return (int)(minutes * 60 + seconds);
    }

// Function to get metadata
    std::string getLyricAtTime(const std::string &content, const std::string &type)
    {
        std::istringstream stream(content);
        std::string line;
        std::string prefix = "[" + type + ":";

        while (std::getline(stream, line))
        {
            if (line.find(prefix) == 0)
            {
                size_t endBracket = line.find(']');
                if (endBracket != std::string::npos)
                {
                    return line.substr(prefix.size(), endBracket - prefix.size());
                }
            }
        }

        return ""; // Return empty string if metadata not found
    }

// Function to get lyrics at a specific time
    std::string getLyricAtTime(const std::string &content, int currentTime)
    {
        std::istringstream stream(content);
        std::string line;
        std::string lastLyric = "";

        while (std::getline(stream, line))
        {
            if (line.empty() || line[0] != '[') { continue; }

            size_t endBracket = line.find(']');
            if (endBracket == std::string::npos) { continue; }

            std::string timePart = line.substr(1, endBracket - 1);

            // Skip non-lyrics metadata lines
            if (timePart.find(':') == std::string::npos) { continue; }

            std::string textPart = line.substr(endBracket + 1);
            int lineTime = convertTimeToSeconds(timePart);

            if (lineTime > currentTime)
            {
                return lastLyric;
            }

            lastLyric = textPart;
        }

        return lastLyric;
    }

// Function to get lyrics between a time range
    std::vector<std::string> getLyricAtTime(const std::string &content, int startTime, int endTime)
    {
        std::istringstream stream(content);
        std::string line;
        std::vector<std::string> lyricsInRange;

        while (std::getline(stream, line))
        {
            if (line.empty() || line[0] != '[') { continue; }

            size_t endBracket = line.find(']');
            if (endBracket == std::string::npos) { continue; }

            std::string timePart = line.substr(1, endBracket - 1);

            // Skip non-lyrics metadata lines
            if (timePart.find(':') == std::string::npos) { continue; }

            std::string textPart = line.substr(endBracket + 1);
            int lineTime = convertTimeToSeconds(timePart);

            if (lineTime >= startTime && lineTime <= endTime)
            {
                lyricsInRange.push_back(textPart);
            }
        }

        return lyricsInRange;
    }

// Convert std::vector<std::string> to C-Style Array of Strings
    char **vectorToCStringArray(const std::vector<std::string> &vec)
    {
        char **cArray = new char *[vec.size() + 1]; // One extra for the NULL terminator

        for (size_t i = 0; i < vec.size(); ++i)
        {
            cArray[i] = new char[vec[i].length() + 1];  // Allocate memory for each string
            std::strcpy(cArray[i], vec[i].c_str());
        }

        cArray[vec.size()] = nullptr;  // NULL terminator for the array

        return cArray;
    }

// New function to get lyrics around a specific time and at a range size
    char **getLyricAtTime(const std::string &content, int currentTime, int rangeSize, bool null)
    {
        std::istringstream stream(content);
        std::string line;
        std::vector<std::string> allLines;
        int currentTimeIndex = -1;

        while (std::getline(stream, line))
        {
            if (line.empty() || line[0] != '[')
            {
                allLines.push_back("");
                continue;
            }

            size_t endBracket = line.find(']');
            if (endBracket == std::string::npos)
            {
                allLines.push_back("");
                continue;
            }

            std::string timePart = line.substr(1, endBracket - 1);

            // Skip non-lyrics metadata lines
            if (timePart.find(':') == std::string::npos)
            {
                allLines.push_back("");
                continue;
            }

            std::string textPart = line.substr(endBracket + 1);
            int lineTime = convertTimeToSeconds(timePart);

            allLines.push_back(textPart);
            if (currentTimeIndex == -1 && lineTime >= currentTime)
            {
                currentTimeIndex = allLines.size() - 1;  // Save the index where the current time should be
            }
        }

        if (currentTimeIndex == -1)
        {
            currentTimeIndex = allLines.size() - 1;
        }

        int halfRange = rangeSize / 2;
        std::vector<std::string> rangeLyrics(rangeSize, " ");  // Default fill with empty string

        int startIndex = std::max(0, currentTimeIndex - halfRange);
        int endIndex = std::min(static_cast<int>(allLines.size()), currentTimeIndex + halfRange + 1);
        int rangeStart = std::max(0, halfRange - currentTimeIndex);

        for (int i = startIndex, j = rangeStart; i < endIndex && j < rangeSize; ++i, ++j)
        {
            rangeLyrics[j] = allLines[i];
        }

        // Convert vector to C-style array
        char **cArray = new char *[rangeLyrics.size() + 1]; // One extra for the NULL terminator

        for (size_t i = 0; i < rangeLyrics.size(); ++i)
        {
            cArray[i] = new char[rangeLyrics[i].length() + 1];  // Allocate memory for each string
            std::strcpy(cArray[i], rangeLyrics[i].c_str());
        }

        cArray[rangeLyrics.size()] = nullptr;  // NULL terminator for the array

        return cArray;
    }
// Function to strip time prefix from a line
    std::string stripTimePrefix(const std::string &line)
    {
        size_t endBracket = line.find(']');
        if (endBracket != std::string::npos)
        {
            return line.substr(endBracket + 1);
        }
        return line;
    }
// New function to convert the entire lyrics to C-style array
    char **getLyricAtTime(int &arrayLength, const std::string &content)
    {
        std::istringstream stream(content);
        std::string line;

        // Vector to hold the lines
        std::vector<std::string> lines;

        // Read each line from the content
        while (std::getline(stream, line))
        {
            // Strip the time prefix if present
            lines.push_back(stripTimePrefix(line));
        }

        // Set the array length
        arrayLength = lines.size();

        // Convert vector to C-style array
        char **cArray = new char *[lines.size() + 1]; // One extra for the NULL terminator

        for (size_t i = 0; i < lines.size(); ++i)
        {
            cArray[i] = new char[lines[i].length() + 1];  // Allocate memory for each string
            std::strcpy(cArray[i], lines[i].c_str());
        }

        cArray[lines.size()] = nullptr;  // NULL terminator for the array

        return cArray;
    }
    static std::string formatTime(double seconds)
    {
        int minutes = static_cast<int>(seconds) / 60;
        int sec = static_cast<int>(seconds) % 60;

        std::ostringstream oss;
        oss << minutes << ':' << std::setfill('0') << std::setw(2) << sec;
        return oss.str();
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
