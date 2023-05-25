/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_sync.h"
#include "shm_api.h"
#include "trace.h"
#include "os_queue.h"
#include "gap_timer.h"
#include "os_mem.h"
#include "os_queue.h"
#include "app_a2dp_enc.h"

T_OS_QUEUE a2dp_enc_queue;

void *a2dp_enc_audio_peek(int offset)
{
    void *p_media_dspkt = os_queue_peek(&a2dp_enc_queue, offset);;
    return p_media_dspkt;
}

uint8_t a2dp_enc_audio_flush(uint16_t cnt)
{
    uint16_t i;
    T_A2DP_ENC_MEIDAHEAD *p_media_head;
    AUDIO_PRINT_TRACE1("media_buffer_a2dp_enc_flush: %d", cnt);
    if (cnt > a2dp_enc_queue.count)
    {
        cnt = a2dp_enc_queue.count;
    }
    for (i = 0; i < cnt; i++)
    {
        p_media_head = (T_A2DP_ENC_MEIDAHEAD *)os_queue_out(&a2dp_enc_queue);
        os_mem_free(p_media_head);
    }
    return 0;
}

bool a2dp_enc_data_rx(uint8_t *packet_pt, uint16_t packet_length, uint16_t frame_num)
{
    T_A2DP_ENC_MEIDAHEAD media_head;
    T_A2DP_ENC_MEIDAHEAD *mediapacket;

    if (packet_pt == NULL)
    {
        AUDIO_PRINT_ERROR0("a2dp_enc_data_rx pointer null");
        return false;
    }
    media_head.p_next = NULL;
    media_head.payload_length = packet_length;
    media_head.frame_num = frame_num;
    mediapacket = (T_A2DP_ENC_MEIDAHEAD *)os_mem_alloc(RAM_TYPE_DSPSHARE,
                                                       sizeof(T_A2DP_ENC_MEIDAHEAD) + packet_length);

    if (mediapacket == NULL)
    {
        AUDIO_PRINT_ERROR0("a2dp_enc_data_rx get buffer error");
        return false;
    }
    memcpy(mediapacket, &media_head, sizeof(T_A2DP_ENC_MEIDAHEAD));
    memcpy((uint8_t *)mediapacket + sizeof(T_A2DP_ENC_MEIDAHEAD), packet_pt, packet_length);

    os_queue_in(&a2dp_enc_queue, mediapacket);

    return true;
}
