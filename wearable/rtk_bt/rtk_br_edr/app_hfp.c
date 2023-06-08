/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "app_timer.h"
#include "btm.h"
#include "remote.h"
#include "ringtone.h"
#include "bt_bond.h"
#include "app_cfg.h"
#include "dp_br_info.h"
#include "app_report.h"
#include "app_br_link_util.h"
#include "app_le_link_util.h"
#include "bt_hfp.h"
#include "app_sdp.h"
#include "app_hfp.h"
#include "dp_app.h"
#include "dp_br_info.h"
#include "audio.h"
#include "os_mem.h"
#include "dp_br_info.h"
#include "app_cfg.h"
#include "dp_rtk_db.h"
#include "app_audio_policy.h"
#include "audio_volume.h"
#include "app_bond.h"
#include "os_timer.h"
#include "app_linkback.h"

typedef enum
{
    APP_HFP_TIMER_AUTO_ANSWER_CALL = 0x00,
    APP_HFP_TIMER_RING             = 0x01,
} T_APP_HFP_TIMER;

static uint8_t hfp_timer_id = 0;
static uint8_t timer_idx_hfp_auto_answer_call = 0;
static uint8_t timer_idx_hfp_ring = 0;
//static void *timer_handle_mic_mute_alarm = NULL;

static T_APP_HFP_CALL_STATUS app_call_status;
static uint8_t active_hf_index = 0;
static bool hf_ring_active = false;
uint8_t  call_num_hfp[21];
void extern *acl_reconnect_timer;

void app_hfp_volume_up(void)
{
    uint8_t max_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx;
    T_AUDIO_STREAM_TYPE volume_type;
    volume_type = AUDIO_STREAM_TYPE_VOICE;
    active_idx = app_hfp_get_active_hf_index();

    if (br_db.br_link[active_idx].call_status == APP_HFP_CALL_INCOMING &&
        br_db.br_link[active_idx].is_inband_ring == false)
    {
        APP_PRINT_INFO0("app_volume_up: do not change vol when outband ringtone");
        return;
    }

    if (bt_bond_index_get(br_db.br_link[active_idx].bd_addr, &pair_idx) == false)
    {
        APP_PRINT_ERROR0("app_volume_up: find active hfp pair_index fail");
        return;
    }
    curr_volume = app_cfg_nv.voice_gain_level[pair_idx];
    max_volume = app_cfg_volume.voice_out_volume_max;

    if (curr_volume < max_volume)
    {
        curr_volume++;
    }

    audio_volume_out_set(volume_type, curr_volume);

    uint8_t level = (curr_volume * 0x0F + app_cfg_volume.voice_out_volume_max /
                     2) / app_cfg_volume.voice_out_volume_max;

    if (br_db.audio_info.voice_muted)
    {
        br_db.audio_info.voice_muted = false;
    }

    app_cfg_nv.voice_gain_level[pair_idx] = curr_volume;
    bt_hfp_speaker_gain_level_report(br_db.br_link[active_idx].bd_addr, level);
}

void app_hfp_volume_down(void)
{
    bool already_vgs0 = false;
    uint8_t min_volume = 0;
    uint8_t curr_volume = 0;
    uint8_t active_idx;
    uint8_t pair_idx;
    T_AUDIO_STREAM_TYPE volume_type;
    volume_type = AUDIO_STREAM_TYPE_VOICE;
    active_idx = app_hfp_get_active_hf_index();

    if (br_db.br_link[active_idx].call_status == APP_HFP_CALL_INCOMING &&
        br_db.br_link[active_idx].is_inband_ring == false)
    {
        APP_PRINT_INFO0("app_volume_down: do not change vol when outband ringtone");
        return;
    }

    if (bt_bond_index_get(br_db.br_link[active_idx].bd_addr, &pair_idx) == false)
    {
        APP_PRINT_ERROR0("app_volume_down: find active hfp pair_index fail");
        return;
    }
    curr_volume = app_cfg_nv.voice_gain_level[pair_idx];
    min_volume = app_cfg_volume.voice_out_volume_min;

    if (curr_volume > min_volume)
    {
        curr_volume--;
    }
    else
    {
        curr_volume = min_volume;
        /*if ios version is 13, send AT+VGS=0 repeatedly will make voice mute*/
        if (curr_volume == 0)
        {
            already_vgs0 = true;
        }
    }

    audio_volume_out_set(volume_type, curr_volume);

    uint8_t level = (curr_volume * 0x0F + app_cfg_volume.voice_out_volume_max /
                     2) / app_cfg_volume.voice_out_volume_max;

    if (br_db.audio_info.voice_muted)
    {
        br_db.audio_info.voice_muted = false;
    }

    if (already_vgs0 == false)
    {
        bt_hfp_speaker_gain_level_report(br_db.br_link[active_idx].bd_addr, level);
    }
    app_cfg_nv.voice_gain_level[pair_idx] = curr_volume;
}

