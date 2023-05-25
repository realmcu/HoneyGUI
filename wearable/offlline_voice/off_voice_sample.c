
#include "rtthread.h"
#define DBG_TAG     "OFF_VOICE"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>
#include "qvoice_casr.h"
#include "rtl876x.h"

// mdl file
static const unsigned char param_res[] =
{
#include "data/wtk_kparm_res.txt"
};

static const unsigned char fsm_res[] =
{
#include "data/wtk_fsm_res.txt"
};

static const unsigned char sym_res[] =
{
#include "data/wtk_outsym_res.txt"
};

static const unsigned char asr_mdl_res[] =
{
#include "data/kmdl_data.txt"
};

static const unsigned char vad_mdl_res[] =
{
#include "data/vad_mdl_data.txt"
};

// audio file
const char audio[] =
{
#include "data/dkyy.txt"
};



static void _rec_cb(const char *res, int len, uint32_t sframe, uint32_t eframe,
                    void *upval, int id, char *res2, int len2)
{
    if (res)
    {
        if (res2)
        {
            rt_kprintf("%f %f %.*s\n", sframe * 20.0 / 1000, eframe * 20.0 / 1000, len,
                       res);
        }
        else
        {
            rt_kprintf("%f %f %.*s\n", sframe * 20.0 / 1000, eframe * 20.0 / 1000, len, res);
        }
    }
    else
    {
        rt_kprintf("%f %f Nil\n", sframe * 20.0 / 1000, eframe * 20.0 / 1000);
    }
}

void offline_voice(void)
{
    rt_trustzone_enter(1);
    rt_base_t lock;
    lock = rt_hw_interrupt_disable();
    void *c;
    {
        init_pmu_counter();
        SCB_CleanInvalidateDCache();
        qvoice_casr_set_res(vad_mdl_res, asr_mdl_res, fsm_res, sym_res, param_res);
        c = qvoice_casr_new();
        LOG_I("casr new!");
        qvoice_casr_set_notify(c, NULL, (qvoice_casr_cb)_rec_cb);
        qvoice_casr_feed(c, (short *)audio, sizeof(audio) / sizeof(short));
        qvoice_casr_feed_end(c);
        qvoice_casr_delete(c);
        LOG_I("casr delete!");
        pmu_print_counter();
        rt_thread_delay(1000);
    }
    rt_hw_interrupt_enable(lock);
    rt_trustzone_exit();
}

MSH_CMD_EXPORT(offline_voice, voice voice test);
