/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      datebse.h
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef DP_APP_H
#define DP_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"


typedef struct
{
    uint8_t                     local_batt_level;           /**< local battery level */
    uint8_t                     remote_batt_level;          /**< remote battery level */
    uint8_t                     factory_addr[6];            /**< local factory address */
} T_APP_DB;


extern T_APP_DB app_db;


#ifdef __cplusplus
}
#endif

#endif /* DP_APP_H */
