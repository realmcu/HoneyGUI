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

/*============================================================================*
*                        Header Files
*============================================================================*/
#include "gui_view.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "guidef.h"
#include "root_image/ui_resource.h"
#include "gui_canvas_rect.h"
#include "gui_img.h"
#include "gui_win.h"
#include "tp_algo.h"
#include "gui_text.h"
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip> // for std::setw and std::setfill

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "music_view"
#define SCREEN_W (int16_t)gui_get_screen_width()
#define SCREEN_H (int16_t)gui_get_screen_height()
#define COLOR_ARTISTNAME  gui_rgb(230, 230, 230)
#define COLOR_LIST gui_rgb(100, 100, 100)
#define COLOR_THEME gui_color_css("#892FE0")

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif
/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C" {
    static gui_view_t *current_view = NULL;
    const static gui_view_descriptor_t *menu_view = NULL;
    static void app_music_ui_design(gui_view_t *view);
    static void clear_music(gui_view_t *view);

    static gui_view_descriptor_t const descriptor =
    {
        /* change Here for current view */
        .name = (const char *)CURRENT_VIEW_NAME,
        .pView = &current_view,
        .on_switch_in = app_music_ui_design,
        .on_switch_out = clear_music,
    };

    static int gui_view_descriptor_register_init(void)
    {
        gui_view_descriptor_register(&descriptor);
        gui_log("File: %s, Function: %s\n", __FILE__, __func__);
        return 0;
    }
    static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

    static int gui_view_get_other_view_descriptor_init(void)
    {
        /* you can get other view descriptor point here */
        menu_view = gui_view_descriptor_get("menu_view");
        gui_log("File: %s, Function: %s\n", __FILE__, __func__);
        return 0;
    }
    static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);
}

/*============================================================================*
 *                             C++ Namespace
 *============================================================================*/
extern "C" {
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
#if __WIN32
        extern int win32_play_music(const char *music_file);
        win32_play_music(music_file);
#endif
        return GUI_SUCCESS;
    }
    gui_error_t gui_music_stop()
    {
#if __WIN32
        extern int win32_stop_music(void);
        win32_stop_music();
#endif
        return GUI_SUCCESS;
    }
    bool gui_music_completion_status()
    {
#if __WIN32
        extern bool win32_music_completion_status(void);
        return win32_music_completion_status();
#endif
        return 0;
    }
    float gui_music_length()
    {
        float length = 0;
#if __WIN32
        extern double win32_music_get_music_length();
        length = (float)win32_music_get_music_length();
#endif
        return length;
    }
    float gui_music_current_time()
    {
        float time = 0;
#if __WIN32
        extern  double win32_music_get_music_current_time();
        time = (float)win32_music_get_music_current_time();
#endif
        return time;
    }
    gui_error_t gui_music_load(const char *music_file)
    {
        gui_error_t e = GUI_ERROR;
#if __WIN32
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

/*============================================================================*
 *                             C++ Namespace
 *============================================================================*/
namespace gui_music_app
{
/*============================================================================*
*                              Variables
*============================================================================*/
constexpr int SWITCH_W = 30;
constexpr int SWITCH_H = 30;
constexpr int BUTTON_Y = 440;
constexpr int COVER_Y = 50;
constexpr int COVER_W = 300;
constexpr int MINIMIZE_WINDOW_HEIGHT = 60;
constexpr int MINIMIZE_COVER_HEIGHT = 50;
constexpr int LIST_COVER_H = 120;
constexpr int LIST_SONG_H = 64;
constexpr char MP3_FILE[] =
    "C:\\wokspace\\GitLoadHouse\\HoneyComb\\sdk\\src\\sample\\gui\\example\\application\\screen_410_502\\root_image\\source\\music\\Free_Test_Data_5MB_MP3.mp3";
gui_text_t *current_time = nullptr;
gui_canvas_rect_t *current_time_bar = nullptr;
gui_list_t *lrc_list = nullptr;
gui_img_t *play_button = nullptr;
int page_list_item_space = 0;
int lyrics_array_length = 0;
char **all_lyrics_array = nullptr;

/*============================================================================*
*                            Functions
*============================================================================*/
bool starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), str.begin());
}
class MUSIC_PLAYER
{
public:
    MUSIC_PLAYER(gui_obj_t *parent)
        : SWITCH_X(SCREEN_W / 2 - SWITCH_W / 2),
          COVER_X(SCREEN_W / 2 - COVER_W / 2),
          playing_flag()
    {
        setup_ui(parent);
    }

