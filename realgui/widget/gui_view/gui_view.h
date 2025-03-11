/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_view.c
  * @brief the view widget is a kind of container that makes switching more convenient and effectively reduces memory consumption.
  * @details switch the current view by sliding or clicking
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

typedef struct
{
    int8_t x;
    int8_t y;
} gui_view_id_t;

/* VIEW_SWITCH_STYLE enum start*/
typedef enum
{
    VIEW_STILL            = 0x0000, ///< Overlay effect with new view transplate in
    VIEW_TRANSPLATION     = 0x0001, ///< Transplate from the slide direction
    VIEW_REDUCTION        = 0x0002, ///< Zoom in from the slide direction
    VIEW_ROTATE           = 0x0003, ///< Rotate in from the slide direction
    VIEW_CUBE             = 0x0004, ///< Rotate in from the slide direction like cube
    VIEW_ANIMATION_NULL   = 0x0005,
    VIEW_ANIMATION_1,               ///< Recommended for startup
    VIEW_ANIMATION_2,               ///< Recommended for startup
    VIEW_ANIMATION_3,               ///< Recommended for startup
    VIEW_ANIMATION_4,               ///< Recommended for startup
    VIEW_ANIMATION_5,               ///< Recommended for startup
    VIEW_ANIMATION_6,               ///< Recommended for shutdown
    VIEW_ANIMATION_7,               ///< Recommended for shutdown
    VIEW_ANIMATION_8,               ///< Recommended for shutdown
} VIEW_SWITCH_STYLE;
/* VIEW_SWITCH_STYLE enum end*/


#define EVENT_NUM_MAX 10

/** @brief  view structure */

struct gui_view_descriptor;
struct gui_view_on_event;
typedef struct gui_view
{
    gui_obj_t base;
    int16_t release_x;
    int16_t release_y;
    int16_t release_his; // record release for moveback judge
    gui_animate_t *animate;
    gui_view_id_t cur_id;
    VIEW_SWITCH_STYLE style;

    uint32_t view_switch_ready       : 1; // 1: target view switch done
    uint32_t event                   : 1; // 1: stop setting event
    uint32_t moveback                : 1; // 1: move to the opposite direction
    uint32_t view_tp                 : 1; // 1: enable to update release
    uint32_t view_left               : 1;
    uint32_t view_right              : 1;
    uint32_t view_up                 : 1;
    uint32_t view_down               : 1;
    uint32_t view_click              : 1;
    uint32_t view_touch_long         : 1;
    uint32_t view_button             : 1;
    uint32_t view_button_long        : 1;

    struct gui_view_on_event *on_event[EVENT_NUM_MAX];
    uint8_t on_event_num;

    uint8_t checksum;
} gui_view_t;

/* gui_view_descriptor start*/
typedef struct gui_view_descriptor
{
    const char *name;
    gui_view_t **pView;
    void (* design_cb)(gui_view_t *view);
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
 * @param x The X-axis coordinate relative to parent widget
 * @param y The Y-axis coordinate relative to parent widget
 * @param w Width
 * @param h Height
 * @return return the widget object pointer.
 *
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
 * @param name View descriptor's name that can used to find target view.
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
 * @brief Get current view pointer.
 *
 * @return return current view pointer.
 */
gui_view_t *gui_view_get_current_view(void);

#ifdef __cplusplus
}
#endif
#endif
