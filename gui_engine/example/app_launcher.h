/*
 * File      : app_launcher.h
 */
#ifndef __APP_LAUNCHER_H__
#define __APP_LAUNCHER_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_iconlist.h>

#include <gui_seekbar.h>

#include <gui_alert.h>
#include <gui_page.h>

#include <gui_d_island.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "app_dialing.h"
#include "rtk_gui_resource.h"
#include "gui_img_with_animate.h"
#include <gui_magic_img.h>
#include <gui_app.h>
#include <stdlib.h>
#include "acc_engine.h"
#include "gui_grid.h"
#include "gui_scroll_text.h"
#include "gui_radio.h"
#include "gui_switch.h"
typedef struct app_launcher app_launcher_t;


gui_app_t *get_launcher_app(void);
/*call back handler declaration*/
void clock_rotation_handler(void *p_xtimer);
void setting_icon_cb(void *obj, gui_event_t e);
void heartrate_icon_cb(void *obj, gui_event_t e);
void iconlist_win_cb(void *obj, gui_event_t e);
void flappybird_iocn_cb(void *obj, gui_event_t e);
void btcall_icon_cb(void *obj, gui_event_t e);
void iconbox_icon_cb(void *obj, gui_event_t e);
void home_icon_cb(void *obj, gui_event_t e);
void album_icon_cb(void *obj, gui_event_t e);
void create_icon_list_theme0(void *parent);
void create_icon_list_theme1(void *parent);
void create_icon_list_theme2(void *parent);
void create_icon_list_theme3(void *parent);
void design_launcher_ui(gui_app_t *app);
void handle_launcher_msg(void);
#ifdef __cplusplus
}
#endif

#endif
