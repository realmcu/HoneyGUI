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
void heart_rate_chart(gui_obj_t *parent)
{
    void *array[] = {CRAB_LIME_BIN, CRAB_LIME_BIN, RTL_ORIGIN_BIN, RTL_ORIGIN_BIN, WATCH_RED_BIN, WATCH_RED_BIN};
    gui_cube_create(parent, "cube", array, 0, 0, 454, 454);

}
