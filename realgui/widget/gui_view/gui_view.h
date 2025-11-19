/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
  * @brief The view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details Switch the current view by sliding or clicking.
  * @author shel_deng@realsil.com.cn
  * @date 2025/02/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
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
#include "gui_matrix.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/* VIEW_SWITCH_STYLE enum start*/
typedef enum
{
    SWITCH_INIT_STATE = 0x0000, ///< Switch out to left.
    SWITCH_IN_STILL_USE_BLUR,   ///< Switch in still with gauss blur.
    SWITCH_OUT_STILL_USE_BLUR,  ///< Switch out still with gauss blur.

    SWITCH_OUT_TO_LEFT_USE_TRANSLATION = 0x0100, ///< Switch out to left with transition effect.
    SWITCH_OUT_TO_RIGHT_USE_TRANSLATION, ///< Switch out to right with transition effect.
    SWITCH_OUT_TO_TOP_USE_TRANSLATION, ///< Switch out to top with transition effect.
    SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION, ///< Switch out to bottom with transition effect.

    SWITCH_IN_FROM_LEFT_USE_TRANSLATION, ///< Switch in from left with transition effect.
    SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, ///< Switch in from right with transition effect.
    SWITCH_IN_FROM_TOP_USE_TRANSLATION, ///< Switch in from top with transition effect.
    SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION, ///< Switch in from bottom with transition effect.
    SWITCH_IN_FROM_TOP_RIGHT_USE_TRANSLATION,
    SWITCH_IN_CENTER_ZOOM_FADE,

    SWITCH_IN_FROM_LEFT_USE_CUBE = 0x0200, ///< Switch in from left with cube effect.
    SWITCH_IN_FROM_RIGHT_USE_CUBE, ///< Switch in from right with cube effect.
    SWITCH_IN_FROM_TOP_USE_CUBE, ///< Switch in from top with cube effect.
    SWITCH_IN_FROM_BOTTOM_USE_CUBE, ///< Switch in from bottom with cube effect.

    SWITCH_OUT_TO_LEFT_USE_CUBE, ///< Switch out to left with cube effect.
    SWITCH_OUT_TO_RIGHT_USE_CUBE, ///< Switch out to right with cube effect.
    SWITCH_OUT_TO_TOP_USE_CUBE, ///< Switch out to top with cube effect.
    SWITCH_OUT_TO_BOTTOM_USE_CUBE, ///< Switch out to bottom with cube effect.

    SWITCH_IN_FROM_LEFT_USE_ROTATE = 0x0300, ///< Switch in from left with rotate effect.
    SWITCH_IN_FROM_RIGHT_USE_ROTATE, ///< Switch in from right with rotate effect.
    SWITCH_IN_FROM_TOP_USE_ROTATE, ///< Switch in from top with rotate effect.
    SWITCH_IN_FROM_BOTTOM_USE_ROTATE, ///< Switch in from bottom with rotate effect.

    SWITCH_OUT_TO_LEFT_USE_ROTATE, ///< Switch out to left with rotate effect.
    SWITCH_OUT_TO_RIGHT_USE_ROTATE, ///< Switch out to right with rotate effect.
    SWITCH_OUT_TO_TOP_USE_ROTATE, ///< Switch out to top with rotate effect.
    SWITCH_OUT_TO_BOTTOM_USE_ROTATE, ///< Switch out to bottom with rotate effect.

    SWITCH_IN_FROM_LEFT_USE_REDUCTION = 0x0400, ///< Switch in from left with reduction effect.
    SWITCH_IN_FROM_RIGHT_USE_REDUCTION, ///< Switch in from right with reduction effect.
    SWITCH_IN_FROM_TOP_USE_REDUCTION, ///< Switch in from top with reduction effect.
    SWITCH_IN_FROM_BOTTOM_USE_REDUCTION, ///< Switch in from bottom with reduction effect.

    SWITCH_OUT_TO_LEFT_USE_REDUCTION, ///< Switch out to left with reduction effect.
    SWITCH_OUT_TO_RIGHT_USE_REDUCTION, ///< Switch out to right with reduction effect.
    SWITCH_OUT_TO_TOP_USE_REDUCTION, ///< Switch out to top with reduction effect.
    SWITCH_OUT_TO_BOTTOM_USE_REDUCTION, ///< Switch out to bottom with reduction effect.

    SWITCH_OUT_NONE_ANIMATION = 0x0500, ///< No animation.
    SWITCH_OUT_ANIMATION_ZOOM,
    SWITCH_OUT_ANIMATION_FADE,
    SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
    SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
    SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_LEFT,       // scale to top left
    SWITCH_OUT_ANIMATION_ZOOM_TO_TOP_RIGHT,      // scale to top right
    SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_LEFT,      // scale from top left
    SWITCH_IN_ANIMATION_ZOOM_FROM_TOP_RIGHT,     // scale from top right
    SWITCH_IN_NONE_ANIMATION,           ///< No animation.
    SWITCH_IN_ANIMATION_ZOOM,
    SWITCH_IN_ANIMATION_FADE,
    SWITCH_IN_ANIMATION_MOVE_FADE,
    SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT,
    SWITCH_IN_ANIMATION_MOVE_FROM_LEFT,
    SWITCH_IN_ANIMATION_BOUNCE_FROM_RIGHT,


} VIEW_SWITCH_STYLE;
/* VIEW_SWITCH_STYLE enum end*/


