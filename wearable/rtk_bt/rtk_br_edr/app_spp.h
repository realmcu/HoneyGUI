/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_SPP_H_
#define _APP_SPP_H_

#include "btm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_SPP App Spp
  * @brief App Spp
  * @{
  */

#define APP_SPP_DEFAULT_MTU_SIZE 1012
#define APP_SPP_DEFAULT_CREDITS 7

/**
    * @brief  spp module init
    * @param  void
    * @return void
    */
void app_spp_init(void);
bool spp_demo_role_get(void);
void spp_demo_role_set(bool role);


/** End of APP_SPP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_SPP_H_ */
