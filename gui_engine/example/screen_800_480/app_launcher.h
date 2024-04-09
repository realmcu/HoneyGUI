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
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "root_image_800_480/ui_resource.h"
#include <gui_app.h>
#include <stdlib.h>
#include "draw_font.h"
#include <gui_app.h>

gui_app_t *get_app_dashboard(void);
void app_dashboard_ui_design(gui_app_t *app);
void gui_dashboard_init(void);

#ifdef __cplusplus
}
#endif

#endif
