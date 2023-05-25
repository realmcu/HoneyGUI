/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      ams.c
   * @brief     This file handles AMS Client routines.
   * @author
   * @date      2018-12-03
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <trace.h>
#include <os_msg.h>
#include <os_mem.h>
#include <ams.h>
#include <ams_client.h>
#include "hub_interaction.h"
#include "communicate_protocol.h"
#include "stdlib.h"
#include "module_font.h"
#include "os_sync.h"
#include "hub_task.h"
#include "app_flash_config.h"

extern void *evt_queue_handle;  //!< Event queue handle
extern void *io_queue_handle;   //!< IO queue handle

void *ams_queue_handle;
T_AMS_ENTITY_TRACK_ATTR ams_entity_track = {0};
T_CLIENT_ID      ams_client; /**< ams client id*/

static void ams_send_msg_to_app(uint8_t conn_id)
{
    T_IO_MSG io_msg;
    uint8_t event = EVENT_IO_TO_APP;
    io_msg.type = IO_MSG_TYPE_WRISTBNAD;
    io_msg.subtype = IO_MSG_TYPE_AMS;
    io_msg.u.param = conn_id;

    if (os_msg_send(io_queue_handle, &io_msg, 0) == false)
    {
        GAP_PRINT_ERROR0("ams_send_msg_to_app fail1");
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        GAP_PRINT_ERROR0("ams_send_msg_to_app fail2");
    }
}

void ams_send_msg(uint8_t conn_id, T_AMS_MSG *ams_msg)
{
    uint32_t msg_num;
    os_msg_queue_peek(ams_queue_handle, &msg_num);
    if (msg_num == 0)
    {
        switch (ams_msg->type)
        {
        case AMS_MSG_TYPE_REMOTE_CMD:
            {
                if (ams_write_remote_cmd(conn_id, ams_msg->remote_cmd_id) == true)
                {
                    return;
                }
            }
            break;
        case AMS_MSG_ENTITY_UPDATE_CMD:
            {
                if (ams_write_entity_upd_cmd(conn_id, ams_msg->param, ams_msg->param_len) == true)
                {
                    return;
                }
            }
            break;
        default:
            break;
        }
    }
    if (os_msg_send(ams_queue_handle, ams_msg, 0) == false)
    {
        APP_PRINT_ERROR0("ams send msg: discard, ams msg queue is full");
    }
}

void ams_handle_msg(T_IO_MSG *p_io_msg)
{
    uint8_t conn_id = p_io_msg->u.param;
    T_AMS_MSG ams_msg;
    uint32_t msg_num;
    os_msg_queue_peek(ams_queue_handle, &msg_num);
    if (os_msg_recv(ams_queue_handle, &ams_msg, 0) == false)
    {
        APP_PRINT_INFO1("ams_handle_msg: conn_id 0x%x os_msg_recv null(no msg in ams queue)", conn_id);
        return;
    }
    if (ams_msg.type == AMS_MSG_TYPE_REMOTE_CMD)
    {
        ams_write_remote_cmd(conn_id, ams_msg.remote_cmd_id);
    }
    else if (ams_msg.type == AMS_MSG_ENTITY_UPDATE_CMD)
    {
        ams_write_entity_upd_cmd(conn_id, ams_msg.param, ams_msg.param_len);
    }
}

static void AMSDataBlockInit(void)
{
    ams_entity_track.artist_addr = AMS_ENTITY_UPDATE_ADDRESS;
    ams_entity_track.title_addr = AMS_ENTITY_UPDATE_ADDRESS + FLASH_SECTOR_SIZE;
    ams_entity_track.player_status = PLAYBACKSTATEPAUSED;
    uint8_t prev_bp_lv = 0;
    flash_sw_protect_unlock_by_addr_locked(AMS_ENTITY_UPDATE_ADDRESS, &prev_bp_lv);
    uint32_t s;
    s = os_lock();
    flash_erase_locked(FLASH_ERASE_SECTOR, AMS_ENTITY_UPDATE_ADDRESS);
    flash_erase_locked(FLASH_ERASE_SECTOR, AMS_ENTITY_UPDATE_ADDRESS + FLASH_SECTOR_SIZE);
    os_unlock(s);
    flash_set_block_protect_locked(prev_bp_lv);
}

