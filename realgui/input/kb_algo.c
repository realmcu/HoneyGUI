
/*
 * File      : kb_algo.c
 */
//#include <string.h>
#include <gui_server.h>
#include <guidef.h>
#include <gui_api.h>
//#include <stdlib.h>
#include <kb_algo.h>


//#define RTK_KB_DEBUG

#ifdef RTK_KB_DEBUG

#define KB_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

#else

#define KB_LOG(format, ...)

#endif

static uint32_t start_tick = 0;
static kb_info_t kb = {.type = KB_INVALID};
static bool long_button_flag = false;

static uint32_t judge_timestamp_overflow(uint32_t timestamp, uint32_t timestamp_sub)
{
    if (timestamp > timestamp_sub)
    {
        return timestamp - timestamp_sub;
    }
    else
    {
        return 0;
    }
}

static bool kb_judge_short_press(struct gui_kb_port_data *kb_raw)
{
    struct gui_indev *indev = gui_get_indev();
    if (judge_timestamp_overflow(kb_raw->timestamp_ms_release,
                                 start_tick) > indev->kb_short_button_time_ms &&
        (kb.pressed == true))
    {
        return true;
    }
    return false;
}

static bool kb_judge_long_press(struct gui_kb_port_data *kb_raw)
{
    struct gui_indev *indev = gui_get_indev();
    if (judge_timestamp_overflow(kb_raw->timestamp_ms_pressing,
                                 start_tick) > indev->kb_long_button_time_ms)
    {
        return true;
    }
    return false;
}

static bool kb_judge_long_pressed(struct gui_kb_port_data *kb_raw)
{
    if (kb_judge_long_press(kb_raw) == true && (long_button_flag == false))
    {
        long_button_flag = true;
        kb.type = KB_LONG;
        KB_LOG("type = KB_LONG \n");
        return true;
    }
    return false;
}

static bool kb_judge_short_click(struct gui_kb_port_data *kb_raw)
{
    if (kb_judge_short_press(kb_raw) == true && (long_button_flag == false))
    {
        kb.type = KB_SHORT;
        KB_LOG("type = KB_SHORT \n");
        return true;
    }
    return false;
}

static void kb_judge_short_click_twice(struct gui_kb_port_data *kb_raw)
{
    static bool hold = 0;
    static bool pressed_flag = 0;
    static uint32_t time_release_his = 0;
    if (kb_raw->event == GUI_KB_EVENT_DOWN && !hold)
    {
        hold = 1;
    }
    if (hold)
    {
        if (kb_raw->event == GUI_KB_EVENT_UP)
        {
            hold = 0;
            uint32_t time_release = kb_raw->timestamp_ms_release;
            uint32_t time_diff = time_release - kb_raw->timestamp_ms_press;
            if (time_diff <= 300)
            {
                // gui_log("pressing time = %d\n", time_diff);
                if (pressed_flag && (time_release - time_release_his) < 1000)
                {
                    // gui_log("short click twice\n");
                    kb.short_click_twice = true;
                }
                pressed_flag = !pressed_flag;
                time_release_his = time_release;
                return;
            }
            else
            {
                // gui_log("pressing time = %d\n", time_diff);
                pressed_flag = 0;
            }
        }
    }
    kb.short_click_twice = false;
}

struct kb_info *kb_algo_process(gui_kb_port_data_t *kb_raw)
{
    GUI_ASSERT(kb_raw != NULL);

    uint8_t flag = kb_raw->event;
    if (flag == GUI_KB_EVENT_DOWN)
    {
        if (kb.pressed == false)
        {
            start_tick = kb_raw->timestamp_ms_press;
            kb.pressed = true;
            long_button_flag = false;
            KB_LOG("=====START DOWN====== tick = %d\n", start_tick);
        }
        if (kb_judge_long_pressed(kb_raw) == true)
        {

        }
        else
        {
            // KB_LOG("not cache kb down \n");
        }
        kb.pressing = true;
        kb.released = false;
    }
    else if (flag == GUI_KB_EVENT_UP)
    {
        if (kb.type != KB_INVALID)
        {
            kb.type = KB_INVALID;
            KB_LOG("=====END UP====== tick = %d\n", kb_raw->timestamp_ms_release);
        }
        else
        {
            if (kb_judge_short_click(kb_raw) == true)
            {

            }
            else
            {
                // KB_LOG("not cache kb up \n");
            }
        }
        long_button_flag = false;
        kb.pressing = false;
        kb.pressed = false;
        kb.released = true;
    }
    else
    {
        //KB_LOG("not cache kb down and up, do keep \n");
    }
    kb_judge_short_click_twice(kb_raw);
    //KB_LOG("kb.type:%d\n", kb.type);
    memset(kb.string, 0x00, sizeof(kb.string));
    memcpy(kb.string, kb_raw->name, sizeof(kb_raw->name));
    return &kb;
}

kb_info_t *kb_get_info(void)
{
    return &kb;
}