    void win_cover_transform(void)
    {
        if (win_cover_y >= 0)
        {
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
            gui_set_location(GUI_BASE(play_button), sw_x, sw_y);
            // gui_log("y:%d\n", win_cover_y);
        }
        if (win_cover_y <= 0)
        {
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
            gui_set_location(GUI_BASE(play_button), sw_x, sw_y);
            gui_set_location(GUI_BASE(skip_back), GUI_BASE(skip_back)->x, sw_y);
            gui_set_location(GUI_BASE(skip_fwd), GUI_BASE(skip_fwd)->x, sw_y);
            // gui_log("y:%d,%d,%d\n", win_cover_y, sw_x, sw_y);
        }
        if (win_cover_y > 0)
        {
            gui_obj_hidden(GUI_BASE(music_list_win), false);
            gui_obj_hidden(GUI_BASE(lrc_win), true);
        }
        else if (win_cover_y < 0)
        {
            gui_obj_hidden(GUI_BASE(music_list_win), true);
            gui_obj_hidden(GUI_BASE(lrc_win), false);
        }
        else
        {
            gui_obj_hidden(GUI_BASE(music_list_win), true);
            gui_obj_hidden(GUI_BASE(lrc_win), true);
        }
    }

    void win_cover_animation(void *p)
    {
        touch_info_t *tp = tp_get_info();
        gui_obj_t *obj = GUI_BASE(p);
        int16_t screen_h = SCREEN_H;
        if (tp->pressing != 0 || win_cover_y == MINIMIZE_WINDOW_HEIGHT - screen_h
            || win_cover_y == screen_h - MINIMIZE_WINDOW_HEIGHT || win_cover_y == 0) { return; }
        gui_win_compensate((gui_win_t *)obj, 0, 0);

        if (win_cover_y < MINIMIZE_WINDOW_HEIGHT - screen_h + speed)
        {
            win_cover_y = MINIMIZE_WINDOW_HEIGHT - screen_h;
        }
        else if (win_cover_y < -screen_h / 2)
        {
            win_cover_y -= speed;
        }
        else if (win_cover_y < -speed)
        {
            win_cover_y += speed;
        }
        else if (win_cover_y < speed)
        {
            win_cover_y = 0;
        }
        else if (win_cover_y < screen_h / 2)
        {
            win_cover_y -= speed;
        }
        else if (win_cover_y < screen_h - MINIMIZE_WINDOW_HEIGHT - speed)
        {
            win_cover_y += speed;
        }
        else
        {
            win_cover_y = screen_h - MINIMIZE_WINDOW_HEIGHT;
        }
        win_cover_y_history = win_cover_y;
        obj->y = win_cover_y;
        win_cover_transform();
    }

