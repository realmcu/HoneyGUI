/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      sdcard_fs_app.h
* @brief     header file of sdcard file system application driver.
* @details
* @author   elliot chen
* @date     2019-1-14
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _APP_PLAYBACK_H_
#define _APP_PLAYBACK_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "rtl876x.h"
#include "mp3_parser.h"

#define PLAYBACK_TRACK_STATE_CLEAR                  0xFF

extern Mp3Hdl_t g_curr_song;

typedef struct
{
    uint8_t                     pkt_num;
    uint16_t                    seq_num;
    uint32_t                    file_offset;
} T_PLAYBACK_PUT_DATA_INFO;

/** Little Endian array to uint16 */
#define PLAYBACK_LE_ARRAY_TO_UINT16(u16, a)  {          \
        u16 = ((uint16_t)(*(a + 0)) << 0) +             \
              ((uint16_t)(*(a + 1)) << 8);              \
    }

/** Little Endian array to uint32 */
#define PLAYBACK_LE_ARRAY_TO_UINT32(u32, a) {           \
        u32 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16) +            \
              ((uint32_t)(*(a + 3)) << 24);             \
    }


/** Little Endian uint16 to array */
#define PLAYBACK_LE_UINT16_TO_ARRAY(a, u16)  {          \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 0);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 8);  \
    }

/** Little Endian uint32 to array */
#define PLAYBACK_LE_UINT32_TO_ARRAY(a, u32) {           \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >> 24); \
    }

typedef enum
{
    PLAYBACK_QUIT       = 0x00,
    PLAYBACK_ENTER      = 0x01,
    PLAYBACK_UPDATA     = 0x02,
} T_PLAYBACK_MODE;

typedef enum
{
    PLAYBACK_STOP = 0x00,
    PLAYBACK_START,
} T_PLAYBACK_PAUSE;

/**
  * @brief Initialize.
  * @param   No parameter.
  * @return  execution status
  */
uint8_t app_playback_start(void);
uint8_t app_playback_stop(void);
uint8_t app_playback_close_flie(void);

void app_playback_mode_init(void);

void app_playback_volume_up(void);
void app_playback_volume_down(void);
void app_playback_volume_set(void);

void app_playback_get_audio_track_state_handle(void);
static void app_playback_track_state_changed_handle(void);

void app_playback_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_FS_APP_H */

///@endcond

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/
