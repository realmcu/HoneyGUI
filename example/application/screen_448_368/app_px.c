#include <gui_app.h>
#include "PainterEngine.h"
#include "gui_components_init.h"


static void app_px_ui_design(gui_app_t *app);

static void px_loop(void *this)
{
    while ((1))
    {

    }
}

static gui_app_t app_px =
{
    .screen =
    {
        .name = "app_px",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_px_ui_design,
    .thread_entry = px_loop,
    .active_ms = 1000000,
    .lvgl = true,
};

void *get_app_px(void)
{
    return &app_px;
}



static void app_px_ui_design(gui_app_t *app)
{


}

#if 0
static int app_init(void)
{
    gui_server_init();
    gui_app_startup(get_app_px());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
#endif
