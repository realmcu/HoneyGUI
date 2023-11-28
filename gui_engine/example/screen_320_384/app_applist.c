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
#include "gui_img.h"

static void app_applist_ui_design(gui_app_t *app);

static gui_app_t app_applist =
{
    .screen =
    {
        .name = "app_applist",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_applist_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_applist(void)
{
    return &app_applist;
}

#define sin60 0.866
#define cos60 0.5
#define r 80

static void app_applist_ui_design(gui_app_t *app)
{
    gui_log("app_applist_ui_design\n");

    gui_img_t *applist0 = gui_img_create_from_mem(&(app->screen), "applist",
                                                  APPLIST_ADDRESS_BOOK_BIN, 160 - 36, 192 - 36, 0, 0);

    gui_img_t *applist1 = gui_img_create_from_mem(applist0, "applist", APPLIST_BPM_BIN, r,
                                                  0, 0, 0);
    gui_img_t *applist2 = gui_img_create_from_mem(applist0, "applist",
                                                  APPLIST_AIR_PRESSURE_BIN, round(r * cos60), round(r * sin60), 0, 0);
    gui_img_t *applist3 = gui_img_create_from_mem(applist0, "applist",
                                                  APPLIST_ALARM_CLOCK_BIN, -round(r * cos60), round(r * sin60), 0, 0);
    gui_img_t *applist4 = gui_img_create_from_mem(applist0, "applist",
                                                  APPLIST_CALCULATOR_BIN, -r, 0, 0, 0);
    gui_img_t *applist5 = gui_img_create_from_mem(applist0, "applist", APPLIST_DANCE_BIN,
                                                  -round(r * cos60), -round(r * sin60), 0, 0);
    gui_img_t *applist6 = gui_img_create_from_mem(applist0, "applist", APPLIST_GPS_BIN,
                                                  round(r * cos60), -round(r * sin60), 0, 0);

    // gui_img_t *home_bg = gui_img_create_from_mem(&(app->screen), "home_bg", APP_HEART_RATE_BIN, 0,
    //                                                          0, 0, 0);
    // gui_img_set_mode(home_bg, IMG_BYPASS_MODE);
}