#define EVENT_NUM_MAX 15

/** @brief View structure. */
struct gui_view_descriptor;
struct gui_view_on_event;
typedef struct gui_view
{
    gui_obj_t base;
    uint16_t animate_step;
    uint8_t opacity;

    VIEW_SWITCH_STYLE current_transition_style;
    gui_event_t current_event;
    const struct gui_view_descriptor *descriptor;
    void *blur_param;

    struct gui_view_on_event **on_event;
    uint8_t on_event_num;

    uint8_t checksum;
} gui_view_t;

/* gui_view_descriptor start*/
typedef struct gui_view_descriptor
{
    const char *name;
    gui_view_t **pView;

    void (* on_switch_in)(gui_view_t *view);  // Callback function when view is switched in and created.
    void (* on_switch_out)(gui_view_t
                           *view); // Callback function when view is switched out and destroyed.

uint8_t keep       :
    1; // If keep is true, the view will not be destroyed when switch to other view and will be created when register view
} gui_view_descriptor_t;
/* gui_view_descriptor end*/

typedef struct gui_view_on_event
{
    const gui_view_descriptor_t *descriptor;
    VIEW_SWITCH_STYLE switch_out_style;
    VIEW_SWITCH_STYLE switch_in_style;
    gui_event_t event;
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
 * @param parent The father widget it nested in.
 * @param descriptor Pointer to a descriptor that defines the new view to switch to.
 * @param x The X-axis coordinate relative to parent widget.
 * @param y The Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return Return the widget object pointer.
 */
gui_view_t *gui_view_create(void       *parent,
                            const gui_view_descriptor_t *descriptor,
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
 */
const gui_view_descriptor_t *gui_view_descriptor_get(const char *name);

/**
 * @brief Switches the current GUI view to a new view based on the specified event.
 *
 * This function handles the transition between GUI views. It takes the current
 * view context and switches it to a new view as described by the `descriptor`.
 * The transition is triggered by a specified event and can be customized with
 * different switch styles for the outgoing and incoming views.
 *
 * @param _this Pointer to the current GUI view context that is being manipulated.
 * @param descriptor Pointer to a descriptor that defines the new view to switch to.
 * @param switch_out_style Style applied to the outgoing view during the switch.
 * @param switch_in_style Style applied to the incoming view during the switch.
 * @param event The event that triggers the view switch.
 */
void gui_view_switch_on_event(gui_view_t *_this,
                              const gui_view_descriptor_t *descriptor,
                              VIEW_SWITCH_STYLE switch_out_style,
                              VIEW_SWITCH_STYLE switch_in_style,
                              gui_event_t event);

/**
 * @brief Switches directly the current GUI view to a new view through animation.
 *
 * This function handles the transition between GUI views. It takes the current
 * view context and switches it to a new view as described by the `descriptor`.
 * The transition animation can be customized with different animation switch styles for
 * the outgoing and incoming views.
 *
 * @param _this Pointer to the current GUI view context that is being manipulated.
 * @param descriptor Pointer to a descriptor that defines the new view to switch to.
 * @param switch_out_style Style applied to the outgoing view during the switch.
 * @param switch_in_style Style applied to the incoming view during the switch.
 */
void gui_view_switch_direct(gui_view_t *_this, const gui_view_descriptor_t *descriptor,
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
 * @return Return current view pointer.
 */
gui_view_t *gui_view_get_current(void);

/**
 * @brief Get next view pointer.
 *
 * @return Return next view pointer.
 */
gui_view_t *gui_view_get_next(void);

#ifdef __cplusplus
}
#endif
#endif
