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
#ifdef MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"
#else
#include "gui_cube.h"
#endif
static float angle_to_center(float x0, float y0, float x1, float y1)
{
    float vx = x1 - x0;
    float vy = y1 - y0;
    float mag_v = sqrt(vx * vx + vy * vy);
    return acos(vy / mag_v);
}
float selector_radian_old;
static void palette_wheel_draw(gui_canvas_t *c)
{
    canvas_palette_wheel_t pw = {0};
    touch_info_t *tp = (void *)tp_get_info();
    static bool pressing;
    static float selector_radian;
    if (!pressing && tp->pressed)
    {
        pressing = true;
    }
    if (pressing && tp->released)
    {
        pressing = false;
    }
    if (pressing)
    {
        //selector_radian += 0.1f;
        //if(l2&&l1)

        /* code */
        switch (tp->type)
        {
        //gui_log("tp->type:%d\n",tp->type);
        case TOUCH_HOLD_X:
            break;
        default:
            {
                float degree = angle_to_center((float)(454 / 2), (float)(454 / 2), (float)(tp->x + tp->deltaX),
                                               (float)(tp->y + tp->deltaY)) * 360.0f / 2.0f / 3.14159f;
                if (tp->x + tp->deltaX > 454 / 2)
                {
                    degree =  -degree + 360.0f;
                }
                degree += 90.0f;
                if (degree > 360.0f)
                {
                    degree -= 360.0f;
                }
                degree = degree * M_PI / 180.0f * 1.8f;
                //degree = degree*M_PI/180.0f;
                pw.selector_radian = degree;
                selector_radian = pw.selector_radian;


                //gui_log("%f,%d,%d\n", degree,  tp->x + tp->deltaX, tp->y + tp->deltaY);
            }




            break;
        }


    }
    else
    {
        pw.selector_radian = selector_radian;
        selector_radian_old = selector_radian;
    }





    //pw.selector_radian = 2;
    pw.w = gui_get_screen_width();
    pw.h = gui_get_screen_height();
    gui_canvas_api.palette_wheel(c, &pw);
}
void palette_wheel(gui_obj_t *screen)
{
    gui_canvas_t *canvas = gui_canvas_create(screen, "c", 0, 0, 454, 454, 0);
    canvas->draw = palette_wheel_draw;

}
