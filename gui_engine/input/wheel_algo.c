
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


//#define RTK_WHEEL_DEBUG

#ifdef RTK_WHEEL_DEBUG

#define WHEEL_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

#else

#define WHEEL_LOG(format, ...)

#endif


static wheel_info_t wheel =
{
    .delta = 0,
    .type = WHEEL_INIT,
};
static uint32_t reset_ms = 0;


static bool wheel_judge_ing(gui_wheel_port_data_t *wheel_raw)
{
    if (wheel_raw->delta != 0)
    {
        return true;
    }
    return false;
}

wheel_info_t *wheel_algo_process(gui_wheel_port_data_t *wheel_raw)
{
    GUI_ASSERT(wheel_raw != NULL);

    if (wheel_judge_ing(wheel_raw) == true)
    {
        wheel.type = WHEEL_ING;
        wheel.delta += wheel_raw->delta;
        wheel_raw->delta = 0;
        //do reset
        reset_ms = wheel_raw->timestamp_ms;
    }

    if (((wheel_raw->timestamp_ms - reset_ms) > 300) && (wheel.type == WHEEL_ING))
    {
        wheel.type = WHEEL_FINISHED;
        WHEEL_LOG("Wheel finished wheel.delta = %d!! \n", wheel.delta);
    }
    return &wheel;
}

wheel_info_t *wheel_get_info(void)
{
    return &wheel;
}

