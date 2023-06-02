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
void grid_icon(void *parent)
{
    void *svg_array[] = {ACTIVITY_SVG, ALARM_SVG, HEARTRATE_SVG, MESSAGE_SVG, MUSIC_SVG, SETTING_SVG, CONTACTS_SVG, TELEPHONE_SVG, TIME_SVG, WEATHER_SVG, SLEEP_SVG};
    int svg_size_array[] = {5134, 2980, 2890, 2666, 2746, 2056,  3052, 2882, 2097, 3629, 2539};
    gui_grid_t *g1 = gui_grid_create(parent, 0, 0, 5, 5, 103, 103);
    g1->style = GRID_SCALE;
    size_t j = 0;
    size_t k = 1;
    for (size_t i = 0; i < 25; i++)
    {
        j++;
        if (j == 11)
        {
            j = 0;
        }
        k++;
        if (k == 11)
        {
            k = 0;
        }

        gui_svg_create_from_mem(g1, svg_array[j], svg_size_array[j], 0, 0, 100, 100);
    }
}
