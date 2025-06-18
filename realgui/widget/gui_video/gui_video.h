/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_video.h
  * @brief video widget
  * @details video widget
  * @author roy@realsil.com.cn
  * @date 2024/5/15
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI___VIDEO___H__
#define __GUI___VIDEO___H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_img.h"
#include "gui_fb.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

#define GUI_VIDEO_REPEAT_INFINITE      0xFFFFFFFF

typedef enum
{
    GUI_VIDEO_EVENT_FRAME_UPDATE,
    GUI_VIDEO_EVENT_PLAY,
    GUI_VIDEO_EVENT_PAUSE,
    GUI_VIDEO_EVENT_STOP,
    GUI_VIDEO_EVENT_END

} GUI_VIDEO_EVENT;

typedef enum
{
    GUI_VIDEO_STATE_INIT,
    GUI_VIDEO_STATE_PLAYING,
    GUI_VIDEO_STATE_PAUSE,
    GUI_VIDEO_STATE_STOP,

} GUI_VIDEO_STATE;

typedef enum
{
    GUI_IMG_LIVE_SUCCESSED,
    // GUI_IMG_LIVE_s,

} GUI_VIDEO_RES;


typedef enum
{
    VIDEO_TYPE_NULL,
    VIDEO_TYPE_MJPEG,
    VIDEO_TYPE_H264,

} GUI_VIDEO_TYPE;



/** @brief  stb img widget information structure */
typedef struct
{
    gui_obj_t base;
    gui_img_t *img;

    void *data;                  // mem address or filesystem path
    uint32_t num_frame;
    uint8_t **array;             // mjpeg: jpeg address array, need free when destory widget
    gui_rgb_data_head_t header;  // for image prepare

    uint8_t *frame_buff;
    uint8_t *frame_buff_raw;
    float fps;
    uint32_t frame_cur;
    uint32_t frame_last;
    uint8_t img_type;
    uint8_t src_mode;

    uint8_t state;
    int32_t repeat_cnt;


    uint8_t rgb_type;           // to define RGB type of decoded img (not used yet)
    uint32_t frame_ready: 1;

} gui_video_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define IMG_LIVE_FPS (25)  // default 25 fps, 40 ms
// #define IMG_LIVE_TJPGDEC
// #define USE_JPU_DECODER


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

void gui_video_set_frame_rate(gui_video_t *this, float fps);

void gui_video_set_state(gui_video_t *this, GUI_VIDEO_STATE state);

void gui_video_set_repeat_count(gui_video_t *this, int32_t cnt);

GUI_VIDEO_STATE gui_video_get_state(gui_video_t *this);



void gui_video_refresh_size(gui_video_t *this);

void gui_video_refresh_type(gui_video_t *this);

/**
 * @brief Creat a image live widget with buffer.
 * @note This widget is use to display live image which can be play.
 * @note The data of image can be static char or data transformed by BLE.
 * @param parent The father widget .
 * @param name The widget's name.
 * @param addr The data address of image.
 * @param x The X-axis coordinate .
 * @param y The Y-axis coordinate .
 * @param w The widget width .
 * @param h The widget hight .
 * @return gui_video_t*
 */
gui_video_t *gui_video_create_from_mem(void           *parent,
                                       const char     *name,
                                       void           *addr,
                                       int16_t         x,
                                       int16_t         y,
                                       int16_t         w,
                                       int16_t         h);



#ifdef __cplusplus
}
#endif

#endif
