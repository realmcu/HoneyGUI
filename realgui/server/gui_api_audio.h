/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_audio.h
  * @brief Application Programming Interface for UI
  * @details Display Device
  * @author roy@realsil.com.cn
  * @date 2026/01/27
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_AUDIO_H__
#define __GUI_API_AUDIO_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "guidef.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/



/*============================================================================*
 *                         Functions
 *============================================================================*/

void gui_audio_info_register(gui_audiodev_t *info);

gui_audiodev_t *gui_get_audio(void);

int gui_audio_open(void *info);

void gui_audio_decode_chunk(const uint8_t *data, size_t len);

void gui_port_audio_init(void);

#ifdef __cplusplus
}
#endif
#endif
