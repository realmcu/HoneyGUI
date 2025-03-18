#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_app.h"
#include "gui_server.h"
#include "gui_components_init.h"


static void set_arc_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = RGB565;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = w;
    head->h = h;
}


#include "bitmap.h"
#include "raw_bitmap.h"
#include "rasterizer.h"
#include "renderer.h"
#include "clipper.h"
#include "path.h"
#include "stroke.h"
#include "filling_rules.h"
#include "blenders_generic.h"
#include "nanovg_vertex.h"


using namespace agge;

template <typename T>
agge::rect<T> mkrect(T x1, T y1, T x2, T y2)
{
    agge::rect<T> r = {x1, y1, x2, y2};

    return r;
}

#include "stdlib.h"
#include "gui_canvas_round_rect.h"


static void app_ui_design(gui_app_t *app)
{

    float w = 480.0f;
    float h = 480.0f;
    gui_log("line = %d \n", __LINE__);

    rasterizer<clipper<int> > ras;
    ras.reset();
    rect<agge::real_t> clip_r = mkrect(0.0f, 0.0f, w, h);
    ras.set_clipping(clip_r);
    ras.move_to(0, 200);
    ras.line_to(100, 0);
    ras.line_to(200, 200);
    ras.line_to(300, 0);
    ras.line_to(400, 200);
    ras.close_polygon();
    ras.sort();

    blender_solid_color_rgb< pixel16_bgr565> blender(0, 0xff, 0xff, 255);
    uint8_t *rgb_image = (uint8_t *)gui_malloc(w * h * 2 + 8);
    memset(rgb_image, 0, w * h * 2 + 8);
    set_arc_w_and_h((gui_rgb_data_head_t *)rgb_image, w, h);
    bitmap<pixel16_bgr565, raw_bitmap> surface(w, h, w * 2, rgb_image + 8);
    renderer ren;
    ren(surface, 0 /*no windows*/, ras /*mask*/, blender, winding<>());

    gui_img_create_from_mem(&(app->screen),  "study-svg", (void *)rgb_image, 0, 0, 0, 0);
    // surface.blit(0, 0, 200, 200);

    // gui_canvas_round_rect_create(&(app->screen), "name", 240, 240, 100, 100, 30, APP_COLOR_WHITE);

    return;

}



static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "RealGUI_box2d_demo",
        .x    = 0,
        .y    = 0,
    },
    .active_ms = 1000 * 60 * 60,
    .thread_entry = NULL,
    .ui_design = app_ui_design,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


