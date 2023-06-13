/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     miwear_service.c
* @brief    MI wear service profile source file.
* @details  for mi wear.
* @author   wh
* @date
* @version
*********************************************************************************************************
*/

/* Define to prevent recursive inclusion */
#ifndef _MIWEAR_SERVICE_H_
#define _MIWEAR_SERVICE_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include <profile_server.h>
#include <miwear_type.h>


T_SERVER_ID miwear_ble_service_add_service(void *p_func);

#ifdef __cplusplus
}
#endif

#endif /* _MIWEAR_SERVICE_H_ */