static void app_hfp_set_mic_mute(void)
{
    APP_PRINT_INFO1("app_hfp_set_mic_mute is_mic_mute %d", app_audio_is_mic_mute());

    if (app_audio_is_mic_mute())
    {
        audio_volume_in_mute(AUDIO_STREAM_TYPE_VOICE);
    }
    else
    {
        audio_volume_in_unmute(AUDIO_STREAM_TYPE_VOICE);
    }
}

void app_hfp_update_call_status(void)
{
    uint8_t i = 0;
    uint8_t inactive_hf_index = 0;
    uint8_t exchange_active_inactive_index_fg = 0;
    uint8_t call_status_old;
    T_APP_HFP_CALL_STATUS active_hf_status = APP_HFP_CALL_IDLE;
    T_APP_HFP_CALL_STATUS inactive_hf_status = APP_HFP_CALL_IDLE;

    call_status_old = app_hfp_get_call_status();

    active_hf_status = (T_APP_HFP_CALL_STATUS)br_db.br_link[active_hf_index].call_status;

    APP_PRINT_INFO3("app_hfp_update_call_status: call_status_old 0x%04x, active_hf_status 0x%04x, active_hf_index 0x%04x",
                    call_status_old, active_hf_status, app_hfp_get_active_hf_index());

    //find inactive device
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (i != active_hf_index)
        {
            if ((br_db.br_link[i].connected_profile & HFP_PROFILE_MASK) ||
                (br_db.br_link[i].connected_profile & HSP_PROFILE_MASK))
            {
                inactive_hf_index = i;
                inactive_hf_status = (T_APP_HFP_CALL_STATUS)br_db.br_link[inactive_hf_index].call_status;
                break;
            }
        }
    }

    if (active_hf_status == APP_HFP_CALL_IDLE)
    {
        if (inactive_hf_status != APP_HFP_CALL_IDLE)
        {
            exchange_active_inactive_index_fg = 1;
        }
    }
    else if ((active_hf_status != APP_HFP_CALL_ACTIVE) &&
             (active_hf_status != APP_HFP_CALL_ACTIVE_WITH_CALL_HELD) &&
             (active_hf_status != APP_HFP_CALL_ACTIVE_WITH_CALL_WAITING))
    {
        if ((inactive_hf_status == APP_HFP_CALL_ACTIVE) ||
            (inactive_hf_status == APP_HFP_CALL_ACTIVE_WITH_CALL_HELD) ||
            (inactive_hf_status == APP_HFP_CALL_ACTIVE_WITH_CALL_WAITING))
        {
            exchange_active_inactive_index_fg = 1;
        }
    }
    APP_PRINT_INFO1("app_hfp_update_call_status: exchange_active_inactive_index_fg = %d",
                    exchange_active_inactive_index_fg);

    if (exchange_active_inactive_index_fg)
    {
        uint8_t i;
        //exchange index
        i = inactive_hf_index;
        inactive_hf_index = active_hf_index;
        app_hfp_set_active_hf_index(br_db.br_link[i].bd_addr);
        app_bond_set_priority(br_db.br_link[i].bd_addr);

        //exchange status
        i = inactive_hf_status;
        inactive_hf_status = active_hf_status;
        active_hf_status = (T_APP_HFP_CALL_STATUS)i;
    }

    //update app_call_status
    switch (active_hf_status)
    {
    case APP_HFP_CALL_INCOMING:
        if (inactive_hf_status >= APP_HFP_CALL_ACTIVE)
        {
            app_hfp_set_call_status(APP_HFP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT);
        }
        else
        {
            app_hfp_set_call_status(APP_HFP_CALL_INCOMING);
        }
        break;

    case APP_HFP_CALL_ACTIVE:
        if (inactive_hf_status >= APP_HFP_CALL_ACTIVE)
        {
            app_hfp_set_call_status(APP_HFP_MULTILINK_CALL_ACTIVE_WITH_CALL_HOLD);
        }
        else if (inactive_hf_status == APP_HFP_CALL_INCOMING)
        {
            app_hfp_set_call_status(APP_HFP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT);
        }
        else
        {
            app_hfp_set_call_status(APP_HFP_CALL_ACTIVE);
        }
        break;

    default:
        app_hfp_set_call_status(active_hf_status);
        break;
    }

    APP_PRINT_INFO3("app_hfp_update_call_status: call_status 0x%04x, active_hf_status 0x%04x, active_hf_index 0x%04x",
                    app_hfp_get_call_status(), active_hf_status, app_hfp_get_active_hf_index());

    if (call_status_old != app_hfp_get_call_status())
    {
        if (call_status_old == APP_HFP_CALL_IDLE)
        {
//            app_anc_save_recover_state();
        }
        else if (app_hfp_get_call_status() == APP_HFP_CALL_IDLE)
        {
//            app_anc_load_anc_recover_state();
        }
    }
}

