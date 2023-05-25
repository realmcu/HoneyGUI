#include "app_eq.h"
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "eq_utils.h"
#include "eq.h"
#include "app_link_util.h"
#include "fmc_api.h"
//#include "app_dsp_cfg.h"
typedef struct
{
    uint32_t bypass_near_end : 1;
    uint32_t bypass_far_end : 1;
    uint32_t aec : 1;
    uint32_t dmnr : 1;
    uint32_t aes : 1;
    uint32_t nr_near_end : 1;
    uint32_t nr_far_end : 1;
    uint32_t pitch_det_near_end : 1;
    uint32_t pitch_det_far_end : 1;
    uint32_t drc_near_end : 1;
    uint32_t drc_far_end : 1;
    uint32_t rmdc_near_end : 1;
    uint32_t hpf_near_end : 1;
    uint32_t hpf_far_end : 1;
    uint32_t eq_near_end8k : 1;
    uint32_t eq_near_end16k : 1;
    uint32_t eq_far_end8k : 1;
    uint32_t eq_far_end16k : 1;
    uint32_t ignore_call_active_flag : 1;
    uint32_t dmnr_post_process : 1;
    uint32_t dmnr_test_mode : 1;
    uint32_t dmnr_bypass_lorr : 1;//0 for L,1 for R,effect if Dmnr and DmnrTestMode are 1
    uint32_t side_tone : 1;
    uint32_t far_end_loopback : 1;
    uint32_t dmnr_middle : 1; // 0: follow the setting of DmnrByPassLorR, 1: output L/2+R/2
    uint32_t nr_near_end128pts : 1;
    uint32_t bone_fusion : 1;
    uint32_t resv : 5;
} T_VOICE_PROC_ENB;

typedef struct
{
    // audio effect
    uint32_t bypass_audio_effect : 1;
    uint32_t mbagc_audio : 1;
    uint32_t eq_audio : 1;
    uint32_t aw_audio : 1;

    // audio pass through
    uint32_t audio_pass_through : 1;
    uint32_t vad_trigger : 1;
    uint32_t dehowling : 1;
    uint32_t nr_near_end : 1;

    uint32_t drc_near_end : 1;
    uint32_t rmdc_near_end : 1;
    uint32_t hpf_near_end : 1;
    uint32_t eq_near_end : 1;

    uint32_t fw_dsp_mix : 1;
    uint32_t bypass_near_end_effect : 1;
    uint32_t limiter : 1;
    uint32_t apt_mic_select : 1; // 0 for mic0, 1 for mic1

    uint32_t aec_alr : 1;
    uint32_t aes512 : 1;
    uint32_t resv : 14;
} T_AUDIO_PROC_ENB;

typedef struct t_dsp_param_r_only
{
    uint16_t sync_word;
    uint16_t reserved1;
    uint32_t dsp_tool_version_info;
    uint16_t user_version;
    uint16_t reserved2;
    uint32_t algo_para_block_offset;
    uint32_t eq_cmd_block_offset;
    uint32_t fixed_param_block_offset;
    uint32_t vad_param_block_offset;
    uint32_t eq_extend_info_offset;
    uint32_t hw_eq_block_offset;
    uint8_t  reserved[8];
    uint32_t package_features; //need check it in patch
    T_VOICE_PROC_ENB voice_stream_feature_bit;
    T_AUDIO_PROC_ENB audio_stream_feature_bit;
} T_DSP_PARAM_R_ONLY;

T_AUDIO_EFFECT_INSTANCE g_eq_44p1 = NULL, g_eq_48 = NULL;


static bool is_soft_eq_enabled(void);


void app_eq_init(void)
{
    if (!is_soft_eq_enabled())
    {
        DBG_DIRECT("audio soft eq is not enabled");
        g_eq_44p1 = NULL;
        g_eq_48 = NULL;
        return;
    }

    uint8_t *pbuf = malloc(EQ_MAX_SIZE);
    if (pbuf == NULL)
    {
        DBG_DIRECT("app_eq_init malloc failed.");
        return;
    }

//    uint16_t eq_len = eq_utils_param_get(SPK_SW_EQ, NORMAL_MODE, 0, pbuf, EQ_MAX_SIZE,
//                                         EQ_DATA_TO_DSP, AUDIO_EQ_SAMPLE_RATE_44_1KHZ);
//    g_eq_44p1 = eq_create(EQ_CONTENT_TYPE_AUDIO, pbuf, eq_len);
//    eq_enable(g_eq_44p1);

//    eq_len = eq_utils_param_get(SPK_SW_EQ, NORMAL_MODE, 0, pbuf, EQ_MAX_SIZE,
//                                EQ_DATA_TO_DSP, AUDIO_EQ_SAMPLE_RATE_48KHZ);
//    g_eq_48 = eq_create(EQ_CONTENT_TYPE_AUDIO, pbuf, eq_len);
//    eq_enable(g_eq_48);

    free(pbuf);
}

#define EFFECT_OFS 0x170 // offset from algo parm begin
#define MODE_COUNT 3

static bool is_soft_eq_enabled(void)
{
    const uint8_t *p_dsp_cfg = (const uint8_t *)flash_cur_bank_img_payload_addr_get(
                                   FLASH_IMG_DSPCONFIG);

    const T_DSP_PARAM_R_ONLY *p_dsp_hdr = (const T_DSP_PARAM_R_ONLY *)p_dsp_cfg;
    if (p_dsp_hdr->sync_word != 0xAA55)
    {
        return false;
    }

    uint32_t algo_parm_ofs = p_dsp_hdr->algo_para_block_offset;

    uint32_t effect[MODE_COUNT];
    memcpy(effect, &p_dsp_cfg[algo_parm_ofs + EFFECT_OFS], sizeof(effect));
    for (uint8_t i = 0; i < MODE_COUNT; ++i)
        if ((effect[i] & 0xf) == 2 || ((effect[i] >> 4) & 0xf) == 2 || ((effect[i] >> 8) & 0xf) == 2)
        {
            return true;
        }
    return false;
}