    void music_win_cover_scroll_vertical_cb(void *o)
    {
        gui_obj_t *obj = GUI_TYPE(gui_obj_t, o);
        touch_info_t *tp = tp_get_info();
        win_cover_y = tp->deltaY + win_cover_y_history;
        obj->y = win_cover_y;
        gui_win_compensate((gui_win_t *)obj, 0, win_cover_y);
        win_cover_transform();
    }
private:
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
    gui_img_t *skip_back;
    gui_img_t *skip_fwd;
    gui_img_t *img_cover;
    gui_win_t *win_cover;
    std::string artist_name;
    std::string music_name;
    std::string album_name;
    std::string music_time;
    int win_cover_y = 0;
    int win_cover_y_history = 0;
    int release_win_cover_y = 0;;
    int speed = 30;
    int offset = 0;
    gui_win_t *music_list_win;
    gui_win_t *lrc_win;
    bool playing_flag;
    int value = 0;
// Example LRC content
    std::string LRC_CONTENT = R"(
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
    static void on_playing(void *p)
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
        int16_t offset = -(time / duration * (page_list_item_space * lyrics_array_length -
                                              SCREEN_H / 2));
        gui_list_set_offset(lrc_list, offset);
        if (gui_music_completion_status())
        {
            gui_img_t *img = GUI_TYPE(gui_img_t, p);
            img->data = PLAY_BIN;
        }
    }
    static void music_clicked_timer_cb(void *p)
    {
        gui_obj_hidden(GUI_BASE(p), true);
    }
    static void music_clicked(void *obj, gui_event_t e, void *param)
    {
        gui_obj_hidden(GUI_BASE(param), false);
        gui_obj_create_timer(GUI_BASE(param), 20, false, music_clicked_timer_cb);
        gui_obj_start_timer(GUI_BASE(param));

        if (play_button->data == PLAY_BIN) //play this music
        {
            gui_music_play(MP3_FILE);
            play_button->data = PAUSE_BIN;
            gui_img_set_mode(play_button, IMG_SRC_OVER_MODE);
            gui_obj_start_timer(GUI_BASE(play_button));
        }
    }
    gui_text_t *dispaly_text(gui_obj_t *parent, const char *text_string, int x, int y,
                             gui_color_t color)
    {
        // This function is to create text GUI element
        const char *text = text_string;
        int font_size = 24;
        gui_text_t *t = gui_text_create(parent, 0,  x, y,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, color, strlen(text), font_size);
        void *addr1 = SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
        return t;
    }
    static void music_list_design(gui_obj_t *obj, void *p)
    {
        gui_list_note_t *note = GUI_TYPE(gui_list_note_t, obj);
        uint8_t index = note->index;
        void *img_file[] =
        {
            GROUP1000000744_BIN,
            GROUP1000000745_BIN,
            GROUP1000000746_BIN,
            GROUP1000000747_BIN,
            GROUP1000000748_BIN,
        };
        gui_canvas_rect_t *rect = gui_canvas_rect_create(GUI_BASE(note), 0, 0, 0, SCREEN_W, LIST_SONG_H,
                                                         COLOR_THEME);
        gui_obj_hidden(GUI_BASE(rect), true);
        gui_img_t *img = gui_img_create_from_mem(obj, 0, img_file[index % 5], 30, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, music_clicked, GUI_EVENT_TOUCH_CLICKED, rect);
    }
    static void lrc_list_design(gui_obj_t *obj, void *p)
    {
        gui_list_note_t *note = GUI_TYPE(gui_list_note_t, obj);
        uint8_t index = note->index;
        char **all_lyrics_array = (char **)p;
        const char *text = all_lyrics_array[index];
        gui_text_t *t = gui_text_create(obj, 0, 0, 35, 0, 0);
        gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_RED, strlen(text), 24);
        gui_text_type_set(t, (void *)SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }

    static void play_button_cb(void *obj, gui_event_t e, void *param)
    {
        gui_img_t *img = GUI_TYPE(gui_img_t, obj);
        if (img->data == PLAY_BIN)
        {
            gui_music_play(MP3_FILE);
            img->data = PAUSE_BIN;
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_obj_start_timer(GUI_BASE(obj));
        }
        else
        {
            gui_music_stop();
            img->data = PLAY_BIN;
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_obj_stop_timer(GUI_BASE(obj));
        }
    }
    static void skip_back_timer_cb(void *p)
    {
        gui_img_t *img = GUI_TYPE(gui_img_t, p);
        gui_img_set_image_data(img, (const uint8_t *)SKIPBACK_BIN);
    }
    static void skip_back_button_cb(void *obj, gui_event_t e, void *param)
    {
        gui_img_t *img = GUI_TYPE(gui_img_t, obj);
        gui_img_set_image_data(img, (const uint8_t *)SKIPBACKHL_BIN);
        gui_obj_create_timer(GUI_BASE(img), 10, false, skip_back_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));
    }

    static void skip_fwd_timer_cb(void *p)
    {
        gui_img_t *img = GUI_TYPE(gui_img_t, p);
        gui_img_set_image_data(img, (const uint8_t *)SKIPFWD_BIN);
    }
    static void skip_fwd_button_cb(void *obj, gui_event_t e, void *param)
    {
        gui_img_t *img = GUI_TYPE(gui_img_t, obj);
        gui_img_set_image_data(img, (const uint8_t *)SKIPFWDHL_BIN);
        gui_obj_create_timer(GUI_BASE(img), 10, false, skip_fwd_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));
    }
    static void win_clicked_cb(void *obj, gui_event_t e, void *param)
    {
        // stop music list clicked event
    }

    static void win_cover_scroll_vertical_cb(void *p);
    static void cover_animation(void *p);
    void setup_ui(gui_obj_t *parent)
    {
        /*LIST WINDOW*/
        music_list_win = gui_win_create(parent, "win list", 0, 0, SCREEN_W, SCREEN_H);
        gui_canvas_rect_create(GUI_BASE(music_list_win), 0, 0, 0, SCREEN_W, SCREEN_H, COLOR_LIST);
        gui_list_t *music_list = gui_list_create(music_list_win, "music_list", 0, 0, SCREEN_W, SCREEN_H,
                                                 LIST_SONG_H, 0, VERTICAL, music_list_design, NULL, false);
        gui_list_set_note_num(music_list, 20);
        gui_list_set_style(music_list, LIST_CLASSIC);
        gui_obj_hidden(GUI_BASE(music_list_win), true);

        /*LYRIC WINDOW*/
        lrc_win = gui_win_create(parent, "LYRIC", 0, 0, SCREEN_W, SCREEN_H);
        gui_canvas_rect_create(GUI_BASE(lrc_win), 0, 0, 0, SCREEN_W, SCREEN_H, gui_color_css("GhostWhite"));
        int arrayLength = 0;
        all_lyrics_array = get_lyric_at_time(arrayLength,
                                             LRC_CONTENT); // If use other overload functions, notice mem management
        lyrics_array_length = arrayLength;
        page_list_item_space = 40;
        // for (int i = 0; all_lyrics_array[i] != nullptr; ++i)
        // {
        //     gui_log("%s\n", all_lyrics_array[i]);
        // }
        lrc_list = gui_list_create(lrc_win, "lrc_list", 0, 0, 0, 0, 30, 5, VERTICAL, lrc_list_design,
                                   all_lyrics_array, false);
        gui_list_set_note_num(lrc_list, lyrics_array_length);
        gui_list_set_style(lrc_list, LIST_CLASSIC);
        gui_obj_hidden(GUI_BASE(lrc_win), true);
        /*COVER WINDOW*/
        win_cover = gui_win_create(parent, "win cover", 0, 0, SCREEN_W, SCREEN_H);
        gui_obj_add_event_cb(GUI_BASE(win_cover), (gui_event_cb_t)win_cover_scroll_vertical_cb,
                             GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
        gui_obj_create_timer(GUI_BASE(win_cover), 10, true, cover_animation);
        gui_obj_add_event_cb(GUI_BASE(win_cover), (gui_event_cb_t)win_clicked_cb, GUI_EVENT_TOUCH_CLICKED,
                             NULL);

        parent = GUI_BASE(win_cover);
        gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, APP_COLOR_SILVER);
        img_cover = gui_img_create_from_mem(parent, 0, RECTANGLE86_BIN, COVER_X, COVER_Y, 0, 0);
        play_button = gui_img_create_from_mem(parent, 0, PLAY_BIN, MINIMIZE_SWITCH_TARGT_X, BUTTON_Y, 0, 0);
        gui_obj_add_event_cb(play_button, play_button_cb, GUI_EVENT_TOUCH_PRESSED, NULL);
        gui_obj_create_timer(GUI_BASE(play_button), 50, true, on_playing);
        gui_obj_stop_timer(GUI_BASE(play_button));
        skip_back = gui_img_create_from_mem(parent, 0, SKIPBACK_BIN, SWITCH_X - SCREEN_W / 3, BUTTON_Y, 0,
                                            0);
        gui_obj_add_event_cb(skip_back, skip_back_button_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        skip_fwd = gui_img_create_from_mem(parent, 0, SKIPFWD_BIN, SWITCH_X + SCREEN_W / 3, BUTTON_Y, 0, 0);
        gui_obj_add_event_cb(skip_fwd, skip_fwd_button_cb, GUI_EVENT_TOUCH_PRESSED, NULL);

        // Display artist, music, and album name
        gui_music_load(MP3_FILE);
        float length = gui_music_length();
        artist_name = "FreeTestData.com";
        album_name = "Sample Audio";
        music_name = "Free_Test_Data_5MB_MP3";
        music_time = formatTime(length);
        dispaly_text(parent, music_name.c_str(), COVER_X, COVER_Y + COVER_W, APP_COLOR_WHITE);
        dispaly_text(parent, artist_name.c_str(), COVER_X, COVER_Y + COVER_W + 18, COLOR_ARTISTNAME);
        dispaly_text(parent, album_name.c_str(), COVER_X + 180, COVER_Y + COVER_W + 18, COLOR_ARTISTNAME);
        gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16, COVER_W, 3,
                               COLOR_ARTISTNAME);
        current_time_bar = gui_canvas_rect_create(parent, 0, COVER_X, COVER_Y + COVER_W + 18 + 15 + 16,
                                                  1, 3, APP_COLOR_WHITE);
        current_time = dispaly_text(parent, "0:00", COVER_X, COVER_Y + COVER_W + 18 + 15 + 16 + 4,
                                    APP_COLOR_WHITE);
        dispaly_text(parent, music_time.c_str(), COVER_X + COVER_W - 30,
                     COVER_Y + COVER_W + 18 + 15 + 16 + 4, APP_COLOR_WHITE);
    }


