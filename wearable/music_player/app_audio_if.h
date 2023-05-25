#ifndef _APP_AUDIO_IF_H_
#define _APP_AUDIO_IF_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "remote.h"
#include "app_audio_fs_interface.h"


T_APP_AUDIO_ERROR app_audio_play_by_name(uint8_t *file_name, uint16_t length);
T_APP_AUDIO_STATE app_audio_get_play_status(void);

void app_volume_up(void);
void app_volume_down(void);
uint8_t app_audio_get_volume(void);

uint16_t app_audio_start(void);
uint16_t app_audio_pause(void);
uint16_t app_audio_stop(void);
uint16_t app_audio_next(void);
uint16_t app_audio_prev(void);
void app_audio_mode_switch(uint8_t new_mode);
void app_audio_interface_init(void);


#endif //_APP_AUDIO_IF_H_
