/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <gap_bond_le.h>
#include <app_msg.h>
#include <bt_task.h>
#include "board.h"
#include "trace.h"
#include "ble_gap_init.h"
#include "ble_profile_init.h"
#include "bt_gap_init.h"
#if (RTK_BT_TASK == 1)
#include "btm.h"
#include "sysm.h"
#include "remote.h"
#endif
#include "rtl_pinmux.h"
#include <watch_msg.h>
#ifdef __RTTHREAD__
#include "rtthread.h"
#else
#include "menu_config.h"
#endif

#define ADD_DSP_JTAG_PINMUX   0

/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define BLE_TASK_PRIORITY             1         //!< Task priorities
#define BLE_TASK_STACK_SIZE           1024 * 8   //!<  Task stack size
#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define MAX_NUMBER_OF_APP_TIMER       0x30    //!< indicate app timer queue size
#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

#define PLAYBACK_POOL_SIZE                  (10*1024)
#define VOICE_POOL_SIZE                     (2*1024)
#define RECORD_POOL_SIZE                    (1*1024)
#define NOTIFICATION_POOL_SIZE              (3*1024)

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *app_task_handle;   //!< APP Task handle
void *evt_queue_handle;  //!< Event queue handle
void *io_queue_handle;   //!< IO queue handle







bool app_send_msg_to_bt_task(T_IO_MSG *p_msg)
{
    uint8_t event = EVENT_IO_TO_APP;
    if (os_msg_send(io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail io queue");
        return false;
    }
    if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        APP_PRINT_ERROR0("send_evt_msg_to_app fail event queue");
        return false;
    }
    return true;
}

#if (RTK_BT_TASK == 1)
/**
 * @brief    Contains the initialization of framework
 * @return   void
 */
static void framework_init(void)
{
    /* System Manager */
    if (sys_mgr_init(evt_queue_handle) == true)
    {
        APP_PRINT_INFO0("System manager was initialized successfully!\n");
    }
    else
    {
        APP_PRINT_ERROR0("System manager was failed to initialize!\n");
    }

    /* RemoteController Manager */
    if (remote_mgr_init(REMOTE_SESSION_ROLE_SINGLE) == true)
    {
        APP_PRINT_INFO0("Remote controll manager was initialized successfully!\n");
    }
    else
    {
        APP_PRINT_ERROR0("Remote controll manager was failed to initialize!\n");
    }

    /* Bluetooth Manager */
    if (bt_mgr_init() == true)
    {
        APP_PRINT_INFO0("Bluetooth manager was initialized successfully!\n");
    }
    else
    {
        APP_PRINT_ERROR0("Bluetooth manager was failed to initialize!\n");
    }

    /* Audio Manager */
    if (audio_mgr_init(PLAYBACK_POOL_SIZE, VOICE_POOL_SIZE, RECORD_POOL_SIZE,
                       NOTIFICATION_POOL_SIZE) == true)
    {
        APP_PRINT_INFO0("Audio manager was initialized successfully!\n");
    }
    else
    {
        APP_PRINT_ERROR0("Audio manager was failed to initialize!\n");
    }
}
#endif


/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void bt_task_entry(void *p_param)
{
    uint8_t event;
    os_alloc_secure_ctx(1024);

    os_msg_queue_create(&io_queue_handle, "ioQ", MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&evt_queue_handle, "evtQ", MAX_NUMBER_OF_EVENT_MESSAGE,
                        sizeof(uint8_t));
#if (RTK_BT_TASK == 1)
    app_init_timer(evt_queue_handle, MAX_NUMBER_OF_APP_TIMER);
#endif
    le_gap_init(1);
    gap_lib_init();
    app_le_gap_init();
    app_le_profile_init();
#if 0
    app_cfg_init();
#endif
#if (RTK_BT_TASK == 1)
    framework_init();
    watch_bt_gap_init();
    watch_app_gap_init();
#endif
#ifdef MODULE_LOCAL_PLAYBACK
    app_playback_init();
#endif
#ifdef BT_PROFILE_SDP
    app_sdp_init();
#endif
#ifdef BT_PROFILE_SPP
    app_spp_init();
#endif
#ifdef BT_PROFILE_A2DP
    // to do fix this
    app_a2dp_init();
#endif
#ifdef BT_PROFILE_AVRCP
    app_avrcp_init();
#endif
#ifdef BT_PROFILE_PBAP
    app_pbap_init();
#endif
#ifdef BT_PROFILE_HFP
    app_hfp_init();
#endif

    gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

#if (ADD_DSP_JTAG_PINMUX == 1)
    extern void dsp_load_test(void);
    extern void start_vp(void);
    extern void stop_vp(void);
    extern void start_audio(void);
    extern void start_tts(void);
    // to do fix this
    // set DSP JTAG pinmux
    //set PMUX_PAD_P10_0 ~ 4 -> 140 ~ 144
    //w4 0x400E2450 8f8e8d8c
    //*(volatile uint32_t *)0x400E2450 = 0x8F8E8D8C;
    ////w4 0x400E2454 90
    //*(volatile uint32_t *)0x400E2454 = 0x90;
    ////PAD vcore2 & pull up
    ////w4 0x400019A0 30C130C1
    //*(volatile uint32_t *)0x400019A0 = 0x30C130C1;
    ////w4 0x400019A4 30C130C1
    //*(volatile uint32_t *)0x400019A4 = 0x30C130C1;
    ////w4 0x400019A8 308130C1
    //*(volatile uint32_t *)0x400019A8 = 0x30C130C1;
    Pinmux_Config(P10_0, DSP_JTCK);
    Pinmux_Config(P10_1, DSP_JTDI);
    Pinmux_Config(P10_2, DSP_JTDO);
    Pinmux_Config(P10_3, DSP_JTMS);
    Pinmux_Config(P10_4, DSP_JTRST);
    //for vcore2 domain
    Pad_Config(P10_0, PAD_V2_V4_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P10_1, PAD_V2_V4_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P10_2, PAD_V2_V4_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P10_3, PAD_V2_V4_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P10_4, PAD_V2_V4_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    DBG_DIRECT("set dsp jtag 0x%X, 0x%X, 0x%X, 0x%X, 0x%X", *(volatile uint32_t *)0x400E2450,
               *(volatile uint32_t *)0x400E2454, *(volatile uint32_t *)0x400019A0,
               *(volatile uint32_t *)0x400019A4, *(volatile uint32_t *)0x400019A8);

    //start_audio();
    //start_tts();
#endif

    while (true)
    {
        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
                {
                    app_handle_io_msg(io_msg);
                }
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_STACK)
            {
                gap_handle_msg(event);
            }
#if (RTK_BT_TASK == 1)
            else if (EVENT_GROUP(event) == EVENT_GROUP_FRAMEWORK)
            {
                sys_mgr_event_handle(event);
            }
#endif
        }
    }
}

void bt_task_init(void)
{
    os_task_create(&app_task_handle, "bt_task", bt_task_entry, 0, BLE_TASK_STACK_SIZE, 2);
}

/** @} */ /* End of group PERIPH_APP_TASK */


