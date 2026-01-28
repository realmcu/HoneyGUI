/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_audio.c
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
 *                        Header Files
 *============================================================================*/

#include "guidef.h"
#include "gui_api_audio.h"
#include "gui_api_os.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_audiodev *audio = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_audio_info_register(gui_audiodev_t *info)
{
    audio = info;

}


gui_audiodev_t *gui_get_audio(void)
{
    return audio;
}

int gui_audio_open(void *info)
{
    if (audio && audio->open)
    {
        return audio->open(info);
    }
    return -1;
}

void gui_audio_decode_chunk(const uint8_t *data, size_t len)
{
    if (audio && audio->decode_chunk)
    {
        audio->decode_chunk(data, len);
    }
}
