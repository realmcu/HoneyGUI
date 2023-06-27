#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_iconlist.h>
#include "gui_canvas.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_grid.h"
#include <gui_dynamic_img.h>
#include "tp_algo.h"
#include <math.h>
#ifdef RTK_MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"
#else
#include "gui_cube.h"
#endif
void static sport_chart_draw(gui_canvas_t *c)
{
    float  end_angle1 = 0.0f;
    float  end_angle2 = 0.0f;
    float  end_angle3 = 0.0f;
    {
        static  float pro  = 0;
        static bool end_flag = false;
        if (pro >= 0.99f)
        {
            end_flag = !end_flag;
            pro = 0;
        }
        bool temp = (pro >= 0.99f || pro <= 0.01f);
        //if (!temp)
        {
            if (!end_flag)
            {
                end_angle1 = pro * (330 - 30) * 0.5f + 30;
                end_angle2 = pro * (330 - 30) * 0.75f + 30;
                end_angle3 = pro * (330 - 30) + 30;
            }
            else
            {
                end_angle1 = (1 - pro) * (330 - 30) * 0.5f + 30;
                end_angle2 = (1 - pro) * (330 - 30) * 0.75f + 30;
                end_angle3 = (1 - pro) * (330 - 30) + 30;
            }
//            gui_log("end_angle1:%f,%d,%f\n", end_angle1, end_flag, pro);
        }
        pro += 0.03f;
    }

    int w = gui_get_screen_width();
    int h = gui_get_screen_height();
    int gap = 6;
    int stroke_width = 30;
    canvas_arc_t arc1 = {0};
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - gap - stroke_width;
    arc1.stroke.fill.color_data.rgba = 0xffff0099;
    arc1.stroke.stroke_width = stroke_width;
    arc1.stroke.stroke_linecap = STROKE_CAP_ROUND;
    arc1.start_angle = 30;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle1;
    arc1.stroke.fill.color_data.rgba = 0xffff00ff;
    gui_canvas_api.arc(c, &arc1);
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - (gap + stroke_width) * 2;
    arc1.stroke.fill.color_data.rgba = 0xff00ff99;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle2;
    arc1.stroke.fill.color_data.rgba = 0xff00ffff;
    gui_canvas_api.arc(c, &arc1);
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - (gap + stroke_width) * 3;
    arc1.stroke.fill.color_data.rgba = 0xffff99;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle3;
    arc1.stroke.fill.color_data.rgba = 0xffffff;
    gui_canvas_api.arc(c, &arc1);
}

void sport_chart(gui_obj_t *parent)
{
    gui_canvas_t *c = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    c->draw = sport_chart_draw;
}
