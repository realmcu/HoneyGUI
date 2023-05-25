/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_HFP_H_
#define _APP_HFP_H_

#include "bt_hfp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_HFP App Hfp
  * @brief this file handle hfp profile related process
  * @{
  */

typedef enum t_app_hfp_call_status
{
    APP_HFP_CALL_IDLE                              = 0x00,
    APP_HFP_VOICE_ACTIVATION_ONGOING               = 0x01,
    APP_HFP_CALL_INCOMING                          = 0x02,
    APP_HFP_CALL_OUTGOING                          = 0x03,
    APP_HFP_CALL_ACTIVE                            = 0x04,
    APP_HFP_CALL_ACTIVE_WITH_CALL_WAITING          = 0x05,
    APP_HFP_CALL_ACTIVE_WITH_CALL_HELD             = 0x06,
    APP_HFP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT   = 0x07,
    APP_HFP_MULTILINK_CALL_ACTIVE_WITH_CALL_HOLD   = 0x08,
} T_APP_HFP_CALL_STATUS;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_HFP_Exported_Functions App Hfp Functions
    * @{
    */
/**
    * @brief  hfp module init.
    * @param  void
    * @return void
    */
void app_hfp_init(void);

/**
    * @brief  get current call status.
    * @param  void
    * @return @ref T_APP_HFP_CALL_STATUS
    */
T_APP_HFP_CALL_STATUS app_hfp_get_call_status(void);

/**
    * @brief  set current call status.
    * @param  T_APP_HFP_CALL_STATUS
    * @return void
    */
void app_hfp_set_call_status(T_APP_HFP_CALL_STATUS call_status);

/**
    * @brief  get active hfp connection br link id.
    * @param  void
    * @return link id
    */
uint8_t app_hfp_get_active_hf_index(void);

/**
    * @brief  set active hfp connection br link .
    * @param  bd_addr active link bt device address
    * @return link id
    */
bool app_hfp_set_active_hf_index(uint8_t *bd_addr);

/**
    * @brief  get sco connect status.
    * @param  void
    * @return true for sco connected
    */
bool app_hfp_sco_is_connected(void);

/**
    * @brief  control hfp mic mute.
    * @param  void
    * @return void
    */
void app_hfp_mute_ctrl(void);

/**
    * @brief  hfp volume up.
    * @param  void
    * @return void
    */
void app_hfp_volume_up(void);

/**
    * @brief  hfp volume down.
    * @param  void
    * @return void
    */
void app_hfp_volume_down(void);

/** @} */ /* End of group APP_HFP_Exported_Functions */
/** End of APP_HFP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HFP_H_ */
