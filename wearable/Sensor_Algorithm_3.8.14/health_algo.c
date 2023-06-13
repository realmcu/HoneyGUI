enum { __FILE_NUM__ = 0 };
/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     health_algo.c
* @brief
* @details
* @author
* @date
* @version  v0.2
*********************************************************************************************************
*/

#include "trace.h"
#include <time.h>
#include "health_algo.h"
#include "rtthread.h"


// Private Functions Declaration
static void pedo_algo_cb(gsa_pedo_info_t *info);
static void sleep_algo_cb(gsa_sleep_stt_e stt, uint32 offset);
static void act_algo_cb(gsa_act_type_e type, uint16 para);
static void act_lift_hdl(uint16 para);
static void act_twist_hdl(uint16 para);
static void act_wave_hdl(uint16 para);


// Variables
static gsa_cbs_t gsa_cbs[] =
{
    pedo_algo_cb,
    sleep_algo_cb,
    act_algo_cb
};
static void (*act_cbs[])(uint16) =
{
    act_lift_hdl,
    act_twist_hdl,
    act_wave_hdl
};

static void sleep_algo_cb(gsa_sleep_stt_e stt, uint32 offset)
{
    time_t now;

    now = time(RT_NULL);

    /*Monitoring period : 18:00 ~ 10:00 +1, not in this do return*/


    rt_kprintf("************sleep status:%d**************", stt);
    switch (stt)
    {
    case GSA_SLEEP_OFF:                  // band not wear TODO:add state corresponding actions
        rt_kprintf("\n sleep off \n");
        rt_kprintf("%s\n", ctime(&now));
        break;
    case GSA_SLEEP_WAKE:                 //  Wake->Sleep
        rt_kprintf("\n sleep wake \n");
        rt_kprintf("%s\n", ctime(&now));
        break;
    case GSA_SLEEP_SLEEP:                // 1. Sleep -> Wake  2. Sleep->Deep Sleep
        rt_kprintf("\n light sleep \n");
        rt_kprintf("%s\n", ctime(&now));
        break;
    case GSA_SLEEP_DEEP:                //  1. Deep Sleep->Sleep  2. Deep Sleep -> Wake
        rt_kprintf("\n deep sleep \n");
        rt_kprintf("%s\n", ctime(&now));
        break;
    default:
        rt_kprintf("invalid sleep status:%d\n", stt);
        rt_kprintf("%s\n", ctime(&now));
        break;
    }

}


void pedo_algo_cb(gsa_pedo_info_t *info)
{
    time_t rawtime;
    struct tm *t;

    rawtime = time(RT_NULL);
    /*get GMT TIME*/
    t = gmtime(&rawtime);

    uint8_t offset = (t->tm_hour) * 4 + t->tm_min / 15;
    //uint8_t offset = ((rawtime / 60) % 1440) / 15;
    if (info != NULL)
    {
        rt_kprintf("@cb Steps=%d Distance =%d, Calory=%d, offset = %d \n",
                   info->steps,
                   info->distance,
                   info->calories,
                   offset);
    }
    else
    {
        rt_kprintf("error: algorithm callback failed... \n");
    }

}

static void act_algo_cb(gsa_act_type_e type, uint16 para)
{
    act_cbs[type](para);
}

static void act_lift_hdl(uint16 para)
{
    rt_kprintf("======lift_action_callback=====%d=\n", para);
}

static void act_wave_hdl(uint16 para)
{
    rt_kprintf("~~~~~wave action call back:%d\n", para);

    /* send take photo action */
}

static void act_twist_hdl(uint16 para)
{
    rt_kprintf("======lift_action_callback====%d==\n", para);
}

static usr_prof_t user_profile =
{
    .weight = 120,
    .height = 173 * 2,
    .age = 23,
    .gender = 1,
};

extern uint32_t imc20618_sensor_fifo_cnt_get(void);
extern void imc20618_read_fifo(uint8_t *read_buf, uint32_t len);
extern void imc20618_init(void);

void health_task_entry(void *p_param)
{
    imc20618_init();
    while (true)
    {
        rt_thread_mdelay(500);

        uint8_t read[512];
        uint32_t len = imc20618_sensor_fifo_cnt_get();
        //rt_kprintf("FIFO LEN = %d \n", len);
        imc20618_read_fifo(read, len);

        int16_t x_accs = 0;
        int16_t y_accs = 0;
        int16_t z_accs = 0;

        for (uint32_t i = 0; i < len / 6; i++)
        {
            int16 gsa_accs[3];
            x_accs = ((int16_t)((read[i * 6 + 0] << 8) + read[i * 6 + 1])) >> 2; //right shift for 512 = 1G
            y_accs = ((int16_t)((read[i * 6 + 2] << 8) + read[i * 6 + 3])) >> 2; //right shift for 512 = 1G
            z_accs = ((int16_t)((read[i * 6 + 4] << 8) + read[i * 6 + 5])) >> 2; //right shift for 512 = 1G
            gsa_accs[0] = x_accs;
            gsa_accs[1] = y_accs;
            gsa_accs[2] = z_accs;

            rtk_gsa_fsm(gsa_accs);
        }
        //rt_kprintf("x = %d, y = %d, z = %d \n", x_accs, y_accs, z_accs);

    }
}

int health_algo_init(void)
{
    usr_prof_t      *prof;
    gsa_gs_inf_t    pos;

    /*get user profile from flash in wristband config_data_init*/
    rt_kprintf("f:%d a:%d h%d w%d\r\n", user_profile.gender,
               user_profile.age,
               user_profile.height,
               user_profile.weight);

    prof = (usr_prof_t *)&user_profile;
    pos.xpos = GSENSOR_XPOS;
    pos.zpos = GSENSOR_ZPOS;
    pos.odr  = 25 * 1024;
    rtk_gsa_init(prof, &pos, gsa_cbs);


    gsa_ver_t *rtk_algo_ver;
    rtk_algo_ver = rtk_gsa_get_ver();
    rt_kprintf("health algo version:%d.%d.%d \n", rtk_algo_ver->major,
               rtk_algo_ver->minor, rtk_algo_ver->revision);

//    /*set action detection on/off*/
//    rtk_gsa_act_switch(GSA_ACT_LIFT, true);
//    rtk_gsa_act_switch(GSA_ACT_TWIST, true); // DO NOT modify to xxx.twist_switch_status
//    rtk_gsa_act_switch(GSA_ACT_WAVE, false);

    rt_thread_t tid;

    tid = rt_thread_create("health_task",
                           health_task_entry, RT_NULL,
                           2048, 25, 10);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}

INIT_APP_EXPORT(health_algo_init);

