#include "gui_app.h"
#include "gui_graphic.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_arc.h"
#include "gui_arc_hl.h"




static void pie3_cb(void *p);
static void create_pie3(gui_obj_t *parent, int x, int y);
static void arc1_cb(void *p);
static void arc2_cb(void *p);
static void arc3_cb(void *p);
void create_pie1(gui_obj_t *parent, int x, int y);
static void pie2_cb(void *p);
void create_pie2(gui_obj_t *parent, int x, int y);
static void win_return_cb(void *obj, gui_event_t e);
static void progress_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());
    gui_obj_add_event_cb(win, win_return_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE, NULL);
    create_pie3(&app->screen, 0, 0);
    //create_pie1(&app->screen,160,160);
    //create_pie2(&app->screen,0,160);

}

/**
 * @brief app stuff
 *
 */
static gui_app_t progress =
{
    .screen =
    {
        .name = "progress",
    },
    .ui_design = progress_ui_design,
};
gui_app_t *get_app_progress(void)
{
    return &progress;
}






static void pie3_cb(void *p)
{
    gui_arc_hl_t *hl = p;
    float per = hl->animate->progress_percent;
    int from = 0;
    int to = 360;
    gui_log("per:%f\n", per);
    if (per < 0.5f)
    {
        hl->highlight->set(hl->highlight, hl->highlight->base_canvas.base.color, 0, (to - from)*per * 2,
                           true);
    }
    else
    {
        hl->highlight->set(hl->highlight, hl->highlight->base_canvas.base.color, 0,
                           (to - from) * (1.0f - per) * 2, true);
    }
}
static void create_pie3(gui_obj_t *parent, int x, int y)
{
    gui_arc_hl_t *hl = gui_arc_highlight_create(
                           parent,
                           "arc",
                           x + 80,
                           y + 80,
                           60,
                           20,
                           0x1E90FFff,
                           0x1E90FF66,
                           50,
                           160,
                           true);
    (hl)->set_animate(hl, 2000, -1, pie3_cb, hl);


}
/*
static void create_rect4(gui_obj_t *parent)
{
    int rcr_w = 180;
    int radius = 15;
    gui_round_corner_rect_t *rcr4 = gui_round_corner_rect_create(parent, "rcr2", 0, 0, rcr_w, rcr_w,
                                                                 radius, 0xffffff44);
    create_pie3((void *)rcr4,0,0);
}*/
static void arc1_cb(void *p)
{
    gui_arc_t *arc1 = p;
    int from = 0;
    int to = 270;
    int x;
    float per = arc1->animate->progress_percent;

    if (per < 0.5f)
    {
        x = from + (to - from) * (per * 2);
    }
    else
    {
        x = 270 + (from - to) * ((per - 0.5f) * 2);
    }
    if (x < 20)
    {
        x = 20;
    }
    arc1->set(arc1, arc1->base_canvas.base.color, from, x, true);
}
static void arc2_cb(void *p)
{
    gui_arc_t *arc2 = p;
    int from = 270;
    int to = 20;
    int x;
    float per = arc2->animate->progress_percent;

    if (per < 0.5f)
    {
        x = from + (to - from) * (per * 2);
    }
    else
    {
        x = to + (from - to) * ((per - 0.5f) * 2);
    }
    if (x >= 260)
    {
        x = 260;
    }
    arc2->set(arc2, arc2->base_canvas.base.color, from, x, true);
}
static void arc3_cb(void *p)
{
    gui_arc_t *arc3 = p;
    int from = 270;
    int to = 20;
    int x;
    float per = arc3->animate->progress_percent;

    if (per < 0.5f)
    {
        x = from + (to - from) * (per * 2);
    }
    else
    {
        x = to + (from - to) * ((per - 0.5f) * 2);
    }
    if (x >= 260)
    {
        x = 260;
    }
    arc3->set(arc3, arc3->base_canvas.base.color, x, 360, true);
}
void create_pie1(gui_obj_t *parent, int x, int y)
{
    gui_win_t *win = gui_win_create(parent, "pie1_win", x, y, 200, 200);
    gui_arc_t *arc1 = gui_arc_create(win, "arc", 90, 90, 60, 20, 0x00ff00ff, 0, 0, true);
    (arc1)->set_animate(arc1, 4000, -1, arc1_cb, arc1);

    gui_arc_t *arc2 = gui_arc_create(win, "arc", 90, 90, 35, 20, 0xff0000ff, 270, 360, true);
    (arc2)->set_animate(arc2, 2000, -1, arc2_cb, arc2);

    gui_arc_t *arc3 = gui_arc_create(win, "arc", 90, 90, 10, 20, 0x0000ffff, 270, 360, true);
    (arc3)->set_animate(arc3, 4000, -1, arc3_cb, arc3);
}
/*
static void create_rect5(gui_obj_t *parent)
{
    int rcr_w = 180;
    int radius = 15;
    gui_round_corner_rect_t *rcr5 = gui_round_corner_rect_create(parent, "rcr1", 0, 0, rcr_w, rcr_w,
                                                                 radius, 0xffffff44);
    create_pie1((void *)rcr5,0,0);
}
*/
struct pie2_p
{
    int stop1;
    int stop2;
    int stop3;
    int gap;
    gui_arc_t *arc1;
    gui_arc_t *arc2;
    gui_arc_t *arc3;
    float reverse;
};

