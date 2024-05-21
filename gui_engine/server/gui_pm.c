/**
 * @file gui_server.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <guidef.h>
#include <draw_img.h>
#include <draw_font.h>
#include <gui_fb.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <kb_algo.h>
#include <wheel_algo.h>
#include <string.h>
#include <gui_app.h>



static bool GUI_SERVER_ALLOW_DLPS = false;



bool gui_server_dlps_check(void)
{
    return GUI_SERVER_ALLOW_DLPS;
}


void gui_pm_judge_active(gui_app_t *app)
{
    if ((gui_ms_get() - app->start_ms) < app->active_ms)
    {
        gui_msg_t msg = {.event = GUI_EVENT_SERVER_ACTIVE};
        gui_send_msg_to_server(&msg);
    }
    else
    {
        gui_log("!!!gui app out of active time!!!");
    }
}

