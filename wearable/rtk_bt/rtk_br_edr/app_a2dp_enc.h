/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_A2DP_ENC_H_
#define _APP_A2DP_ENC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define A2DP_ENC_WATER_LEVEL_H  8
#define A2DP_ENC_WATER_LEVEL_L  6

typedef struct t_a2dp_enc_mediahead
{
    struct t_a2dp_enc_mediahead     *p_next;
    uint16_t   payload_length;
    uint16_t   frame_num;
    uint8_t    p_data[0];
} T_A2DP_ENC_MEIDAHEAD;

void *a2dp_enc_audio_peek(int offset);
uint8_t a2dp_enc_audio_flush(uint16_t cnt);
uint8_t a2dp_enc_audio_reset(void);
bool a2dp_enc_data_rx(uint8_t *packet_pt, uint16_t packet_length, uint16_t frame_num);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_A2DP_ENC_H_ */