static void pie2_cb(void *p)
{
    struct pie2_p *pie2 = p;
    float per = pie2->arc1->animate->progress_percent;
    int stop1, stop2, stop3;
    stop1 = pie2->stop1 + per * 60;
    stop2 = pie2->stop2 - per * 60;
    stop3 = pie2->stop3 + per * 60;
    pie2->arc1->set(pie2->arc1, pie2->arc1->base_canvas.base.color, stop3 + pie2->gap,
                    stop1 - pie2->gap, false);
    pie2->arc2->set(pie2->arc2, pie2->arc2->base_canvas.base.color, stop1 + pie2->gap,
                    stop2 - pie2->gap, false);
    pie2->arc3->set(pie2->arc3, pie2->arc3->base_canvas.base.color, stop2 + pie2->gap,
                    stop3 - pie2->gap, false);
}
void create_pie2(gui_obj_t *parent, int x, int y)
{
    gui_win_t *win = gui_win_create(parent, "pie2_win", x, y, 200, 200);
    static struct pie2_p p = {.gap = 5, .stop1 = 60, .stop2 = 60 + 120, .stop3 = 60 + 120 * 2};
    p.arc1 = gui_arc_create(win, "arc", 90, 90, 60, 20, 0x00ff00ff, p.stop3 + p.gap, p.stop1 - p.gap,
                            false);
    (p.arc1)->set_animate(p.arc1, 1000, -1, pie2_cb, &p);
    p.arc2 = gui_arc_create(win, "arc", 90, 90, 60, 10, 0xff0000ff, p.stop1 + p.gap, p.stop2 - p.gap,
                            false);
    p.arc3 = gui_arc_create(win, "arc", 90, 90, 60, 30, 0x0000ffff, p.stop2 + p.gap, p.stop3 - p.gap,
                            false);
}
/*
static void create_rect6(gui_obj_t *parent)
{
    int rcr_w = 180;
    int radius = 15;
    gui_round_corner_rect_t *rcr6 = gui_round_corner_rect_create(parent, "rcr2", 0, 0, rcr_w, rcr_w,
                                                                 radius, 0xffffff44);
    create_pie2((void *)rcr6,0,0);
}
*/
static void win_return_cb(void *obj, gui_event_t e)
{
    extern gui_app_t *get_launcher_app(void);

    gui_switch_app(get_app_progress(), get_launcher_app());
}






