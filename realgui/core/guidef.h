/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUIDEF_H__
#define __GUIDEF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "def_compiler.h"
#include "def_type.h"
#include "def_list.h"
#include "def_event.h"
#include "def_color.h"
#include "def_vg.h"
#include "def_input.h"
#include "def_os.h"
#include "def_file.h"
#include "def_fs.h"
#include "def_fb.h"
#include "def_engine.h"
#include "def_msg.h"


/*============================================================================*
 *                         Types
 *============================================================================*/



/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_UNUSED(x)           (void)(x)             /* macro to get rid of 'unused parameter' warning */
#define GUI_TYPE(type, obj)     ((type *)obj)
#define GET_BASE(_p)            ((gui_obj_t *)_p)
#define GUI_BASE(_p)            ((gui_obj_t *)_p)




#define GUI_SPEED_RECODE_LENGTH 5
#define GUI_SPEED_RECODE_DEFINE int speed_recode[GUI_SPEED_RECODE_LENGTH];
#define GUI_SPEED_MAX 60
#define GUI_SPEED_MIN 7
#define GUI_FRAME_STEP 50
#define GUI_MAGIC_NUMBER 0x0b




#define IMPORT_GUI_TOUCHPAD  extern touch_info_t *tp_get_info(void); \
    touch_info_t *tp = tp_get_info();

#define IMPORT_GUI_WHEEL extern touch_info_t *wheel_get_info(void); \
    touch_info_t *wheel = wheel_get_info();











#ifdef __cplusplus
}
#endif

#endif

