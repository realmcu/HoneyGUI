/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ancs.h
* @brief     ancs
* @details   ancs
* @author    ranhui
* @date      2015-03-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _AMS_H__
#define _AMS_H__

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <gap_le.h>
#include <app_msg.h>
#include <ams_client.h>

#define AMS_MSG_QUEUE_NUM   5  //!< AMS message queue size

typedef enum
{
    AMS_MSG_TYPE_REMOTE_CMD,
    AMS_MSG_ENTITY_UPDATE_CMD,
    AMS_MSG_TYPE_WRITE_ENTITY_ATTR,
    AMS_MSG_TYPE_READ_ENTITY_ATTR,
} T_AMS_MSG_TYPE;

typedef enum
{
    PLAYBACKSTATEPAUSED = '0',
    PLAYBACKSTATEPLAYING = '1',
    PLAYBACKSTATEREWINDING = '2',
    PLAYBACKSTATEPFASTFORWARDING = '3',
} T_AMS_PLAYER_STATUS;

typedef struct
{
    T_AMS_MSG_TYPE type;
    T_AMS_REMOTE_CMD_ID remote_cmd_id;
    uint8_t *param;
    uint8_t param_len;
} T_AMS_MSG;

typedef struct
{
    uint8_t artist_len;
    uint32_t artist_addr;
    uint8_t title_len;
    uint32_t title_addr;
    T_AMS_PLAYER_STATUS player_status;
    uint8_t volume;
} T_AMS_ENTITY_TRACK_ATTR;

typedef union
{
    uint8_t data;
    struct
    {
        uint8_t EntityUpdateFlagTruncated : 1;
        uint8_t Reserved : 7;
    } field;
} T_AMS_ENTITYUPDATEFLAGS;

typedef struct
{
    uint8_t entity_id;
    uint8_t entity_attr;
    T_AMS_ENTITYUPDATEFLAGS flag;
} T_AMS_ENTITY_UPDATE;

void ams_send_msg(uint8_t conn_id, T_AMS_MSG *ams_msg);
void ams_handle_msg(T_IO_MSG *p_io_msg);
void ams_init(uint8_t link_num);

extern T_AMS_ENTITY_TRACK_ATTR ams_entity_track;

#ifdef __cplusplus
}
#endif

#endif

