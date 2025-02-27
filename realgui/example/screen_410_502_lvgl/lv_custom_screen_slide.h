/**
 * @file lv_custom_screen_slide.h
 *
 */

#ifndef LV_CUSTOM_SCREEN_SLIDE_H
#define LV_CUSTOM_SCREEN_SLIDE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lvgl.h"

#include "lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

/**
 * @brief Touch event handler for screen sliding
 *
 * @param e LVGL event object containing:
 * - LV_EVENT_PRESSED:   Record initial touch position
 * - LV_EVENT_PRESSING:  Handle real-time sliding animation
 * - LV_EVENT_RELEASED:  Apply screen switch/reset logic
 */
static void touch_event_handler(lv_event_t *e);

/**
 * @brief Initialize dual-screen UI structure
 *
 * Creates two full-screen objects with:
 * - Centered text labels
 * - Colored bottom rectangles (red/blue)
 * - Touch event bindings
 * - Initial position configuration
 */
void create_screens(void);

/**
 * @brief Switch back to screen1 if not active
 *
 * Used for manual screen reset operations.
 * Logs screen switching status through LV_LOG.
 */
void change_screen(void);

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/



#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CUSTOM_SCREEN_SLIDE_H*/