static void save_ams_entity_update(T_AMS_ENTITY_TRACK_ATTR_ID ams_track_attr_id, uint8_t *p_data,
                                   uint16_t len)
{
    uint16_t *p_buf = NULL;
    p_buf = malloc(128 * sizeof(uint16_t));
    if (p_buf == NULL)
    {
        DBG_DIRECT("save_headset_list fail");
        while (1);
    }

    uint32_t len_unicode;
    len_unicode = utf8_to_unicode(p_data, len, p_buf, 128);

    if (ams_track_attr_id == TRACK_ATTR_ID_ARTIST)
    {
        if (ams_entity_track.artist_addr >= (AMS_ENTITY_UPDATE_ADDRESS + FLASH_SECTOR_SIZE))
        {
            os_mutex_take(flash_mutex_handle, 500);
            flash_erase_locked(FLASH_ERASE_SECTOR, AMS_ENTITY_UPDATE_ADDRESS);
            os_mutex_give(flash_mutex_handle);
        }
        ams_entity_track.artist_len = len_unicode;
        ams_entity_track.artist_addr += 2 * 128;
        os_mutex_take(flash_mutex_handle, 500);
        flash_write_locked(ams_entity_track.artist_addr, len_unicode * 2, (uint8_t *)p_buf);
        os_mutex_give(flash_mutex_handle);
    }
    else if (ams_track_attr_id == TRACK_ATTR_ID_TITLE)
    {
        if (ams_entity_track.title_addr >= (AMS_ENTITY_UPDATE_ADDRESS + 2 * FLASH_SECTOR_SIZE))
        {
            os_mutex_take(flash_mutex_handle, 500);
            flash_erase_locked(FLASH_ERASE_SECTOR, AMS_ENTITY_UPDATE_ADDRESS + FLASH_SECTOR_SIZE);
            os_mutex_give(flash_mutex_handle);
        }
        ams_entity_track.title_len = len_unicode;
        ams_entity_track.title_addr += 2 * 128;
        os_mutex_take(flash_mutex_handle, 500);
        flash_write_locked(ams_entity_track.title_addr, len_unicode * 2, (uint8_t *)p_buf);
        os_mutex_give(flash_mutex_handle);
    }

    if (p_buf)
    {
        free(p_buf);
        p_buf = NULL;
    }
}

