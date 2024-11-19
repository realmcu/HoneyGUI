
/*
 * File      : tp_algo.c
 */

#ifndef RTK_TP_ALGO_V2
//#include <string.h>
#include <gui_server.h>
#include <guidef.h>
#include <gui_api.h>
#include <stdlib.h>
#include <tp_algo.h>


#ifdef ENABLE_MONKEY_TEST
#define RTK_TP_DEBUG
#else
//#define RTK_TP_DEBUG
#endif

#ifdef RTK_TP_DEBUG

#define TP_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

#else

#define TP_LOG(format, ...)

#endif

//static bool new_touch = true;
static uint32_t start_tick;
static struct gui_touch_port_data x_min_record = {0};
static struct gui_touch_port_data y_min_record = {0};
static struct gui_touch_port_data x_max_record = {0};
static struct gui_touch_port_data y_max_record = {0};
static struct gui_touch_port_data first_record = {0};

typedef enum _TOUCH_DIR
{
    TOUCH_NONE,
    TOUCH_HORIZONTAL,
    TOUCH_VERTICAL,
} TOUCH_DIR;
static TOUCH_DIR touch_direct = TOUCH_NONE;

static touch_info_t tp;
//static touch_info_t invalide_tp;
static uint32_t up_cnt = 0;
static uint32_t down_cnt = 0;
static bool long_button_flag = false;


static void tp_do_reset(void)
{
    down_cnt = 0;
}

static uint8_t tp_judge_relese_or_press(struct gui_touch_port_data *raw_data)
{
    uint8_t tp_local_event = 0;
    tp.pressed = false;
    tp.released = false;
    if (raw_data->event == GUI_TOUCH_EVENT_DOWN)
    {
        tp_local_event = GUI_TOUCH_EVENT_DOWN;
        up_cnt = 0;
        down_cnt++;
        if (down_cnt == 1)
        {
            long_button_flag = false;
            tp.pressed = true;
            tp.pressing = true;
            TP_LOG("=====START DOWN====== tick = %d\n", raw_data->timestamp_ms);
        }
    }
    else
    {
        up_cnt++;
        if (down_cnt == 0)
        {
            tp.type = TOUCH_INVALIDE;
        }
#if RTL8762G_86BOX
        if ((up_cnt == 4) && (down_cnt > 0))
#else
        if ((up_cnt == 1) && (down_cnt > 0))
#endif
        {
            tp_local_event = GUI_TOUCH_EVENT_UP;
            down_cnt = 0;
            tp.released = true;
            tp.pressing = false;
            TP_LOG("=====END UP====== tick = %d\n", raw_data->timestamp_ms);
        }
        return tp_local_event;
    }

    if (down_cnt == 1)
    {
        start_tick = raw_data->timestamp_ms;
        x_min_record = *raw_data;
        x_max_record = *raw_data;
        y_min_record = *raw_data;
        y_max_record = *raw_data;
        first_record = *raw_data;
        tp.type = TOUCH_INVALIDE;
        touch_direct = TOUCH_NONE;
    }
    else
    {
        if (raw_data->x_coordinate < x_min_record.x_coordinate)
        {
            x_min_record = *raw_data;
        }
        if (raw_data->y_coordinate < y_min_record.y_coordinate)
        {
            y_min_record = *raw_data;
        }
        if (raw_data->x_coordinate > x_max_record.x_coordinate)
        {
            x_max_record = *raw_data;
        }
        if (raw_data->y_coordinate > y_max_record.y_coordinate)
        {
            y_max_record = *raw_data;
        }
    }
    tp.deltaX = raw_data->x_coordinate - first_record.x_coordinate;
    tp.deltaY = raw_data->y_coordinate - first_record.y_coordinate;
    tp.x = first_record.x_coordinate;
    tp.y = first_record.y_coordinate;
    return tp_local_event;
}

static bool tp_judge_same_point(void)
{
    if (
        ((x_max_record.x_coordinate - x_min_record.x_coordinate) >= SAME_POINT_THR) || \
        ((y_max_record.y_coordinate - y_min_record.y_coordinate) >= SAME_POINT_THR)
    )
    {
        TP_LOG("tp_judge_same_point delta_x: %d , delta_y: %d , return FALSE!!",
               x_max_record.x_coordinate - x_min_record.x_coordinate,
               y_max_record.y_coordinate - y_min_record.y_coordinate);
        return false;
    }
    else
    {
        TP_LOG("tp_judge_same_point delta_x: %d , delta_y: %d , return TRUE!!",
               x_max_record.x_coordinate - x_min_record.x_coordinate,
               y_max_record.y_coordinate - y_min_record.y_coordinate);
        return true;
    }
}

