#include "app_launcher.h"
#include <time.h>
#include "trace.h"

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
    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_set_font_mem_resourse(48, QUICKSAND_MEDIUM_48_BIN, CP500_TABLE_BIN);
    gui_set_font_mem_resourse(56, QUICKSAND_SEMIBOLD_56_BIN, CP500_TABLE_BIN);
    gui_set_font_mem_resourse(64, QUICKSAND_SEMIBOLD_64_BIN, CP500_TABLE_BIN);
    gui_set_font_mem_resourse(96, QUICKSAND_MEDIUM_96_BIN, CP500_TABLE_BIN);

    gui_img_t *bg = gui_img_create_from_mem(&app->screen, "bg", BG_BIN, 0, 0, 0, 0);
//    gui_text_t *text1 = gui_text_create(&app->screen,  "text1",  0, 0, 800, 100);
//    gui_text_set(text1, "/////////////////", GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), 15, 96);
//    gui_text_mode_set(text1, LEFT);
}
void gui_dashboard_init()
{
    APP_PRINT_INFO0("gui_dashboard_init");
    gui_app_install(get_app_dashboard(), get_app_dashboard()->ui_design, NULL);
    gui_app_startup(get_app_dashboard());
}
