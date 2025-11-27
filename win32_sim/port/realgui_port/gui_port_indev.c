
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <SDL.h>
#include <pthread.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "gui_message.h"


gui_touch_port_data_t tp_port_data = {0};

gui_kb_port_data_t kb_port_data = {0};

gui_wheel_port_data_t wheel_port_data = {0};


gui_touch_port_data_t *port_touchpad_get_data(void)
{
    tp_port_data.timestamp_ms = gui_ms_get();//todo
    return &tp_port_data;
}

gui_kb_port_data_t *port_kb_get_data(void)
{
    kb_port_data.timestamp_ms_press = gui_ms_get();
    kb_port_data.timestamp_ms_pressing = gui_ms_get();
    return &kb_port_data;
}

gui_wheel_port_data_t *port_wheel_get_data(void)
{
    wheel_port_data.timestamp_ms = gui_ms_get();
    return &wheel_port_data;
}


static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .kb_get_port_data = port_kb_get_data,
    .wheel_get_port_data = port_wheel_get_data,
    .tp_height = 0,
    .tp_witdh = 0,
    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 800,
    .quick_slide_time_ms = 50,

    .kb_short_button_time_ms = 30,
    .kb_long_button_time_ms = 800,
};


void gui_port_indev_init(void)
{
    gui_indev_info_register(&indev);
}




