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
static void canvas1_draw(gui_canvas_t *c)
{

    canvas_rectangle_t r = {0};
    memset(&r, 0, sizeof(r));

    r.fill.color_data.rgba = 0x000000ff;
    r.width = 454;
    r.height = 454;
    gui_canvas_api.rectangle(c, &r);

}
void curtain_left(void *curtian)
{
    gui_canvas_t *canvas1 = gui_canvas_create(curtian, "canvas1", 0, 0, 454, 454, 0);
    canvas1->draw = canvas1_draw;
    static void *launcher2_curtain1_charge_video_array[] =
    {
        CHARGE2_BIN, CHARGE4_BIN, CHARGE6_BIN, CHARGE8_BIN, CHARGE10_BIN, CHARGE12_BIN, CHARGE14_BIN, CHARGE16_BIN, CHARGE18_BIN, CHARGE20_BIN, CHARGE22_BIN, CHARGE24_BIN, CHARGE26_BIN, CHARGE28_BIN, CHARGE30_BIN, CHARGE32_BIN, CHARGE34_BIN, CHARGE36_BIN, CHARGE38_BIN, CHARGE40_BIN, CHARGE42_BIN, CHARGE44_BIN, CHARGE46_BIN, CHARGE48_BIN, CHARGE50_BIN, CHARGE52_BIN, CHARGE54_BIN, CHARGE56_BIN, CHARGE58_BIN, CHARGE60_BIN, CHARGE62_BIN, CHARGE64_BIN, CHARGE66_BIN, CHARGE68_BIN, CHARGE70_BIN, CHARGE72_BIN, CHARGE74_BIN, CHARGE76_BIN, CHARGE78_BIN, CHARGE80_BIN, /*CHARGE82_BIN, CHARGE84_BIN, CHARGE86_BIN, CHARGE88_BIN, CHARGE90_BIN, CHARGE92_BIN, CHARGE94_BIN, CHARGE96_BIN, CHARGE98_BIN, CHARGE100_BIN, CHARGE102_BIN, CHARGE104_BIN, CHARGE106_BIN, CHARGE108_BIN, CHARGE110_BIN, CHARGE112_BIN, CHARGE114_BIN, CHARGE116_BIN, CHARGE118_BIN, CHARGE120_BIN,*/
    };
    void *charge_img = (void *)gui_dynamic_create_from_mem((void *)curtian, "g",
                                                           launcher2_curtain1_charge_video_array,
                                                           470 - 120 * 2, 0, 59 - 20, 40, INT32_MAX);
    gui_img_scale(charge_img, 2, 2);
}
