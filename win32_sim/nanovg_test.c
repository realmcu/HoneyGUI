/*
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/


#include <stdlib.h>
#include <stdio.h>
#include "gui_server.h"
#include "menu_config.h"
#include "nanovg.h"


void nanosv_test(void)
{
    gui_log("nanosv_test !! \n");
    struct gui_dispdev *dc = gui_get_dc();
    NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                   (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);

    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
    nvgSave(vg);

    nvgBeginPath(vg);
    float center_y = 240;
    float center_x = 240;
    float radius = 100;
    nvgCircle(vg, center_x, center_y, radius);
    nvgFillColor(vg, nvgRGBA(0XFF, 0XFF, 0XFF, 0xff));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, 0, 0);
    nvgLineTo(vg, 300, 0);
    nvgLineTo(vg, 0, 300);

    nvgFillColor(vg, nvgRGBA(0XFF, 0XFF, 0XFF, 0xff));
    nvgFill(vg);

    nvgStrokeColor(vg, nvgRGBA(0XFF, 0X00, 0X00, 0xff));
    nvgStrokeWidth(vg, 8.0f);
    nvgStroke(vg);



    nvgRestore(vg);
    nvgEndFrame(vg);

    nvgDeleteAGGE(vg);

    dc->lcd_update(dc);

}


int nanosv_main(void)
{
    gui_debug_sethook(nanosv_test);
    rtgui_server_init();

    return 0;

}

