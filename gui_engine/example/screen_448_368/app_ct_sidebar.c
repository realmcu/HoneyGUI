#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_app.h"

static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 200));
    nvgFill(canvas->vg);
}

static void touch_long_cb(void *obj, gui_event_t e)
{
    gui_app_shutdown(gui_current_app());
    // gui_log("line = %d \n", __LINE__);
    // gui_mem_debug();
    // gui_log("line = %d \n", __LINE__);

#ifdef ENABLE_LVGL_GUI
    gui_switch_app(gui_current_app(), get_app_lvgl());
#endif
}

void  page_left_figure(void *parent_widget)
{

    gui_win_t *parent = gui_win_create(parent_widget, NULL, 50, 30, 0, 0);
    gui_img_t *time =  gui_img_create_from_mem(parent, "time", TIME_BIN, 142, 20, 0, 0);
    gui_img_t *data =  gui_img_create_from_mem(parent, "data", DATA_BIN, 132, 64, 0, 0);
    gui_img_t *week =  gui_img_create_from_mem(parent, "week", SUN_BIN, 213, 64, 0, 0);

    gui_img_t *figure_bk0 =  gui_img_create_from_mem(parent, "figure_bk0", RECT_110_108_BIN,
                                                     82, 98, 0, 0);
    gui_img_set_opacity(figure_bk0, 200);
    gui_img_t *sky =  gui_img_create_from_mem(parent, "sky", SKY1_BIN, 123, 112, 0, 0);
    gui_img_t *centigrade0 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
                                                      106, 183, 0, 0);
    gui_img_t *centigrade1 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
                                                      124, 183, 0, 0);
    gui_img_t *centigrade2 =  gui_img_create_from_mem(parent, "centigrade", CENTIGRADE_BIN,
                                                      132, 166, 0, 0);

    gui_img_t *map =  gui_img_create_from_mem(parent, "siri", MAP_BIN, 202, 98, 0, 0);

    gui_img_t *figure_bk1 =  gui_img_create_from_mem(parent, "figure_bk1", RECT_228_64_BIN,
                                                     82, 214, 0, 0);
    gui_img_set_opacity(figure_bk1, 200);
    gui_img_t *heart_rate =  gui_img_create_from_mem(parent, "heart_rate", HEART_RATE_BIN,
                                                     98, 225, 0, 0);
    gui_img_t *sound =  gui_img_create_from_mem(parent, "sound", SOUND_RECORDING_BIN, 150,
                                                225, 0, 0);
    gui_img_t *sport =  gui_img_create_from_mem(parent, "sport", SPORT_BIN, 202, 225, 0, 0);
    gui_img_t *ring =  gui_img_create_from_mem(parent, "ring", RING_BIN, 254, 225, 0, 0);

    gui_img_t *figure_bk2 =  gui_img_create_from_mem(parent, "figure_bk2", SOUND_BIN, 72,
                                                     319, 0, 0);
    gui_img_t *siri =  gui_img_create_from_mem(parent, "siri", SIRI_BIN, 161, 297, 0, 0);

    gui_obj_add_event_cb(siri, (gui_event_cb_t)touch_long_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

}
void page_ct_sidebar(void *parent)
{
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
    page_left_figure(parent);
}