#include <gui_tabview.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include "gui_switch.h"
#include "gui_canvas.h"
#include "gui_components_init.h"
static void app_home_ui_design(gui_app_t *app);
gui_tabview_t *home_tab;

extern void design_tv_home(void *parent);
extern void design_tab_sport(void *parent);
extern void design_tab_heart(void *parent);
extern void design_tab_blood(void *parent);
extern void design_tab_call(void *parent);
extern void design_tab_music(void *parent);
extern void design_tab_note(void *parent);
extern void design_tab_weather(void *parent);
extern void design_tab_breath(void *parent);
extern void design_tab_sleep(void *parent);

static gui_app_t app_home =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_home_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_home(void)
{
    return &app_home;
}
static void app_home_ui_design(gui_app_t *app)
{
    gui_log("app_hk_bee4_ui_design\n");

    home_tab = gui_tabview_create(&(app->screen), "tabview", 0, 0, 320, 384);
    gui_tabview_set_style(home_tab, REDUCTION);
    gui_tab_t *tab_home = gui_tab_create(home_tab, "tab_home",            0, 0, 0, 0, 0, 0);
    gui_tab_t *tab_sport = gui_tab_create(home_tab, "tab_sport",          0, 0, 0, 0, 1, 0);
    gui_tab_t *tab_heart = gui_tab_create(home_tab, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tab_blood = gui_tab_create(home_tab, "tb_blood",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tab_call = gui_tab_create(home_tab, "tb_call",             0, 0, 0, 0, 4, 0);
    gui_tab_t *tab_music = gui_tab_create(home_tab, "tb_music",           0, 0, 0, 0, 5, 0);
    gui_tab_t *tab_weather = gui_tab_create(home_tab, "tb_weather",       0, 0, 0, 0, 6, 0);
    gui_tab_t *tab_note = gui_tab_create(home_tab, "tb_note",             0, 0, 0, 0, 7, 0);
    gui_tab_t *tab_breath = gui_tab_create(home_tab, "tb_breath",         0, 0, 0, 0, 8, 0);
    gui_tab_t *tab_sleep = gui_tab_create(home_tab, "tb_note",            0, 0, 0, 0, 9, 0);

    design_tv_home(tab_home);
    design_tab_sport(tab_sport);
    design_tab_heart(tab_heart);
    design_tab_blood(tab_blood);
    design_tab_call(tab_call);
    design_tab_music(tab_music);
    design_tab_weather(tab_weather);
    design_tab_note(tab_note);
    design_tab_breath(tab_breath);
    design_tab_sleep(tab_sleep);


}
uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./realgui/example/screen_320_384/root_image_hongkong_bee4/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_home());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

