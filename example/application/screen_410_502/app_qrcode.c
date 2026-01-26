/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_qbcode.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "qrcode_view"
#define QRCODE_WIDTH     360
#define QRCODE_HEIGHT    360
#define DISPLAY_TYPE     QRCODE_DISPLAY_IMAGE
#define ENCODED_TYPE     QRCODE_ENCODE_TEXT
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_qrcode_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_qrcode_design,
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void app_qrcode_design(gui_view_t *view)
{

    gui_view_switch_on_event(view, heartrate_view->name, SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                             SWITCH_IN_FROM_LEFT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    char *str = "https://www.realtek.com/";

    gui_qbcode_t *qrcode = gui_qbcode_create(view,
                                             "qrcode",
                                             410 / 2 - QRCODE_WIDTH / 2,
                                             502 / 2 - QRCODE_HEIGHT / 2,
                                             QRCODE_WIDTH,
                                             QRCODE_HEIGHT,
                                             DISPLAY_TYPE,
                                             ENCODED_TYPE);

    gui_qbcode_config(qrcode, (uint8_t *)str, strlen(str), 3);
    gui_img_set_quality(qrcode->qbcode_img, true);
}