static bool tp_judge_short_press(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) > indev->short_button_time_ms)
    {
        return false;
    }
    return true;
}

static bool tp_judge_long_press(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) > indev->long_button_time_ms)
    {
        return true;
    }
    return false;
}

static bool tp_judge_quick_x_left_slide(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) < indev->quick_slide_time_ms)
    {
        if (abs(tp.deltaX) >= abs(tp.deltaY))
        {
            if (tp.deltaX < 0)
            {
                tp.type = TOUCH_LEFT_SLIDE;
                TP_LOG("type = TOUCH_LEFT_SLIDE, %d \n", __LINE__);
                return true;
            }
        }
    }
    return false;
}

static bool tp_judge_quick_x_right_slide(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) < indev->quick_slide_time_ms)
    {
        if (abs(tp.deltaX) >= abs(tp.deltaY))
        {
            if (tp.deltaX > 0)
            {
                tp.type = TOUCH_RIGHT_SLIDE;
                TP_LOG("type = TOUCH_RIGHT_SLIDE, %d \n", __LINE__);
                return true;
            }
        }
    }
    return false;
}

static bool tp_judge_quick_y_down_slide(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) < indev->quick_slide_time_ms)
    {
        if (abs(tp.deltaX) < abs(tp.deltaY))
        {
            if (tp.deltaY > 0)
            {
                tp.type = TOUCH_DOWN_SLIDE;
                TP_LOG("type = TOUCH_DOWN_SLIDE\n");
                return true;
            }
        }
    }
    return false;
}

static bool tp_judge_quick_y_up_slide(struct gui_touch_port_data *raw_data)
{
    struct gui_indev *indev = gui_get_indev();
    if ((raw_data->timestamp_ms - start_tick) < indev->quick_slide_time_ms)
    {
        if (abs(tp.deltaX) < abs(tp.deltaY))
        {
            if (tp.deltaY < 0)
            {
                tp.type = TOUCH_UP_SLIDE;
                TP_LOG("type = TOUCH_UP_SLIDE\n");
                return true;
            }
        }
    }
    return false;
}

static bool tp_judge_slow_x_left_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_X)
    {
        if (abs(tp.deltaX) > gui_get_screen_width() / 2)
        {
            if (tp.deltaX < 0)
            {
                tp.type = TOUCH_LEFT_SLIDE;
                TP_LOG("type = TOUCH_LEFT_SLIDE, %d \n", __LINE__);
                return true;
            }
        }
    }
    return false;
}
static bool tp_judge_slow_x_right_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_X)
    {
        if (abs(tp.deltaX) > gui_get_screen_width() / 2)
        {
            if (tp.deltaX > 0)
            {
                tp.type = TOUCH_RIGHT_SLIDE;
                TP_LOG("type = TOUCH_RIGHT_SLIDE, %d \n", __LINE__);
                return true;
            }
        }
    }
    return false;
}
static bool tp_judge_slow_x_orign_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_X)
    {
        if (abs(tp.deltaX) <= gui_get_screen_width() / 2)
        {
            tp.type = TOUCH_ORIGIN_FROM_X;
            TP_LOG("type = TOUCH_ORIGIN_FROM_X\n");
            return true;
        }
    }
    return false;
}
static bool tp_judge_slow_y_down_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_Y)
    {
        if (abs(tp.deltaY) > gui_get_screen_height() / 2)
        {
            if (tp.deltaY > 0)
            {
                tp.type = TOUCH_DOWN_SLIDE;
                TP_LOG("type = TOUCH_DOWN_SLIDE\n");
                return true;
            }
        }
    }
    return false;
}
static bool tp_judge_slow_y_up_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_Y)
    {
        if (abs(tp.deltaY) > gui_get_screen_height() / 2)
        {
            if (tp.deltaY < 0)
            {
                tp.type = TOUCH_UP_SLIDE;
                TP_LOG("type = TOUCH_UP_SLIDE\n");
                return true;
            }
        }
    }
    return false;
}
static bool tp_judge_slow_y_orign_slide(struct gui_touch_port_data *raw_data)
{
    if (tp.type == TOUCH_HOLD_Y)
    {
        if (abs(tp.deltaY) <= gui_get_screen_height() / 2)
        {
            tp.type = TOUCH_ORIGIN_FROM_Y;
            TP_LOG("type = TOUCH_ORIGIN_FROM_Y\n");
            return true;
        }
    }
    return false;
}

