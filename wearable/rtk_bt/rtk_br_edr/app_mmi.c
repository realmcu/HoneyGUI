/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "test_mode.h"
#include "app_timer.h"
#include "bt_avrcp.h"
#include "bt_bond.h"
#include "eq.h"
#include "audio_passthrough.h"
#include "audio.h"
#include "audio_volume.h"
#include "vad.h"
#include "remote.h"
#include "bt_bond.h"
#include "app_mmi.h"
#include "dp_br_info.h"
#include "app_cfg.h"
#include "sysm.h"
#include "app_transfer.h"
#include "app_hfp.h"
#include "app_br_link_util.h"
#include "app_audio_policy.h"
#include "app_playback.h"
#include "app_audio_if.h"

static void app_mmi_mic_mute_set(void)
{
    {
        uint8_t active_hf_idx = app_hfp_get_active_hf_index();
        T_APP_BR_LINK *p_link = NULL;
        p_link = app_find_br_link(br_db.br_link[active_hf_idx].bd_addr);
        audio_track_volume_in_mute(p_link->sco_track_handle);
    }
}
static void app_mmi_mic_unmute_set(void)
{
    {
        uint8_t active_hf_idx = app_hfp_get_active_hf_index();
        T_APP_BR_LINK *p_link = NULL;
        p_link = app_find_br_link(br_db.br_link[active_hf_idx].bd_addr);
        audio_track_volume_in_unmute(p_link->sco_track_handle);
    }
}
void app_mmi_handle_action(uint8_t action)
{
    APP_PRINT_TRACE1("app_mmi_handle_action: action 0x%02x", action);

    switch (action)
    {
    case MMI_HF_ANSWER_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            /* BBPRO2BUG-2578 */
            voice_prompt_flush(true);

            bt_hfp_call_answer_req(br_db.br_link[app_idx].bd_addr);
        }
        break;

    case MMI_HF_REJECT_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();
            bt_hfp_call_terminate_req(br_db.br_link[app_idx].bd_addr);
        }
        break;

    case MMI_HF_END_ACTIVE_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            bt_hfp_call_terminate_req(br_db.br_link[app_idx].bd_addr);
        }
        break;

    case MMI_HF_END_OUTGOING_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            bt_hfp_call_terminate_req(br_db.br_link[app_idx].bd_addr);
        }
        break;

    case MMI_HF_TRANSFER_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            if (br_db.br_link[app_idx].sco_handle)// to phone
            {
                bt_hfp_audio_disconnect_req(br_db.br_link[app_idx].bd_addr);
            }
            else //to bud
            {
                bt_hfp_audio_connect_req(br_db.br_link[app_idx].bd_addr);
            }
        }
        break;

    case MMI_HF_INITIATE_VOICE_DIAL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            if (br_db.br_link[app_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
            {
                bt_hfp_voice_recognition_enable_req(br_db.br_link[app_idx].bd_addr);
            }
            else
            {
                //may connect phone here
            }
        }
        break;

    case MMI_HF_CANCEL_VOICE_DIAL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();

            bt_hfp_voice_recognition_disable_req(br_db.br_link[app_idx].bd_addr);
        }
        break;
    case MMI_HF_RELEASE_HELD_OR_WAITING_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();
            bt_hfp_release_held_or_waiting_call_req(br_db.br_link[app_idx].bd_addr);
        }
        break;
    case MMI_HF_RELEASE_ACTIVE_CALL_ACCEPT_HELD_OR_WAITING_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();
            bt_hfp_release_active_call_accept_held_or_waiting_call_req(br_db.br_link[app_idx].bd_addr);
        }
        break;
    case MMI_HF_SWITCH_TO_SECOND_CALL:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();
            bt_hfp_hold_active_call_accept_held_or_waiting_call_req(br_db.br_link[app_idx].bd_addr);
        }
        break;
    case MMI_HF_JOIN_TWO_CALLS:
        {
            uint8_t app_idx = app_hfp_get_active_hf_index();
            bt_hfp_join_two_calls_req(br_db.br_link[app_idx].bd_addr);
        }
        break;
    case MMI_DEV_SPK_VOL_UP:
        {
//20230607  app_volume_up();
        }
        break;

    case MMI_DEV_SPK_VOL_DOWN:
        {
//20230607  app_volume_down();
        }
        break;
    case MMI_DEV_MIC_MUTE:
        {
            app_mmi_mic_mute_set();
        }
        break;
    case MMI_DEV_MIC_UNMUTE:
        {
            app_mmi_mic_unmute_set();
        }
        break;
    case MMI_DEV_MIC_MUTE_UNMUTE:
        {
            if (app_audio_check_mic_mute_enable() == true)
            {
                if (app_audio_is_mic_mute())
                {
                    app_audio_set_mic_mute_status(0);
                }
                else
                {
                    app_audio_set_mic_mute_status(1);
                }
                app_hfp_mute_ctrl();
            }
        }
        break;
    case MMI_AV_PLAY_PAUSE:
        {
            /*20230607 APP_PRINT_INFO1("call status = %d", app_hfp_get_call_status());
                       if ((app_hfp_get_call_status() == APP_HFP_CALL_IDLE
                       && br_db.batt.allow_open.playback && br_db.transfer_status == TRANSFER_STOPPED
                       ) || app_audio_get_play_status() == APP_AUDIO_STATE_PLAY
                   )
                       {
                           if (app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
                           {
                               app_audio_pause();
                           }
                           else if (app_audio_get_play_status() == APP_AUDIO_STATE_PAUSE)
                           {
                               app_audio_start();
                           }
                       }
                       else
                       {
                           APP_PRINT_ERROR0("call active or low battery level or song is transferring,, do not play music!");
                       }*/
        }
        break;

    case MMI_AV_STOP:
        {
            /*20230607  if (app_audio_get_play_status() != APP_AUDIO_STATE_STOP)
                        {
                            app_audio_stop();
                        }*/
        }
        break;

    case MMI_AV_FWD:
        {
            /*20230607  APP_PRINT_INFO2("mmi fwd call status = %d, playstatus = %d",
                                        app_hfp_get_call_status(),
                                        app_audio_get_play_status());
                        if (app_hfp_get_call_status() == APP_HFP_CALL_IDLE
                            && app_db.batt.allow_open.playback)
                        {
                            app_audio_next();
                        }
                        else
                        {
                            APP_PRINT_ERROR0("call active or low battery level, do not play music!");
                        }*/
        }
        break;

    case MMI_AV_BWD:
        {
            /*20230607  APP_PRINT_INFO2("mmi bwd call status = %d, playstatus = %d",
                                        app_hfp_get_call_status(),
                                        app_audio_get_play_status());
                        if ((app_hfp_get_call_status() == APP_HFP_CALL_IDLE
                            && app_db.batt.allow_open.playback) ||
                            app_audio_get_play_status() == APP_AUDIO_STATE_PLAY)
                        {
                            app_audio_prev();
                        }
                        else
                        {
                            APP_PRINT_ERROR0("call active or low battery level, do not play music!");
                        }*/
        }
        break;

    case MMI_SD_PLAYBACK_SWITCH:
        {
//20230607  app_audio_mode_switch(MODE_APP_PLAYBACK);
        }
        break;

    case MMI_SD_A2DP_SRC_SWITCH:
        {
//20230607  app_audio_mode_switch(MODE_APP_A2DP_SRC);
        }
        break;

    case MMI_SD_A2DP_SNK_SWITCH:
        {
//20230607  app_audio_mode_switch(MODE_APP_A2DP_SNK);
        }
        break;

    case MMI_RECORD_START:
        {
//20230607  app_mic_record_init();
//20230607  app_mic_start_recording();
        }
        break;

    case MMI_RECORD_STOP:
        {
//20230607  app_mic_stop_recording();
        }
        break;

    case MMI_RECORD_PLAY_START:
        {
//20230607  app_mic_record_play_init();
        }
        break;

    case MMI_RECORD_PLAY_STOP:
        {
//20230607  app_stop_record_play();
        }
        break;

    default:
        break;
    }
}
