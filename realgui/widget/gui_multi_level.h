/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_multi_level.h
  * @brief multi_level widget
  * @details A multi_level container widget
  * @author triton_yu@realsil.com.cn
  * @date 2024/06/03
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
#ifndef __GUI_MULTI_LEVEL_H__
#define __GUI_MULTI_LEVEL_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"
/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  multi_level structure */
typedef struct gui_multi_level
{
    gui_obj_t base;
    void (*ui_design)(gui_obj_t *);
    const char *group_name;
    bool hidden;
    uint8_t level;
    uint8_t index;
} gui_multi_level_t;
//_GUI_API_DEFINE(gui_multi_level_t)
typedef struct __gui_api_gui_multi_level_t
{
    /**
     * @brief Jump to a specific multi_level widget.
     * @param this One of the multi_level widgets in app.
     * @param level Specific level. 0,1,2...
     * @param index Index in this level. 0,1,2...
     *
     */
    void (*jump)(gui_multi_level_t *this, int levle, int index);

//_GUI_API_DECLARE(gui_multi_level_t)
} _gui_api_gui_multi_level_t; extern _gui_api_gui_multi_level_t _gui_api_for_gui_multi_level_t;
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
 * @brief Create a multi_level widget.
 * @param parent The father widget nested in.
 * @param widget_name The widget name.
 * @param x The X-axis coordinate.
 * @param y The Y-axis coordinate.
 * @param ui_design Ui_design function pointer.
 * @return Return the widget object pointer.
 *
 */
gui_multi_level_t *gui_multi_level_create(void *parent, const char *widget_name,
                                          void (*ui_design)(gui_obj_t *));
/**
 * @brief Jump to a specific multi_level widget.
 * @param this One of the multi_level widgets in app.
 * @param level Specific level. 0,1,2...
 * @param index Index in this level. 0,1,2...
 *
 */
void gui_multi_level_jump(gui_multi_level_t  *this, int level, int index);
/**
 * @brief Get a multi_level widget by group name.
 * @param group_name Multi_level widget group name.
 * @return Return the widget object pointer.
 *
 */
gui_multi_level_t *gui_multi_level_get_group(const char *group_name);

#ifdef __cplusplus
}
#endif

#endif
