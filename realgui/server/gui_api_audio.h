/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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
