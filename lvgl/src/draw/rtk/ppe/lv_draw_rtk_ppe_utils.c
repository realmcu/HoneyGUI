/**
 * @file lv_vglite_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_utils.h"
#include "../../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_color32_t lv_ppe_toABGR8888(lv_color_t color)
{
    lv_color32_t ABGR8888_color;
#if LV_COLOR_SIZE == 16
    {
        ABGR8888_color.ch.alpha = 0xFF;
        ABGR8888_color.ch.blue = (color.ch.red << 3) | (color.ch.red >> 2);
        ABGR8888_color.ch.green = (color.ch.green << 2) | (color.ch.green >> 4);
        ABGR8888_color.ch.red = (color.ch.blue << 3) | (color.ch.blue >> 2);
    }
#elif LV_COLOR_SIZE == 32
    {
        ABGR8888_color.ch.alpha = color.ch.alpha;
        ABGR8888_color.ch.blue = color.ch.red;
        ABGR8888_color.ch.green = color.ch.green;
        ABGR8888_color.ch.red = color.ch.blue;
    }
#endif
    return ABGR8888_color;
}

#endif /*LV_USE_GPU_RTK_VG_LITE*/