static void ams_parse_entity_update(uint8_t *p_data, uint16_t len)
{
    T_AMS_ENTITY_UPDATE *p_parse_data = (T_AMS_ENTITY_UPDATE *)p_data;
    switch (p_parse_data->entity_id)
    {
    case ENTITY_ID_PLAYER:
        {
            APP_PRINT_INFO0("AMS ENTITY_ID_PLAYER");
            switch (p_parse_data->entity_attr)
            {
            case PLAYER_ATTR_IDPLAYER_ATTR_ID_VOLUME_NAME:
                {
                    APP_PRINT_INFO0("AMS PLAYER_ATTR_IDPLAYER_ATTR_ID_VOLUME_NAME");
                }
                break;
            case PLAYER_ATTR_ID_PLAYBACK_INFO:
                {
                    APP_PRINT_INFO0("AMS PLAYER_ATTR_ID_PLAYBACK_INFO");
                    ams_entity_track.player_status = *(T_AMS_PLAYER_STATUS *)(p_data + 3);
                    APP_PRINT_INFO1("AMS player_status = %d", ams_entity_track.player_status);
                }
                break;
            case PLAYER_ATTR_ID_VOLUME:
                {
                    *(uint8_t *)(p_data + len) = '\0';
                    APP_PRINT_INFO1("AMS PLAYER_ATTR_ID_VOLUME = %s", TRACE_STRING((uint8_t *)(p_data + 3)));

                }
                break;
            default:
                break;
            }
        }
        break;
    case ENTITY_ID_QUEUE:
        {
            APP_PRINT_INFO0("AMS ENTITY_ID_QUEUE");
        }
        break;
    case ENTITY_ID_TRACK:
        {
            switch (p_parse_data->entity_attr)
            {
            case TRACK_ATTR_ID_ARTIST:
                {
                    APP_PRINT_INFO0("AMS TRACK_ATTR_ID_ARTIST");
                    ams_entity_track.artist_len = len - 3;
                    save_ams_entity_update(TRACK_ATTR_ID_ARTIST, (uint8_t *)p_data + 3, ams_entity_track.artist_len);
                }
                break;
            case TRACK_ATTR_ID_ALBUM:
                {
                    APP_PRINT_INFO0("AMS TRACK_ATTR_ID_ALBUM");
                }
                break;
            case TRACK_ATTR_ID_TITLE:
                {
                    APP_PRINT_INFO0("AMS TRACK_ATTR_ID_TITLE");
                    ams_entity_track.title_len = len - 3;
                    save_ams_entity_update(TRACK_ATTR_ID_TITLE, (uint8_t *)p_data + 3, ams_entity_track.title_len);
                }
                break;
            case TRACK_ATTR_ID_DURATION:
                {
                    APP_PRINT_INFO0("AMS TRACK_ATTR_ID_DURATION");
                }
                break;
            default:
                break;
            }
        }
    default:
        break;
    }
}

static void ams_queue_init(void)
{
    if (os_msg_queue_create(&ams_queue_handle, AMS_MSG_QUEUE_NUM,
                            sizeof(T_AMS_MSG)) == false)
    {
        DBG_DIRECT("ams_init create queue failed");
    }
}

