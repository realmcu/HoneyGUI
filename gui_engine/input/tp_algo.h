/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file tp_algo.h
  * @brief touch panel algorithm
  * @details input: x,y,press,output: gesture
  * @author howie
  * @date 2020-11-18
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __TP_ALGO_H__
#define __TP_ALGO_H__


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"

/** @defgroup INPUT INPUT
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup INPUT_Exported_Types INPUT Exported Types
  * @brief
  * @{
  */



/** End of INPUT_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup INPUT_Exported_Constants INPUT Exported Constants
  * @brief
  * @{
  */


/** End of INPUT_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup INPUT_Exported_Macros INPUT Exported Macros
  * @brief
  * @{
  */
#define SAME_POINT_THR      5  //!< threshold for judging if finger don't move
#define HALF_SCREEN_SIZE    (320/2) //!< half of screen size 

/* Touch event */
#define GUI_TOUCH_EVENT_NONE     0                //!<Touch none 
#define GUI_TOUCH_EVENT_UP       1                //!< Touch up event 
#define GUI_TOUCH_EVENT_DOWN     2                //!< Touch down event 
/** End of INPUT_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup INPUT_Exported_Variables INPUT Exported Variables
  * @brief
  * @{
  */


/** End of INPUT_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup INPUT_Exported_Functions INPUT Exported Functions
  * @brief
  * @{
  */



/**
 * @brief algorithm process
 *
 * @param raw_data from gui_port_indev
 * @return struct touch_info*
 */
struct touch_info *tp_algo_process(struct gui_touch_data *raw_data);

/**
 * @brief read the output
 *
 * @return struct touch_info*
 */
struct touch_info *tp_get_info(void);

/** End of INPUT_Exported_Functions
  * @}
  */

/** End of INPUT
  * @}
  */




#endif


