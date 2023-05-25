/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_UTIL_H_
#define _APP_UTIL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_UTIL App Util
  * @brief App Util
  * @{
  */


/**
    * @brief  App calculate transfer payload checksum.
    * @param  dataPtr payload ptr
    * @param  len payload length
    * @return check_sum
    */
uint8_t app_util_calc_checksum(uint8_t *dataPtr, uint16_t len);


/** End of APP_UTIL
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_UTIL_H_ */