// Helper function to convert time string to seconds
    int convert_time_to_seconds(const std::string &timePart)
    {
        int minutes = 0, seconds = 0;
        char dummy = ':';
        std::istringstream timeStream(timePart);
        timeStream >> minutes >> dummy >> seconds;
        int rst = 0;
        if (minutes * 60 + seconds > INT_MAX)
        {
            rst = INT_MAX;
        }
        else
        {
            rst = minutes * 60 + seconds;
        }
        return rst;
    }

// Function to get metadata
    std::string get_lyric_at_time(const std::string &content, const std::string &type)
    {
        std::istringstream stream(content);
        std::string line;
        std::string prefix = "[" + type + ":";

        while (std::getline(stream, line))
        {
            if (starts_with(line, prefix)) //if (line.find(prefix) == 0)
            {
                size_t endBracket = starts_with(line, "]");//line.find(']');
                if (endBracket != std::string::npos)
                {
                    return line.substr(prefix.size(), endBracket - prefix.size());
                }
            }
        }

        return ""; // Return empty string if metadata not found
    }

// Function to get lyrics at a specific time
    std::string get_lyric_at_time(const std::string &content, int currentTime)
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
            int lineTime = convert_time_to_seconds(timePart);

            if (lineTime > currentTime)
            {
                return lastLyric;
            }

            lastLyric = textPart;
        }

        return lastLyric;
    }

