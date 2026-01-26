/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "draw_img.h"
#include "gui_fb.h"
#include "gui_server.h"
#include "gui_obj.h"
#include <string.h>



static bool GUI_SERVER_ALLOW_DLPS = false;




bool gui_server_dlps_check(void)
{
    return GUI_SERVER_ALLOW_DLPS;
}


