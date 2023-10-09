#include "root_image_hongkong/ui_resource.h"
#include "gui_perspective.h"
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_app.h"
#include <gui_tabview.h>
#include "app_hongkong.h"



static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");
    static gui_tabview_t *tv;

    tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, REDUCTION);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_blood = gui_tab_create(tv, "tb_blood",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 5, 0);

    page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_blood(tb_blood);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);
}

static void canvas_cb_black(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 128, 255));
    nvgFill(canvas->vg);
}

static void callback_prism_touch_clicked()
{
    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    gui_tree_free(screen);
    app_hongkong_ui_design(get_app_hongkong());
}
void callback_prism(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);

    gui_tree_free(screen);

    gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
    gui_canvas_t *canvas = gui_canvas_create(win, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
    uint8_t *array_flash[] = {ACTIVITY_BIN, BLOODOXYGEN_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};
    gui_perspective_t *img_test = gui_perspective_create(canvas, "test", array_flash, 0, 0, 0, 0);

    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_prism_touch_clicked, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
}