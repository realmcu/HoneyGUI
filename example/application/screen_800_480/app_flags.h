/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _APP_FLAGS_H_
#define _APP_FLAGS_H_


/** @defgroup  PERIPH_Config Peripheral App Configuration
    * @brief This file is used to config app functions.
    * @{
    */
/*============================================================================*
 *                              Constants
 *============================================================================*/

/** @brief  Config APP LE link number */
#define APP_MAX_LINKS  1
/** @brief  Config DLPS: 0-Disable DLPS, 1-Enable DLPS */
#define F_BT_DLPS_EN                        0
/** @brief  Config ANCS Client: 0-Not built in, 1-Open ANCS client function */
#define F_BT_ANCS_CLIENT_SUPPORT            0
#define F_BT_ANCS_APP_FILTER                (F_BT_ANCS_CLIENT_SUPPORT & 1)
#define F_BT_ANCS_GET_APP_ATTR              (F_BT_ANCS_CLIENT_SUPPORT & 0)
/** @brief  Config ANCS Client debug log: 0-close, 1-open  */
#define F_BT_ANCS_CLIENT_DEBUG              (F_BT_ANCS_CLIENT_SUPPORT & 0)

/** @} */ /* End of group PERIPH_Config */
#endif
