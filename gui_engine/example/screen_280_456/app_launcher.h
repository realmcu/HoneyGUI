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

#include <gui_seekbar.h>

#include <gui_page.h>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "root_image_8762g/ui_resource.h"
#include <gui_app.h>
#include <stdlib.h>
#include "acc_engine.h"
#include "gui_scroll_text.h"
#include "draw_font.h"

typedef struct app_launcher app_launcher_t;


gui_app_t *get_launcher_app(void);
/*call back handler declaration*/
void design_launcher_ui(gui_app_t *app);
#ifdef __cplusplus
}
#endif

#endif