static void app_hfp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;
    APP_PRINT_INFO1("app_hfp_bt_cback: 0x%04x", event_type);

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_IND:
        {
            T_APP_BR_LINK *p_link = app_find_br_link(param->hfp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_hfp_bt_cback: no acl link found");
                return;
            }

            bool is_hfp_connected = false;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; ++i)
            {
                if (br_db.br_link[i].used &&
                    (br_db.br_link[i].connected_profile & HFP_PROFILE_MASK))
                {
                    is_hfp_connected = true;
                    break;
                }
            }
            if (is_hfp_connected)
            {
                APP_PRINT_INFO0("HFP is already connected. Connection is rejected.");
                bt_hfp_connect_cfm(p_link->bd_addr, false);
                break; // Jump over handling.
            }

            uint8_t temp_index = app_bt_bond_get_index_by_addr(param->hfp_conn_ind.bd_addr);
            if (br_db.bond_device[temp_index].device_type == T_DEVICE_TYPE_EAR)
            {
                APP_PRINT_ERROR0("app_hfp_bt_cback: special ear device cfm false");
                bt_hfp_connect_cfm(p_link->bd_addr, false);
                memcpy(br_db.acl_info.acl_reconnect_addr, param->hfp_conn_ind.bd_addr, 6);
                os_timer_restart(&acl_reconnect_timer, 1000);
            }
            bt_hfp_connect_cfm(p_link->bd_addr, true);

            if ((p_link->connected_profile & PBAP_PROFILE_MASK) == 0)
            {
//20230607      app_create_connection(param->hfp_conn_ind.bd_addr, PBAP_PROFILE_MASK);
            }
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                uint8_t link_number;
                uint8_t pair_idx;

                p_link->call_id_type_check = true;
                p_link->call_id_type_num = false;

                bt_bond_index_get(p_link->bd_addr, &pair_idx);

                uint8_t voice_gain_level[8];
                rtk_kvdb_get(KVDB_MCU_CFG_RW_NAME, KEY_MCU_CFG_RW_VOICE_GAIN_LEVEL, voice_gain_level,
                             sizeof(voice_gain_level));

                bt_hfp_speaker_gain_level_report(p_link->bd_addr, voice_gain_level[pair_idx]);
