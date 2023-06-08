/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_bond.h"
#include "app_cfg.h"
#include "app_br_link_util.h"
#include "app_avrcp.h"
#include "dp_br_info.h"
#include "audio_volume.h"
#include "app_mmi.h"
#include "bt_task.h"
#include "app_audio_if.h"
#include "audio_track.h"
#include "watch_msg.h"


static void app_avrcp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_AVRCP_CONN_IND:
        {
            p_link = app_find_br_link(param->avrcp_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                bt_avrcp_connect_cfm(p_link->bd_addr, true);
            }
        }
        break;

    case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                p_link = app_find_br_link(param->avrcp_absolute_volume_set.bd_addr);
                if (p_link != NULL)
                {
                    uint8_t pair_idx;
                    if (bt_bond_index_get(p_link->bd_addr, &pair_idx) == false)
                    {
                        APP_PRINT_ERROR0("abs volume set: find active a2dp pair_index fail");
                        return;
                    }
                    app_cfg_nv.audio_gain_level[pair_idx] = (param->avrcp_absolute_volume_set.volume *
                                                             app_cfg_volume.playback_volume_max + 0x7F / 2) / 0x7F;
                    APP_PRINT_INFO2("abs volume set, pair idx = %d, gain level = %d", pair_idx,
                                    app_cfg_nv.audio_gain_level[pair_idx]);
                    if (p_link->a2dp_track_handle)
                    {
                        audio_track_volume_out_set(p_link->a2dp_track_handle, app_cfg_nv.audio_gain_level[pair_idx]);
                    }
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                p_link = app_find_br_link(param->avrcp_play_status_changed.bd_addr);
                if (p_link != NULL)
                {
                    APP_PRINT_INFO1("BT_EVENT_AVRCP_PLAY_STATUS_CHANGED play status = 0x%x",
                                    param->avrcp_play_status_changed.play_status);
                    if ((param->avrcp_play_status_changed.play_status != BT_AVRCP_PLAY_STATUS_ERROR) && \
                        (p_link->avrcp_play_status != param->avrcp_play_status_changed.play_status))
                    {

                        p_link->avrcp_play_status = param->avrcp_play_status_changed.play_status;
                    }
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SNK)
            {
                p_link = app_find_br_link(param->avrcp_play_status_rsp.bd_addr);
                if (p_link != NULL)
                {
                    APP_PRINT_INFO1("BT_EVENT_AVRCP_PLAY_STATUS_RSP play status = 0x%x",
                                    param->avrcp_play_status_rsp.play_status);
                    if (param->avrcp_play_status_rsp.play_status != BT_AVRCP_PLAY_STATUS_ERROR && \
                        (p_link->avrcp_play_status != param->avrcp_play_status_rsp.play_status))
                    {
                        p_link->avrcp_play_status = param->avrcp_play_status_rsp.play_status;
                    }
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {

        }
        break;

    case BT_EVENT_AVRCP_VOLUME_CHANGED:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
                p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
                if (p_link != NULL)
                {
                    uint8_t volume = (param->avrcp_volume_changed.volume *
                                      app_cfg_volume.playback_volume_max + 0x7F / 2) / 0x7F;

//20230607          app_a2dp_set_src_volume(volume);

                    if (br_db.audio_info.playback_muted)
                    {
                        br_db.audio_info.playback_muted = false;
                    }
                }
            }
        }
        break;

    case BT_EVENT_AVRCP_KEY_VOLUME_UP:
        {

        }
        break;

    case BT_EVENT_AVRCP_KEY_VOLUME_DOWN:
        {

        }
        break;

    case BT_EVENT_AVRCP_REG_VOLUME_CHANGED:
        {
            p_link = app_find_br_link(param->avrcp_reg_volume_changed.bd_addr);
            if (p_link != NULL)
            {
                uint8_t pair_idx;
                uint8_t vol;

                bt_bond_index_get(p_link->bd_addr, &pair_idx);
                vol = (app_cfg_nv.audio_gain_level[pair_idx] * 0x7F +
                       app_cfg_volume.playback_volume_max / 2) /
                      app_cfg_volume.playback_volume_max;
                bt_avrcp_volume_change_register_rsp(p_link->bd_addr, vol);
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY:
        {
            /*20230607
                        if (br_db.audio_play_mode == MODE_APP_A2DP_SRC)
                        {
                            if (app_audio_get_play_status() == APP_AUDIO_STATE_PAUSE)
                            {
                                T_IO_MSG play_msg;
                                play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                                play_msg.subtype = IO_MSG_MMI;
                                play_msg.u.param = MMI_AV_PLAY_PAUSE;
                                app_send_msg_to_bt_task(&play_msg);
                            }
                            else if (app_audio_get_play_status() == APP_AUDIO_STATE_STOP)
                            {
                                play_next_music();
                            }
                        }*/
        }
        break;

    case BT_EVENT_AVRCP_PAUSE:
        {
            /*20230607
                        if (br_db.audio_play_mode == MODE_APP_A2DP_SRC)
                        {
                            if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
                            {
                                T_IO_MSG play_msg;
                                play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                                play_msg.subtype = IO_MSG_MMI;
                                play_msg.u.param = MMI_AV_PLAY_PAUSE;
                                app_send_msg_to_bt_task(&play_msg);
                            }
                        }*/
        }
        break;

    case BT_EVENT_AVRCP_STOP:
        {
            /*20230607
                        if (br_db.audio_play_mode == MODE_APP_A2DP_SRC)
                        {
                            if (app_audio_get_play_status() != APP_AUDIO_STATE_STOP)
                            {
                                T_IO_MSG play_msg;
                                play_msg.type = IO_MSG_TYPE_WRISTBNAD;
                                play_msg.subtype = IO_MSG_MMI;
                                play_msg.u.param = MMI_AV_STOP;
                                app_send_msg_to_bt_task(&play_msg);
                            }
                        }*/
        }
        break;

    case BT_EVENT_AVRCP_FORWARD:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
//20230607      play_next_music();
            }
        }
        break;

    case BT_EVENT_AVRCP_BACKWARD:
        {
            if (br_db.a2dp_info.audio_play_mode == MODE_APP_A2DP_SRC)
            {
//20230607      play_prev_music();
            }
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_avrcp_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_avrcp_init(void)
{
//20230607    if (app_cfg_const.supported_profile_mask & AVRCP_PROFILE_MASK)
    {
        bt_avrcp_init(MAX_BR_LINK_NUM);
//20230607  bt_avrcp_init(app_cfg_const.avrcp_link_number);
        bt_mgr_cback_register(app_avrcp_bt_cback);
    }
}

