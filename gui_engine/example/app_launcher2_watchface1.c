
#include <gui_obj.h>
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_curtain.h>
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
#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include "acc_engine.h"
#include "gui_canvas.h"
#include "nanovg_agge.h"

void static sport_chart_draw(gui_canvas_t *c)
{

    /*{
        GUI_RENDER_DATA
        NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
        nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
        nvgSave(vg);
        nvgBeginPath(vg);
        //nvgCircle(vg, center_x,center_y, radius);
        //nvgRect(vg, 20,20,w-40,h-40);
        //nvgRoundedRect(vg, 20,20,100,50, 25);
        nvgLineCap(vg, NVG_ROUND);
        nvgArc(vg, 200, 200, 100, (100 - 90.0F)*NVG_PI / 180.0f,
               (300 - 90.0F)*NVG_PI / 180.0f, NVG_CW);
        nvgStrokeColor(vg, nvgRGBA(255, 255, 0,
                                   255));
        nvgStrokeWidth(vg, 100);
        nvgStroke(vg);
        nvgRestore(vg);
        nvgEndFrame(vg);

        nvgDeleteAGGE(vg);
    }*/
    canvas_bezier_curve_t bc = {0};
    bc.start_x = 20;
    bc.end_x = 400;
    bc.start_y = 20;
    bc.end_y = 100;
    bc.control_x_1 = 20;
    bc.control_y_1 = 300;
    bc.control_x_2 = 200;
    bc.control_y_2 = 400;
    bc.stroke.fill.color_data.rgba = 0x00ffffff;
    bc.stroke.stroke_width = 30;
    bc.stroke.stroke_linecap = STROKE_CAP_ROUND;
    gui_canvas_api.bezier_curve(c, &bc);
}

static void sport_chart(gui_obj_t *parent)
{
    gui_canvas_t *c = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    c->draw = sport_chart_draw;
}
#ifdef RTK_MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#endif
void watchface(void *parent)
{
#ifdef RTK_MODULE_VG_LITE
    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(parent,  "path_clock", NULL, 0, 0,
                                                               454, 454);
#else
    //gui_button_create(parent, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2, 5134);
    sport_chart(parent);
#endif
}