T_APP_RESULT ams_client_cb(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_AMS_CB_DATA *p_cb_data = (T_AMS_CB_DATA *)p_data;

    switch (p_cb_data->cb_type)
    {
    case AMS_CLIENT_CB_TYPE_DISC_STATE:
        switch (p_cb_data->cb_content.disc_state)
        {
        case AMS_DISC_DONE:
            APP_PRINT_INFO0("ams_client_cb: discover procedure done.");
            ams_subscribe_remote_cmd(conn_id, true);
            break;
        case AMS_DISC_FAILED:
            APP_PRINT_ERROR0("ams_client_cb: discover request failed.");
            break;
        default:
            break;
        }
        break;

    case AMS_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
        switch (p_cb_data->cb_content.notify_data.type)
        {
        case AMS_NOTIFY_FROM_REMOTE_CMD:
            APP_PRINT_INFO2("AMS_NOTIFY_FROM_REMOTE_CMD: data[%d]: %b",
                            p_cb_data->cb_content.notify_data.value_size,
                            TRACE_BINARY(p_cb_data->cb_content.notify_data.value_size,
                                         p_cb_data->cb_content.notify_data.p_value));
            break;
        case AMS_NOTIFY_FROM_ENTITY_UPD:
            {
                APP_PRINT_INFO2("AMS_NOTIFY_FROM_ENTITY_UPD: data[%d]: %b",
                                p_cb_data->cb_content.notify_data.value_size,
                                TRACE_BINARY(p_cb_data->cb_content.notify_data.value_size,
                                             p_cb_data->cb_content.notify_data.p_value));
                uint32_t msg_num;
                os_msg_queue_peek(ams_queue_handle, &msg_num);
                if (msg_num)
                {
                    ams_send_msg_to_app(conn_id);
                }
                ams_parse_entity_update(p_cb_data->cb_content.notify_data.p_value,
                                        p_cb_data->cb_content.notify_data.value_size);
            }
            break;
        default:
            break;
        }
        break;

    case AMS_CLIENT_CB_TYPE_READ_RESULT:
        switch (p_cb_data->cb_content.read_data.type)
        {
        case AMS_READ_FROM_ENTITY_UPD:
            APP_PRINT_INFO3("AMS_READ_FROM_ENTITY_UPD: cause 0x%x, data[%d]: %b",
                            p_cb_data->cb_content.read_data.cause,
                            p_cb_data->cb_content.read_data.value_size,
                            TRACE_BINARY(p_cb_data->cb_content.read_data.value_size, p_cb_data->cb_content.read_data.p_value));
            break;
        default:
            break;
        }
        break;

    case AMS_CLIENT_CB_TYPE_WRITE_RESULT:
        {
            if (p_cb_data->cb_content.write_result.cause != ATT_SUCCESS)
            {
                APP_PRINT_ERROR1("AMS_CLIENT_CB_TYPE_WRITE_RESULT: Failed, cause 0x%x",
                                 p_cb_data->cb_content.write_result.cause);
            }
            switch (p_cb_data->cb_content.write_result.type)
            {
            case AMS_WRITE_REMOTE_CMD_NOTIFY_ENABLE:
                APP_PRINT_INFO0("AMS_WRITE_REMOTE_CMD_NOTIFY_ENABLE");
                ams_subscribe_entity_upd(conn_id, true);
                break;

            case AMS_WRITE_REMOTE_CMD_NOTIFY_DISABLE:
                APP_PRINT_INFO0("AMS_WRITE_REMOTE_CMD_NOTIFY_DISABLE");
                break;

            case AMS_WRITE_ENTITY_UPD_NOTIFY_ENABLE:
                {
                    APP_PRINT_INFO0("AMS_WRITE_ENTITY_UPD_NOTIFY_ENABLE");
                    T_AMS_MSG ams_msg;
                    static uint8_t cmd[3];
                    cmd[0] = ENTITY_ID_TRACK;
                    cmd[1] = TRACK_ATTR_ID_ARTIST;
                    cmd[2] = TRACK_ATTR_ID_TITLE;
                    ams_msg.type = AMS_MSG_ENTITY_UPDATE_CMD;
                    ams_msg.param = cmd;
                    ams_msg.param_len = 3;
                    ams_send_msg(conn_id, &ams_msg);

                    cmd[0] = ENTITY_ID_PLAYER;
                    cmd[1] = PLAYER_ATTR_ID_PLAYBACK_INFO;
                    cmd[2] = PLAYER_ATTR_ID_VOLUME;
                    ams_msg.type = AMS_MSG_ENTITY_UPDATE_CMD;
                    ams_msg.param = cmd;
                    ams_msg.param_len = 3;
                    ams_send_msg(conn_id, &ams_msg);
                }
                break;
            case AMS_WRITE_ENTITY_UPD_NOTIFY_DISABLE:
                APP_PRINT_INFO0("AMS_WRITE_ENTITY_UPD_NOTIFY_DISABLE");
                break;

            case AMS_WRITE_ENTITY_ATTR_VALUE:
                APP_PRINT_INFO0("AMS_WRITE_ENTITY_ATTR_VALUE");
                break;

            case AMS_WRITE_REMOTE_CMD_VALUE:
                {
                    APP_PRINT_INFO0("AMS_WRITE_REMOTE_CMD_VALUE");
                    uint32_t msg_num;
                    os_msg_queue_peek(ams_queue_handle, &msg_num);
                    if (msg_num)
                    {
                        ams_send_msg_to_app(conn_id);
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case AMS_CLIENT_CB_TYPE_DISCONNECT_INFO:
        {
            APP_PRINT_INFO1("AMS_CLIENT_CB_TYPE_DISCONNECT_INFO: conn_id = 0x%x", conn_id);
            T_AMS_MSG ams_msg;
            /*release msg queue*/
            while (os_msg_recv(ams_queue_handle, &ams_msg, 0));
        }
        break;

    default:
        break;
    }
    return result;
}

void ams_init(uint8_t link_num)
{
    AMSDataBlockInit();
    ams_queue_init();
    ams_client = ams_add_client(ams_client_cb, link_num);
}
