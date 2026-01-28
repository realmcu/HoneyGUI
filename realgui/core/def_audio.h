/**
 * @file def_fb.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __DEF_AUDIO_H__
#define __DEF_AUDIO_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "def_type.h"


typedef struct gui_audiodev
{
    int freq;
    uint8_t channels;

    int (*open)(void *info);
    void (*decode_chunk)(const uint8_t *data, size_t len);




} gui_audiodev_t;

#ifdef __cplusplus
}
#endif

#endif

