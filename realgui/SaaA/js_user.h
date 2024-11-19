/**
 * @file js_user.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-11-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __JS_USER_H__
#define __JS_USER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jerry_util.h>
#include <jerryscript.h>
#include <jerry_util.h>
#include "../3rd/js/js-port/config.h"
#include <gui_app.h>
#include <gui_obj.h>
#include <gui_text.h>
#include "gui_server.h"
#include "gui_win.h"
#include "gui_button.h"
#include "gui_progressbar.h"
#include "ezxml.h"
#include "gui_tabview.h"
#include "gui_server.h"
#include "gui_switch.h"
#include "gui_img.h"
#include "gui_keyboard.h"
#include "guidef.h"
#include "gui_page.h"
#include "gui_server.h"

#ifdef RTL87x2G
#include "wdt.h"
#include "rtl_wdt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct ezjs
{
    int a;
    int b;
};
typedef struct cb_arg
{
    jerry_value_t func;
    jerry_value_t *args_p;
    jerry_size_t args_count;
} cb_arg_t;


void js_gpio_init(void);
void js_matter_init(void);
void js_mesh_init(void);
void js_wifi_init(void);
void js_key_init(void);
void js_smarthome_init(void);
void js_matter_init();
void js_mesh_init();
jerry_value_t jerry_call_func_sem(const jerry_value_t func_obj_val,  /**< function object to call */
                                  const jerry_value_t this_val, /**< object for 'this' binding */
                                  const jerry_value_t args_p[], /**< function's call arguments */
                                  jerry_size_t args_count); /**< number of the arguments */

#ifdef __cplusplus
}
#endif

#endif

