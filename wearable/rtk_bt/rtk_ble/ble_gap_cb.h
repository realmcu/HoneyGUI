/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief     This file handles BLE peripheral application routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#ifndef _BLE_GAP_CB_APP__
#define _BLE_GAP_CB_APP__

#ifdef __cplusplus
extern "C" {
#endif
#include <gap.h>

T_APP_RESULT le_gap_callback(uint8_t cb_type, void *p_cb_data);


/** End of PERIPH_APP
* @}
*/


#ifdef __cplusplus
}
#endif

#endif

