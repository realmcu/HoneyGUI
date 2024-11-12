/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_PX_H__
#define __GUI_PX_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "PainterEngine.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  structure */
typedef struct gui_px
{
    gui_obj_t base;
    void (*px_main)(void);
    px_dword elapsed;
    px_dword timelasttime;
    void *user_data;
    PX_Object *px_root;
    px_surface RenderSurface;
    px_memorypool   mem_static;
    px_memorypool   mem_dynamic;
    px_memorypool   *mp_static;
    px_memorypool   *mp_dynamic;
} gui_px_t;


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
 * @brief
 *
 * @param parent
 * @param name
 * @param addr
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_px_t*
 */
gui_px_t *gui_px_create(void        *parent,
                        const char  *name,
                        void (*main_cb)(gui_px_t *this),
                        int16_t      x,
                        int16_t      y,
                        int16_t      w,
                        int16_t      h);


#ifdef __cplusplus
}
#endif

#endif