//20230607      bt_hfp_speaker_gain_level_report(p_link->bd_addr, app_cfg_nv.voice_gain_level[pair_idx]);

                bt_hfp_microphone_gain_level_report(p_link->bd_addr, 0x0a);

                bt_hfp_batt_level_report(p_link->bd_addr, app_db.local_batt_level);

                link_number = app_connected_profile_link_num(HFP_PROFILE_MASK | HSP_PROFILE_MASK);
                if (link_number == 1)
                {
                    app_hfp_set_active_hf_index(p_link->bd_addr);
                    app_bond_set_priority(p_link->bd_addr);
                }
                if (br_db.br_link[br_db.hf_info.first_hf_index].app_hf_state == APP_HF_STATE_STANDBY)
                {
                    br_db.hf_info.first_hf_index = p_link->id;
                }
                else
                {
                    br_db.hf_info.last_hf_index = p_link->id;
                }
                p_link->app_hf_state = APP_HF_STATE_CONNECTED;
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_call_status.bd_addr);
            if (p_link != NULL)
            {
                switch (param->hfp_call_status.curr_status)
                {
                case BT_HFP_CALL_IDLE:
                    {
                        p_link->call_status = APP_HFP_CALL_IDLE;
                    }
                    break;

                case BT_HFP_CALL_INCOMING:
                    {
                        p_link->call_status = APP_HFP_CALL_INCOMING;
                    }
                    break;

                case BT_HFP_CALL_OUTGOING:
                    {
                        p_link->call_status = APP_HFP_CALL_OUTGOING;
                    }
                    break;

                case BT_HFP_CALL_ACTIVE:
                    {
                        p_link->call_status = APP_HFP_CALL_ACTIVE;
                    }
                    break;

                case BT_HFP_CALL_HELD:
                    {
                        //p_link->call_status = APP_HFP_CALL_HELD;
                    }
                    break;

                case BT_HFP_CALL_ACTIVE_WITH_CALL_WAITING:
                    {
                        p_link->call_status = APP_HFP_CALL_ACTIVE_WITH_CALL_WAITING;
                    }
                    break;

                case BT_HFP_CALL_ACTIVE_WITH_CALL_HELD:
                    {
                        p_link->call_status = APP_HFP_CALL_ACTIVE_WITH_CALL_HELD;
                    }
                    break;

                default:
                    break;
                }

//auto answer incoming call
//                if (p_link->call_status == APP_HFP_CALL_INCOMING)
//                {
//                    app_start_timer(&timer_idx_hfp_auto_answer_call, "auto_answer_call",
//                                    hfp_timer_id, APP_HFP_TIMER_AUTO_ANSWER_CALL, p_link->id, false,
//                                    2000);
//                }

                if (p_link->call_status == APP_HFP_CALL_IDLE)
                {
                    p_link->call_id_type_check = true;
                    p_link->call_id_type_num = false;
                }

                app_hfp_update_call_status();

                if (p_link->call_status == APP_HFP_CALL_ACTIVE)
                {
//20230607          gui_update_by_event((UI_MenuTypeDef *)&Call, NULL, true);
                }
            }
        }
        break;

    case BT_EVENT_HFP_SERVICE_STATUS:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_service_status.bd_addr);

            if (p_link != NULL)
            {
                p_link->service_status = param->hfp_service_status.status;
            }
        }
        break;

    case BT_EVENT_HFP_CALL_WAITING_IND:
    case BT_EVENT_HFP_CALLER_ID_IND:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                T_APP_BR_LINK *br_link;
                T_APP_LE_LINK *le_link;
                char *number;

                if (event_type == BT_EVENT_HFP_CALLER_ID_IND)
                {
                    br_link = app_find_br_link(param->hfp_caller_id_ind.bd_addr);
                    number = (char *)param->hfp_caller_id_ind.number;
                }
                else
                {
                    br_link = app_find_br_link(param->hfp_call_waiting_ind.bd_addr);
                    number = (char *)param->hfp_call_waiting_ind.number;
                }

                le_link = app_find_le_link_by_addr(param->pbap_caller_id_name.bd_addr);

                if (br_link != NULL)
                {
                    /* Sanity check if BR/EDR TTS session is ongoing */
                    if (br_link->cmd_set_enable == true &&
                        br_link->tts_handle != NULL)
                    {
                        break;
                    }

                    /* Sanity check if BLE TTS session is ongoing */
                    if (le_link != NULL &&
                        le_link->cmd_set_enable == true &&
                        le_link->tts_handle != NULL)
                    {
                        break;
                    }

                    if (br_link->call_id_type_check == true)
                    {
                        if (br_link->connected_profile & PBAP_PROFILE_MASK)
                        {
                            if (bt_pbap_vcard_listing_by_number_pull(br_link->bd_addr, number) == false)
                            {
                                br_link->call_id_type_check = false;
                                br_link->call_id_type_num = true;
                            }
                            else
                            {
                                uint8_t number_len = strlen(number);
                                if (number_len > 20)
                                {
                                    number_len = 20;
                                }
                                call_num_hfp[0] = number_len;
                                memcpy(call_num_hfp + 1, number, number_len);
                            }
                        }
                        else
                        {
                            APP_PRINT_INFO0("BT_EVENT_HFP_CALLER_ID_IND not suppert pbap");
                            br_link->call_id_type_check = false;
                            br_link->call_id_type_num = true;
                            uint8_t number_len = strlen(number);
                            if (number_len > 20)
                            {
                                number_len = 20;
                            }
                            call_num_hfp[0] = number_len;
                            memcpy(call_num_hfp + 1, number, number_len);
                        }
                    }
                    if (br_link->call_id_type_check == false)
                    {
                        if (br_link->call_id_type_num == true)
                        {
                            //show number on screen
//20230607                  update_call_number_name_info((uint8_t *)number, strlen(number));
//20230607                  gui_update_by_event((UI_MenuTypeDef *)&Call, NULL, true);
                            br_link->call_id_type_num = false;
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_RING_ALERT:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_ring_alert.bd_addr);
            if (p_link != NULL)
            {
                p_link->is_inband_ring = param->hfp_ring_alert.is_inband;

                if (p_link->is_inband_ring == false ||
                    p_link->id != active_hf_index) /* TODO check active sco link */
                {
                    if (hf_ring_active == false && p_link->call_status == APP_HFP_CALL_INCOMING)
                    {
//ring alert by ringtone
//                        app_audio_tone_play(app_cfg_const.tone_hf_call_in, false, true);
//                        {
//                            hf_ring_active = true;

//                            app_start_timer(&timer_idx_hfp_ring, "hfp_ring",
//                            hfp_timer_id, APP_HFP_TIMER_RING, p_link->id, true,
//                            3 * 1000);
//                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_SPK_VOLUME_CHANGED:
        {
            T_APP_BR_LINK *p_link;
            p_link = app_find_br_link(param->hfp_spk_volume_changed.bd_addr);
            if (p_link != NULL)
            {
                uint8_t pair_idx;

                bt_bond_index_get(p_link->bd_addr, &pair_idx);

                app_cfg_nv.voice_gain_level[pair_idx] = (param->hfp_spk_volume_changed.volume *
                                                         app_cfg_volume.voice_out_volume_max +
                                                         0x0f / 2) / 0x0f;
                audio_volume_out_set(AUDIO_STREAM_TYPE_VOICE, app_cfg_nv.voice_gain_level[pair_idx]);

                if (br_db.audio_info.voice_muted)
                {
                    br_db.audio_info.voice_muted = false;
                }
            }
        }
        break;

    case BT_EVENT_HFP_MIC_VOLUME_CHANGED:
        {

        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_find_br_link(param->hfp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (param->hfp_disconn_cmpl.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
                    //do nothing
                }
                else
                {
                    uint8_t i;

                    p_link->call_status = APP_HFP_CALL_IDLE;
                    p_link->app_hf_state = APP_HF_STATE_STANDBY;
                    if (br_db.hf_info.first_hf_index == p_link->id)
                    {
                        br_db.hf_info.first_hf_index = br_db.hf_info.last_hf_index;
                    }

                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (br_db.br_link[i].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
                        {
                            app_hfp_set_active_hf_index(br_db.br_link[i].bd_addr);
                            app_bond_set_priority(br_db.br_link[i].bd_addr);
                            break;
                        }
                    }

                    if (app_hfp_get_call_status() != APP_HFP_CALL_IDLE)
                    {
                        app_hfp_update_call_status();
                    }
                }
            }
        }
        break;

    case BT_EVENT_SCO_CONN_IND:
        {
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {

        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {

        }
        break;

    case BT_EVENT_SCO_DATA_IND:
        {

        }
        break;


    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_hfp_bt_cback: event_type 0x%04x", event_type);
    }
}

T_APP_HFP_CALL_STATUS app_hfp_get_call_status(void)
{
    return app_call_status;
}

void app_hfp_set_call_status(T_APP_HFP_CALL_STATUS call_status)
{
    app_call_status = call_status;
}

uint8_t app_hfp_get_active_hf_index(void)
{
    return active_hf_index;
}

bool app_hfp_sco_is_connected(void)
{
    return (br_db.br_link[app_hfp_get_active_hf_index()].sco_handle == 0) ? false : true;
}

bool app_hfp_set_active_hf_index(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_find_br_link(bd_addr);
    if (p_link)
    {
        active_hf_index = p_link->id;
        return bt_sco_link_switch(bd_addr);
    }
    return false;
}

void app_hfp_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_hfp_timeout_cb: timer_id %d, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_HFP_TIMER_AUTO_ANSWER_CALL:
        {
            T_APP_BR_LINK *p_link;

            p_link = &(br_db.br_link[param]);
            app_stop_timer(&timer_idx_hfp_auto_answer_call);
            if (p_link->call_status == APP_HFP_CALL_INCOMING)
            {
                bt_hfp_call_answer_req(p_link->bd_addr);
            }
        }
        break;

    case APP_HFP_TIMER_RING:
        {
            T_APP_BR_LINK *link1 = NULL;
            T_APP_BR_LINK *link2 = NULL;
            bool           ring_trigger = false;
            uint8_t        i;

            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (br_db.br_link[i].call_status == APP_HFP_CALL_INCOMING)
                {
                    link1 = &(br_db.br_link[i]);
                    break;
                }
            }

            for (; i < MAX_BR_LINK_NUM; i++)
            {
                if (br_db.br_link[i].call_status == APP_HFP_CALL_INCOMING)
                {
                    link2 = &(br_db.br_link[i]);
                    break;
                }
            }

            if (link1 != NULL || link2 != NULL)
            {
                if (link1 != NULL)
                {
                    if (link1->is_inband_ring == false ||
                        link1->id != active_hf_index) /* TODO check active sco link */
                    {
                        ring_trigger = true;
                    }
                }

                if (link2 != NULL)
                {
                    if (link2->is_inband_ring == false ||
                        link2->id != active_hf_index) /* TODO check active sco link */
                    {
                        ring_trigger = true;
                    }
                }
            }

            if (ring_trigger == true)
            {
//                    app_audio_tone_play(app_cfg_const.tone_hf_call_in, false, true);
            }
            else
            {
                app_stop_timer(&timer_idx_hfp_ring);
                hf_ring_active = false;
            }
        }
        break;
    default:
        break;
    }
}

void app_hfp_init(void)
{
//20230607    if (app_cfg_const.supported_profile_mask & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
    {
        app_cfg_const.hfp_brsf_capability = 0x2BF;
        bt_hfp_init(MAX_BR_LINK_NUM, RFC_HFP_CHANN_NUM, RFC_HSP_CHANN_NUM,
                    app_cfg_const.hfp_brsf_capability);
//20230607        bt_hfp_init(app_cfg_const.hfp_link_number, RFC_HFP_CHANN_NUM,
//                    RFC_HSP_CHANN_NUM, app_cfg_const.hfp_brsf_capability);
        bt_mgr_cback_register(app_hfp_bt_cback);
        app_timer_reg_cb(app_hfp_timeout_cb, &hfp_timer_id);
    }
}

void app_hfp_mute_ctrl(void)
{
    uint8_t hf_active = 0;
    T_APP_BR_LINK *p_link;
    uint8_t app_idx;

    app_idx = app_hfp_get_active_hf_index();
    p_link = &(br_db.br_link[app_idx]);

    if (p_link->app_hf_state == APP_HF_STATE_CONNECTED)
    {
        if ((p_link->call_status == APP_HFP_CALL_ACTIVE) && (p_link->sco_handle != 0))
        {
            hf_active = 1;
        }
    }
    else
    {
        if (p_link->sco_handle)
        {
            hf_active = 1;
        }
    }

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (hf_active)
        {
            app_hfp_set_mic_mute();
        }
    }
}