static bool tp_judge_hold_x(struct gui_touch_port_data *raw_data)
{
    if (tp_judge_same_point() == false)
    {
        if (touch_direct == TOUCH_NONE)
        {
            if (abs(tp.deltaX) >= abs(tp.deltaY))
            {
                touch_direct = TOUCH_HORIZONTAL;
                tp.type = TOUCH_HOLD_X;
                TP_LOG("type = TOUCH_HOLD_X \n");
                return true;
            }
        }
        else if (touch_direct == TOUCH_HORIZONTAL)
        {
            tp.type = TOUCH_HOLD_X;
            TP_LOG("type = TOUCH_HOLD_X \n");
            return true;
        }
    }
    return false;
}
static bool tp_judge_hold_y(struct gui_touch_port_data *raw_data)
{
    if (tp_judge_same_point() == false)
    {
        if (touch_direct == TOUCH_NONE)
        {
            if (abs(tp.deltaX) < abs(tp.deltaY))
            {
                touch_direct = TOUCH_VERTICAL;
                tp.type = TOUCH_HOLD_Y;
                TP_LOG("type = TOUCH_HOLD_Y \n");
                return true;
            }
        }
        else if (touch_direct == TOUCH_VERTICAL)
        {
            tp.type = TOUCH_HOLD_Y;
            TP_LOG("type = TOUCH_HOLD_Y \n");
            return true;
        }
    }
    return false;
}
static bool tp_judge_long_pressed(struct gui_touch_port_data *raw_data)
{
    if ((tp_judge_same_point() == true) && (tp_judge_long_press(raw_data) == true) &&
        (long_button_flag == false))
    {
        long_button_flag = true;
        tp.type = TOUCH_LONG;
        TP_LOG("type = TOUCH_LONG \n");
        return true;
    }
    return false;
}

static bool tp_judge_short_click(struct gui_touch_port_data *raw_data)
{
    if ((tp_judge_same_point() == true) && (tp_judge_short_press(raw_data) == true))
    {
        tp.type = TOUCH_SHORT;
        TP_LOG("type = TOUCH_SHORT \n");
        return true;
    }
    return false;
}



struct touch_info *tp_algo_process(struct gui_touch_port_data *raw_data)
{

    GUI_ASSERT(raw_data != NULL);
    uint8_t flag = tp_judge_relese_or_press(raw_data);


    if (flag == GUI_TOUCH_EVENT_DOWN)
    {
        if (tp_judge_hold_x(raw_data) == true)
        {

        }
        else if (tp_judge_hold_y(raw_data) == true)
        {

        }
        else if (tp_judge_long_pressed(raw_data) == true)
        {

        }
        else
        {
            TP_LOG("not cache tp down \n");
        }
    }
    else if (flag == GUI_TOUCH_EVENT_UP)
    {
        if (tp_judge_short_click(raw_data) == true)
        {

        }
        else if (tp_judge_quick_x_left_slide(raw_data) == true)
        {

        }
        else if (tp_judge_quick_x_right_slide(raw_data) == true)
        {

        }
        else if (tp_judge_quick_y_down_slide(raw_data) == true)
        {

        }
        else if (tp_judge_quick_y_up_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_x_left_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_x_right_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_x_orign_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_y_down_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_y_up_slide(raw_data) == true)
        {

        }
        else if (tp_judge_slow_y_orign_slide(raw_data) == true)
        {

        }
        else
        {
            TP_LOG("not cache tp up \n");
        }
        tp_do_reset();
    }
    else
    {
        //TP_LOG("not cache tp down and up, do keep \n");
    }
    //gui_log("tp.type:%d\n",tp.type);
    return &tp;

}

touch_info_t *tp_get_info(void)
{
    return &tp;
}
#endif