// Function to get lyrics between a time range
    std::vector<std::string> get_lyric_at_time(const std::string &content, int startTime, int endTime)
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
            int lineTime = convert_time_to_seconds(timePart);

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
    char **get_lyric_at_time(const std::string &content, int currentTime, int rangeSize, bool null)
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
            int lineTime = convert_time_to_seconds(timePart);

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
    char **get_lyric_at_time(int &arrayLength, const std::string &content)
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

MUSIC_PLAYER *player = nullptr;
void MUSIC_PLAYER::cover_animation(void *p)
{
    player->win_cover_animation(p);
}

void MUSIC_PLAYER::win_cover_scroll_vertical_cb(void *o)
{
    player->music_win_cover_scroll_vertical_cb(o);
}

void music_design(gui_obj_t *parent)
{
    player = new MUSIC_PLAYER(parent);
}

void free_music_app(void)
{
    if (player)
    {
        delete player;
        player = nullptr;
    }
    uint16_t i = 0;
    while (1)
    {
        if (all_lyrics_array[i])
        {
            delete[] all_lyrics_array[i];
            all_lyrics_array[i] = nullptr;
            i++;
        }
        else
        {
            delete all_lyrics_array;
            all_lyrics_array = nullptr;
            break;
        }
    }
}
}

/*============================================================================*
 *                                  C Interface
 *============================================================================*/
extern "C"
{
    static void return_cb()
    {
        gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }

    static void clear_music(gui_view_t *view)
    {
        gui_music_app::free_music_app();
    }

    static void app_music_ui_design(gui_view_t *view)
    {
        gui_obj_t *parent = GUI_BASE(view);
        gui_log("app_music_ui_design\n");
        gui_music_app::music_design(parent);


        gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
}