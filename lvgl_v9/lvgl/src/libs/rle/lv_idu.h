/**
 * @file lv_idu.h
 */

#ifndef LV_IDU_H
#define LV_IDU_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_RTK_IDU

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


void lv_rtk_idu_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RTK_IDU*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_IDU_H*/
