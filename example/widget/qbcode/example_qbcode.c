/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* gui qbcode widget example start*/
#include "gui_qbcode.h"
#include "gui_components_init.h"

/* Use the following two macro to checkout qrcode or barcode */
// #define DISPLAY_QRCODE   QRCODE_DISPLAY_IMAGE
// #define DISPLAY_QRCODE   QRCODE_DISPLAY_SECTION
#define DISPLAY_BARCODE  BARCODE_DISPLAY_IMAGE
// #define DISPLAY_BARCODE  BARCODE_DISPLAY_SECTION


#define SCREEN_WIDTH     (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT    (int16_t)gui_get_screen_height()


static int app_init(void)
{
#ifdef DISPLAY_QRCODE
#define QRCODE_WIDTH     360
#define QRCODE_HEIGHT    360
    char *str = "https://www.realtek.com/";

    gui_qbcode_t *qrcode = gui_qbcode_create(gui_obj_get_root(),
                                             "qrcode",
                                             SCREEN_WIDTH / 2 - QRCODE_WIDTH / 2,
                                             SCREEN_HEIGHT / 2 - QRCODE_HEIGHT / 2,
                                             QRCODE_WIDTH,
                                             QRCODE_HEIGHT,
                                             DISPLAY_QRCODE,
                                             QRCODE_ENCODE_TEXT);

    gui_qbcode_config(qrcode, (uint8_t *)str, strlen(str), 3);
#elif defined(DISPLAY_BARCODE)
#define BARCODE_WIDTH     (143 * 3)
#define BARCODE_HEIGHT    143
#define ENCODED_TYPE     BARCODE_ENCODE_TEXT
    char *str = "1234567890";

    gui_qbcode_t *barcode = gui_qbcode_create(gui_obj_get_root(),
                                              "qrcode",
                                              SCREEN_WIDTH / 2 - BARCODE_WIDTH / 2,
                                              SCREEN_HEIGHT / 2 - BARCODE_HEIGHT / 2,
                                              BARCODE_WIDTH,
                                              BARCODE_HEIGHT,
                                              DISPLAY_BARCODE,
                                              ENCODED_TYPE);

    gui_qbcode_config(barcode, (uint8_t *)str, strlen(str) + 1, 10);
#endif

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui qbcode widget example end*/


