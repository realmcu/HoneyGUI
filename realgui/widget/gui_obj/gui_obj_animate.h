/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj_animate.h
  * @brief  object animation
  * @details animation related functions and structures
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
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
#ifndef __GUI_OBJ_ANIMATE_H__
#define __GUI_OBJ_ANIMATE_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj_type.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_animate
{
    uint32_t dur;
    int repeat_count;
    uint32_t init_time_ms;
    uint32_t cur_time_ms;
    uint32_t current_repeat_count;
    uint32_t current_frame;
    uint32_t last_round;
    float progress_percent;
    float last_per;
    void (* callback)(void *p, void *this_widget, struct gui_animate *animate);
    void *p;
    const char *name;
    void *trigger_name;
    bool animate;
    bool init;
    bool Beginning_frame;
    bool end_frame;
} gui_animate_t;

typedef void (* gui_animate_callback_t)(void *p, void *this_widget, gui_animate_t *animate);

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief update animate on every frame.
 *
 * @param animate widget object pointer.
 * @param obj widget.
 */
void animate_frame_update(gui_animate_t *animate, gui_obj_t *obj);

/**
 * @brief set animate.
 *
 * @param animate pointer.
 * @param dur animation time cost in ms.
 * @param repeat_count rounds to repeat.
 * @param callback every frame callback.
 * @param p callback's parameter.
 */
gui_animate_t *gui_obj_set_animate(gui_animate_t *animate,
                                   uint32_t      dur,
                                   int           repeat_count,
                                   void         *callback,
                                   void         *p);

/**
 * @brief Retrieve the progress of an animation.
 *
 * This function returns the current progress of the specified animation.
 * The progress is represented as a floating-point value between 0.0 and 1.0,
 *
 * @param animation Pointer to the animation structure.
 * @return The current progress of the animation, ranging from 0.0 to 1.0.
 */
float gui_animation_get_progress(gui_animate_t *animation);

#ifdef __cplusplus
}
#endif

#endif