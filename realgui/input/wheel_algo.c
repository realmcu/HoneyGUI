
/**
 * @file wheel_algo.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-02-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <gui_server.h>
#include <guidef.h>
#include <gui_api.h>
#include <wheel_algo.h>


#define STEP 40
#define RELEASE_THRESHOLD_MS 300
static touch_info_t wheel = {.type = TOUCH_INVALID, .x = 454 / 2, .y = 454 / 2};
touch_info_t *wheel_algo_process(gui_wheel_port_data_t *wheel_raw)
{
    GUI_ASSERT(wheel_raw != NULL);


    static uint32_t timestamp;
    wheel.button_down = 0;
    wheel.button_up = 0;
    wheel.released = 0;
    wheel.pressed = 0;

    if (wheel.type == TOUCH_DOWN_SLIDE || wheel.type == TOUCH_UP_SLIDE)
    {
        wheel.type = TOUCH_INVALID;
    }

    switch (wheel_raw->event)
    {
    case GUI_WHEEL_NO_EVENT:
        if (gui_ms_get() - timestamp > RELEASE_THRESHOLD_MS / 2)
        {
            if (wheel.pressing)
            {
                wheel.released = 1;
                wheel.pressing = 0;

                if (wheel.deltaY > 0)
                {
                    wheel.type = TOUCH_DOWN_SLIDE;
                }
                else
                {
                    wheel.type = TOUCH_UP_SLIDE;
                }

                wheel.deltaY = 0;
            }

        }

        break;
    case GUI_WHEEL_BUTTON_DOWN:
        {
            wheel.button_down = 1;
            wheel.button_hold = 1;
        }
        break;
    case GUI_WHEEL_BUTTON_UP:
        {
            wheel.button_down = 0;
            wheel.button_hold = 0;
            wheel.button_up  = 1;
        }
        break;
    case GUI_WHEEL_SCROLL:
        {

            if (wheel.type == TOUCH_INVALID)
            {
                wheel.pressed = 1;
                wheel.pressing = 1;
            }
            if (wheel.pressing)
            {
                wheel.deltaY = wheel.deltaY + wheel_raw->delta * STEP;
                wheel.type = TOUCH_HOLD_Y;
            }


            wheel.history_y = wheel.history_y + wheel_raw->delta * STEP;
            timestamp = wheel_raw->timestamp_ms;
        }
        break;
    default:
        break;
    }
    wheel_raw->event = GUI_WHEEL_NO_EVENT;
    return &wheel;
}

touch_info_t *wheel_get_info(void)
{
    return &wheel;
}
