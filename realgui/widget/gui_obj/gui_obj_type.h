/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_TYPE_H__
#define __GUI_OBJ_TYPE_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum t_obj_type
{
    SCREEN                   = 0x0000,
    WINDOW                   = 0x0001,
    IMAGE_FROM_MEM           = 0x0002,
    TEXTBOX                  = 0x0003,
    SCROLLTEXTBOX            = 0x0004,
    VIEW                     = 0x0005,
    LIST                     = 0x0006,
    LIST_NOTE                = 0x0007,
} T_OBJ_TYPE;

/* Listed in execution order. Explicit values preserve the existing ABI. */
typedef enum
{
    OBJ_INPUT_PROCESS     = 0, /* Update input state before frame preparation. */
    OBJ_PRE_PROCESS       = 1, /* Run once before rendering the frame. */
    OBJ_PROCESS           = 3, /* Draw the current framebuffer section. */
    OBJ_EXTENSION_PROCESS = 2, /* Extend processing after the object and before its children. */
    OBJ_POST_PROCESS      = 4, /* Run once after all framebuffer sections. */
    OBJ_DESTROY           = 5, /* Run before object memory is released. */
} T_OBJ_CB_TYPE;

typedef struct gui_obj_timer
{
    uint32_t interval_ms;
    bool reload;
    void (*p_timer_callback)(void *);
    uint32_t expire_time;
} gui_obj_timer_t;

typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    /* Absolute layout position, recomputed once per frame from the parent
     * chain before any callback runs. This is what hit testing uses. */
    int16_t abs_x;
    int16_t abs_y;
    /* Absolute draw position. Normally equals abs_x/abs_y; a view transition
     * shifts it away from the layout position so an in-flight slide does not
     * move the input hit area. Always write it as abs + (own offset). */
    int16_t draw_x;
    int16_t draw_y;

    gui_node_list_t  child_list;
    gui_node_list_t  brother_list;

    //start of run time
    void (* obj_cb)(struct _gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
    //end of run time
    uint32_t type                       : 8;    // T_OBJ_TYPE; no need this, only use name
    uint32_t active                     : 1;    // this_widget flag means obj location in screen
uint32_t hidden                     :
    1;    // if true, object is not rendered and does not receive events; replaces not_show

    uint32_t suppress_conflict_obj_cnt  : 6;
    uint32_t opacity_value              : 8;
    uint32_t event_dsc_cnt              : 5;
    uint32_t create_done                : 1;
    uint32_t has_input_process_cb       : 1;
    uint32_t has_pre_process_cb         : 1;
    uint32_t has_process_cb             : 1;
    uint32_t has_post_process_cb        : 1;
    uint32_t has_destroy_cb             : 1;
    uint32_t extension_process_enabled  : 1;
    uint32_t has_subscribe              : 1;
    uint32_t dirty                      : 1;    // widget self-redraw request flag
    uint32_t magic                      : 4;
    gui_event_dsc_t   *event_dsc;
    struct _gui_obj_t **suppress_conflict_obj_list; //chrild obj list that has the same event

    gui_obj_timer_t *timer;
    void *user_data;
    gui_rect_t last_render_rect;
} gui_obj_t;


/*============================================================================*
*                         Macros
*============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/



#ifdef __cplusplus
}
#endif

#endif
