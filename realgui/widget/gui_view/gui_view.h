/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_VIEW_H__
#define __GUI_VIEW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_img.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/* VIEW_SWITCH_STYLE enum start*/
typedef enum
{
    SWITCH_INIT_STATE = 0x0000, ///< Default status.

    SWITCH_OUT_TO_LEFT_USE_TRANSLATION = 0x0100, ///< Switch out to left with transition effect.
    SWITCH_OUT_TO_RIGHT_USE_TRANSLATION, ///< Switch out to right with transition effect.
    SWITCH_OUT_TO_TOP_USE_TRANSLATION, ///< Switch out to top with transition effect.
    SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION, ///< Switch out to bottom with transition effect.

    SWITCH_IN_FROM_LEFT_USE_TRANSLATION, ///< Switch in from left with transition effect.
    SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, ///< Switch in from right with transition effect.
    SWITCH_IN_FROM_TOP_USE_TRANSLATION, ///< Switch in from top with transition effect.
    SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION, ///< Switch in from bottom with transition effect.

    SWITCH_OUT_NONE_ANIMATION = 0x0500, ///< No animation.
    SWITCH_IN_NONE_ANIMATION,           ///< No animation.

} VIEW_SWITCH_STYLE;
/* VIEW_SWITCH_STYLE enum end*/


#define EVENT_NUM_MAX 15


/** @brief View structure. */
struct gui_view_descriptor;
struct gui_view_on_event;
typedef struct gui_view
{
    gui_obj_t base;
    uint32_t animate_step       : 16;
    uint32_t opacity            : 8;
    uint32_t on_event_num       : 8;

    VIEW_SWITCH_STYLE current_transition_style;
    gui_event_code_t current_event_code;
    const struct gui_view_descriptor *descriptor;
    struct gui_view_on_event **on_event;

    draw_img_t *bg_img;
    gui_color_t bg_color;
} gui_view_t;

/* gui_view_descriptor start*/
typedef struct gui_view_descriptor
{
    const char *name;
    gui_view_t **pView;

    /* Called when the view is switched in and (re)created. */
    void (*on_switch_in)(gui_view_t *view);
    /* Called when the view is switched out and destroyed. */
    void (*on_switch_out)(gui_view_t *view);

    /* If set, the view is kept resident (not destroyed on switch-out) and is
       pre-created when registered. */
    uint8_t keep          : 1;
} gui_view_descriptor_t;
/* gui_view_descriptor end*/

typedef struct gui_view_on_event
{
    const gui_view_descriptor_t *descriptor;
    VIEW_SWITCH_STYLE switch_out_style;
    VIEW_SWITCH_STYLE switch_in_style;
    gui_event_code_t event_code;
} gui_view_on_event_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a view widget.
 * @param parent Father widget it nested in.
 * @param name View name that can be used to find the target view descriptor.
 * @param x X-axis coordinate relative to parent widget.
 * @param y Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return Widget object pointer.
 */
gui_view_t *gui_view_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h);

/**
 * @brief Register view's descriptor.
 * @param descriptor Pointer to a descriptor that defines the new view to switch to.
 */
void gui_view_descriptor_register(const gui_view_descriptor_t *descriptor);

/**
 * @brief Get target view's descriptor by name.
 * @param name View descriptor's name that can be used to find the target view.
 * @return Pointer to the view descriptor.
 */
const gui_view_descriptor_t *gui_view_descriptor_get(const char *name);

/**
 * @brief Get target view by name.
 * @param name View descriptor's name that can be used to find the target view.
 * @return Pointer to the view.
 */
gui_view_t *gui_view_get(const char *name);

/**
 * @brief Switches the current GUI view to a new view based on the specified event.
 *
 * This function handles the transition between GUI views. It takes the current
 * view context and switches it to a new view as described by the target view name.
 * The transition is triggered by a specified event and can be customized with
 * different switch styles for the outgoing and incoming views.
 *
 * @param _this Pointer to the current GUI view context that is being manipulated.
 * @param target_view_name Target view name that can be used to find the target view descriptor.
 * @param switch_out_style Style applied to the outgoing view during the switch.
 * @param switch_in_style Style applied to the incoming view during the switch.
 * @param event Event that triggers the view switch.
 */
void gui_view_switch_on_event(gui_view_t *_this,
                              const char *target_view_name,
                              VIEW_SWITCH_STYLE switch_out_style,
                              VIEW_SWITCH_STYLE switch_in_style,
                              gui_event_code_t event);

/**
 * @brief Switches directly the current GUI view to a new view through animation.
 *
 * This function handles the transition between GUI views. It takes the current
 * view context and switches it to a new view as described by the target view name.
 * The transition animation can be customized with different animation switch styles for
 * the outgoing and incoming views.
 *
 * @param _this Pointer to the current GUI view context that is being manipulated.
 * @param target_view_name Target view name that can be used to find the target view descriptor.
 * @param switch_out_style Style applied to the outgoing view during the switch.
 * @param switch_in_style Style applied to the incoming view during the switch.
 */
void gui_view_switch_direct(gui_view_t *_this, const char *target_view_name,
                            VIEW_SWITCH_STYLE switch_out_style,
                            VIEW_SWITCH_STYLE switch_in_style);

/**
 * @brief Set view animate step.
 * @param _this Pointer to view.
 * @param step Animate step, the larger the value, the faster the animation speed.
 */
void gui_view_set_animate_step(gui_view_t *_this, uint16_t step);

/**
 * @brief Set view opacity.
 * @param _this Pointer to view.
 * @param opacity 0 is totally transparent, 255 is opaque.
 */
void gui_view_set_opacity(gui_view_t *_this, uint8_t opacity);

/**
 * @brief Get current view pointer.
 *
 * @return Current view pointer.
 */
gui_view_t *gui_view_get_current(void);

/**
 * @brief Get next view pointer.
 *
 * @return Next view pointer.
 */
gui_view_t *gui_view_get_next(void);

/**
 * @brief Set view background color. Default is black.
 * @param _this Pointer to view.
 * @param color Background color.
 */
void gui_view_set_bg_color(gui_view_t *_this, gui_color_t color);

/**
 * @brief Enable trigger move region, only influence GUI_EVENT_TOUCH_MOVE_* event. Valid region is 20% of screen's width/height.
 * @param enable True to enable, False to disable. Default is false.
 */
void gui_view_enable_trigger_move_region(bool enable);

#ifdef __cplusplus
}
#endif
#endif
