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
#ifndef DP_BLE_INFO_H
#define DP_BLE_INFO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "dp_sports_refs.h"
#include "app_le_link_util.h"


typedef struct
{
    T_APP_LE_LINK               le_link[MAX_BLE_LINK_NUM];
} T_LE_DB;

extern T_LE_DB le_db;

#ifdef __cplusplus
}
#endif

#endif /* DP_CONFIG_H */
