
/*
 * File      : gui_kb.c
 */
//#include <string.h>
#include <gui_server.h>
#include <guidef.h>
#include <gui_api.h>
//#include <stdlib.h>
#include <gui_kb.h>


//#define RTK_KB_DEBUG

#ifdef RTK_KB_DEBUG

#define KB_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

#else

#define KB_LOG(format, ...)

#endif


static uint32_t start_tick;
static kb_info_t kb = {0};
static uint32_t up_cnt = 0;
static uint32_t down_cnt = 0;
static bool long_button_flag = false;


static void kb_do_reset(void)
{
    down_cnt = 0;
}

static uint8_t kb_judge_relese_or_press(struct gui_kb_port_data *kb_raw)
{
    uint8_t kb_local_event = 0;
    kb.pressed = false;
    kb.released = false;
    if (kb_raw->event == GUI_KB_EVENT_DOWN)
    {
        kb_local_event = GUI_KB_EVENT_DOWN;
        up_cnt = 0;
        down_cnt++;
        if (down_cnt == 1)
        {
            long_button_flag = false;
            kb.pressed = true;
            KB_LOG("=====START DOWN====== tick = %d\n", kb_raw->timestamp_ms);
        }
    }
    else
    {
        up_cnt++;
        if (down_cnt == 0)
        {
            kb.type = KB_INVALIDE;
        }
        if ((up_cnt == 4) && (down_cnt > 0))
        {
            kb_local_event = GUI_KB_EVENT_UP;
            down_cnt = 0;
            kb.released = true;
            KB_LOG("=====END UP====== tick = %d\n", kb_raw->timestamp_ms);
        }
    }

    if (down_cnt == 1)
    {
        start_tick = kb_raw->timestamp_ms;
        kb.type = KB_INVALIDE;
    }

    return kb_local_event;
}

static bool kb_judge_short_press(struct gui_kb_port_data *kb_raw)
{
    struct gui_indev *indev = gui_get_indev();
    if ((kb_raw->timestamp_ms - start_tick) > indev->kb_short_button_time_ms)
    {
        return true;
    }
    return false;
}

static bool kb_judge_long_press(struct gui_kb_port_data *kb_raw)
{
    struct gui_indev *indev = gui_get_indev();
    if ((kb_raw->timestamp_ms - start_tick) > indev->kb_long_button_time_ms)
    {
        return true;
    }
    return false;
}

static bool kb_judge_long_pressed(struct gui_kb_port_data *kb_raw)
{
    if ((kb_judge_long_press(kb_raw) == true) && (long_button_flag == false))
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
    if (kb_judge_short_press(kb_raw) == true)
    {
        kb.type = KB_SHORT;
        KB_LOG("type = KB_SHORT \n");
        return true;
    }
    return false;
}

struct kb_info *kb_algo_process(gui_kb_port_data_t *kb_raw)
{
    GUI_ASSERT(kb_raw != NULL);
    uint8_t flag = kb_judge_relese_or_press(kb_raw);

    if (flag == GUI_KB_EVENT_DOWN)
    {
        if (kb_judge_long_pressed(kb_raw) == true)
        {

        }
        else
        {
            // KB_LOG("not cache kb down \n");
        }
    }
    else if (flag == GUI_KB_EVENT_UP)
    {
        if (kb_judge_short_click(kb_raw) == true)
        {

        }
        else
        {
            // KB_LOG("not cache kb up \n");
        }
        kb_do_reset();
    }
    else
    {
        //KB_LOG("not cache kb down and up, do keep \n");
    }
    //KB_LOG("kb.type:%d\n", kb.type);
    return &kb;
}

kb_info_t *kb_get_info(void)
{
    return &kb;
}

