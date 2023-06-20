/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "os_mem.h"
#include "trace.h"
//#include "app_dsp_cfg.h"
#include "app_cfg.h"
#include "app_audio_route.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_cmd.h"
#endif



bool app_audio_route_dac_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_cfg_volume.ringtone_volume_max)
            {
                //app_dsp_cfg_data->notification_dac_dig_gain_table[level] = gain;
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_cfg_volume.voice_prompt_volume_max)
            {
                //app_dsp_cfg_data->notification_dac_dig_gain_table[level] = gain;
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_LINE:
        {
            if (level <= app_cfg_volume.playback_volume_max)
            {
                //app_dsp_cfg_data->audio_dac_dig_gain_table[level] = gain;
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_cfg_volume.voice_out_volume_max)
            {
                //app_dsp_cfg_data->voice_dac_dig_gain_table[level] = gain;
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
    case AUDIO_CATEGORY_LLAPT:
        {

        }
        break;

    default:
        return false;
    }
    return true;
}

bool app_audio_route_adc_gain_set(T_AUDIO_CATEGORY category, uint8_t level, uint16_t gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_ANC:
    case AUDIO_CATEGORY_LINE:
    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_cfg_volume.voice_volume_in_max)
            {
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //app_dsp_cfg_data->voice_adc_gain_table[level] = gain;
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {

        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {

        }
        break;

    default:
        return false;
    }
    return true;
}

static bool app_dac_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                   T_AUDIO_ROUTE_DAC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_TONE:
        {
            if (level <= app_cfg_volume.ringtone_volume_max)
            {
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_dig_gain = app_dsp_cfg_data->notification_dac_dig_gain_table[level];
                //gain->dac_ana_gain = app_dsp_cfg_data->notification_dac_ana_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_VP:
        {
            if (level <= app_cfg_volume.voice_prompt_volume_max)
            {
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_dig_gain = app_dsp_cfg_data->notification_dac_dig_gain_table[level];
                //gain->dac_ana_gain = app_dsp_cfg_data->notification_dac_ana_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_LINE:
        {
#if (F_APP_AVP_INIT_SUPPORT == 1)
            if (app_dac_gain_set_hook)
            {
                return app_dac_gain_set_hook(level, gain);
            }
#else
            if (level <= app_cfg_volume.playback_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                if ((level < app_cfg_const.playback_volume_max) || (teams_dac_gain == 0))
                {
                    gain->dac_dig_gain = app_dsp_cfg_data->audio_dac_dig_gain_table[level] + teams_dac_gain;
                }
                else
                {
                    gain->dac_dig_gain = app_dsp_cfg_data->audio_dac_dig_gain_table[level - 1] + teams_dac_gain + 384;
                }
#else
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_dig_gain = app_dsp_cfg_data->audio_dac_dig_gain_table[level];
#endif
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_ana_gain = app_dsp_cfg_data->audio_dac_ana_gain_table[level];
            }
            else
            {
                return false;
            }
#endif
        }
        break;

    case AUDIO_CATEGORY_VOICE:
        {
            if (level <= app_cfg_volume.voice_out_volume_max)
            {
#if F_APP_TEAMS_FEATURE_SUPPORT
                if ((level < app_cfg_const.voice_out_volume_max) || (teams_dac_gain == 0))
                {
                    gain->dac_dig_gain = app_dsp_cfg_data->voice_dac_dig_gain_table[level] + teams_dac_gain;
                }
                else
                {
                    gain->dac_dig_gain = app_dsp_cfg_data->voice_dac_dig_gain_table[level - 1] + teams_dac_gain + 384;
                }
#else
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_dig_gain = app_dsp_cfg_data->voice_dac_dig_gain_table[level];
#endif
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->dac_ana_gain = app_dsp_cfg_data->voice_dac_ana_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_ANC:
    case AUDIO_CATEGORY_APT:
    case AUDIO_CATEGORY_LLAPT:
        {

        }
        break;

    default:
        return false;
    }
    return true;
}

static bool app_adc_gain_get_cback(T_AUDIO_CATEGORY category, uint32_t level,
                                   T_AUDIO_ROUTE_ADC_GAIN *gain)
{
    switch (category)
    {
    case AUDIO_CATEGORY_AUDIO:
    case AUDIO_CATEGORY_VOICE:
    case AUDIO_CATEGORY_RECORD:
    case AUDIO_CATEGORY_ANC:
    case AUDIO_CATEGORY_LINE:
    case AUDIO_CATEGORY_VAD:
        {
            if (level <= app_cfg_volume.voice_volume_in_max)
            {
                APP_PRINT_INFO2("(%s:%d) need to do", __FUNCTION__, __LINE__);
                //gain->adc_gain = app_dsp_cfg_data->voice_adc_gain_table[level];
            }
            else
            {
                return false;
            }
        }
        break;

    case AUDIO_CATEGORY_APT:
        {

        }
        break;

    case AUDIO_CATEGORY_LLAPT:
        {

        }
        break;

    default:
        return false;
    }
    return true;

}

void app_audio_route_gain_init(void)
{
    uint8_t index;

    for (index = 0; index < AUDIO_CATEGORY_NUM; index++)
    {
        audio_route_gain_register((T_AUDIO_CATEGORY)index, app_dac_gain_get_cback, app_adc_gain_get_cback);
    }
}
