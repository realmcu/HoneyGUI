#include "app_launcher.h"
#include <time.h>
// #include "trace.h"

static gui_app_t app_dashboard =
{
    .screen =
    {
        .name = "app_dashboard",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dashboard_ui_design,
    .active_ms = 60 * 1000 * 60,
};

gui_app_t *get_app_dashboard(void)
{
    return &app_dashboard;
}

void app_dashboard_ui_design(gui_app_t *app)
{
    gui_img_t *bg = gui_img_create_from_mem(&app->screen, "bg", BG_BIN, 0, 0, 0, 0);
}
void gui_dashboard_init()
{
    APP_PRINT_INFO0("gui_dashboard_init");
    gui_app_install(get_app_dashboard(), get_app_dashboard()->ui_design, NULL);
    gui_app_startup(get_app_dashboard());
}
