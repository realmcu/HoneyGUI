/*
 * File      : app_launcher.h
 */
#ifndef __APP_DASHBOARD_LAUNCHER_H__
#define __APP_DASHBOARD_LAUNCHER_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_seekbar.h>
#include <gui_page.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "root_image_800_480/ui_resource.h"
#include <gui_app.h>
#include <stdlib.h>

gui_app_t *get_app_dashboard_launcher(void);
void app_dashboard_launcher_ui_design(gui_app_t *app);
void app_dashboard_launcher_update_thread(void *this);
gui_win_t *win_main_display = NULL;
gui_win_t *win_connected_display = NULL;

#ifdef __cplusplus
}
#endif

#endif