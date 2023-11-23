/*
 * File      : app_cluster_connected_display.h
 */
#ifndef __APP_CLUSTER_CONNECTED_DISPLAY_H__
#define __APP_CLUSTER_CONNECTED_DISPLAY_H__



#ifdef __cplusplus
extern "C" {
#endif
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_seekbar.h>
#include <gui_page.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
//#include "app_dialing.h"
#include "resource_8762g.h"
#include <gui_app.h>
#include <stdlib.h>
#include "acc_engine.h"
#include "app_cluster_data.h"

static gui_img_t *cluster_c_background = NULL;
static gui_img_t *speed_high_c_digital = NULL;
static gui_img_t *speed_low_c_digital = NULL;
static gui_img_t *left_turn_light_c_status = NULL;
static gui_img_t *right_turn_light_c_status = NULL;
static gui_img_t *navi_c_status = NULL;
static gui_img_t *bluetooth_c_status = NULL;
static gui_img_t *ans_c_button = NULL;
static gui_img_t *refuse_c_button = NULL;
static gui_img_t *tel_box_left_c_button = NULL;
static gui_img_t *tel_box_right_c_button = NULL;
static gui_img_t *short_c_message = NULL;
static gui_img_t *hour_high_c_digital = NULL;
static gui_img_t *hour_low_c_digital = NULL;
static gui_img_t *min_high_c_digital = NULL;
static gui_img_t *min_low_c_digital = NULL;
static gui_img_t *tense_high_c_digital = NULL;
static gui_img_t *tense_low_c_digital = NULL;
static gui_img_t *bat_high_c_digital = NULL;
static gui_img_t *bat_low_c_digital = NULL;
static gui_img_t *dashboard_Cpointer = NULL;

static gui_text_t *short_c_navi_message_1 = NULL;
static gui_text_t *short_c_navi_message_2 = NULL;
static gui_text_t *short_c_navi_message_3 = NULL;
static gui_text_t *short_c_tel_number = NULL;
static gui_text_t *short_c_message_data = NULL;
static gui_text_t *short_c_tel_accept = NULL;

static void *dashboard_Cpointer_num[25] =
{
    DASHBOARD_C2_BIN, DASHBOARD_C4_BIN, DASHBOARD_C6_BIN, DASHBOARD_C8_BIN, DASHBOARD_C10_BIN,
    DASHBOARD_C12_BIN, DASHBOARD_C14_BIN, DASHBOARD_C16_BIN, DASHBOARD_C18_BIN, DASHBOARD_C20_BIN,
    DASHBOARD_C22_BIN, DASHBOARD_C24_BIN, DASHBOARD_C26_BIN, DASHBOARD_C28_BIN, DASHBOARD_C30_BIN,
    DASHBOARD_C32_BIN, DASHBOARD_C34_BIN, DASHBOARD_C36_BIN, DASHBOARD_C38_BIN, DASHBOARD_C40_BIN,
    DASHBOARD_C42_BIN, DASHBOARD_C44_BIN, DASHBOARD_C46_BIN, DASHBOARD_C48_BIN, DASHBOARD_C49_BIN
};

static void *connected_display_time_resource_def[] =
{
    TIMER0_BIN,
    TIMER1_BIN,
    TIMER2_BIN,
    TIMER3_BIN,
    TIMER4_BIN,
    TIMER5_BIN,
    TIMER6_BIN,
    TIMER7_BIN,
    TIMER8_BIN,
    TIMER9_BIN,
};

static void *connected_display_battery_resource_def[] =
{
    TIMER0_BIN,
    TIMER1_BIN,
    TIMER2_BIN,
    TIMER3_BIN,
    TIMER4_BIN,
    TIMER5_BIN,
    TIMER6_BIN,
    TIMER7_BIN,
    TIMER8_BIN,
    TIMER9_BIN,
};

static void *connected_display_speed_resource_def[] =
{
    SPED_C0_BIN,
    SPED_C1_BIN,
    SPED_C2_BIN,
    SPED_C3_BIN,
    SPED_C4_BIN,
    SPED_C5_BIN,
    SPED_C6_BIN,
    SPED_C7_BIN,
    SPED_C8_BIN,
    SPED_C9_BIN,
};

static void *connected_display_navi_resource_def[] =
{
    NAVI_C1_BIN,
    NAVI_C2_BIN,
    NAVI_C3_BIN,
    NAVI_C4_BIN,
};

/* Create function */
void app_cluster_create_connected_display(gui_win_t *target_main_display);

/* Update timer information */
void app_cluster_update_connected_display_time_info(void);
void app_cluster_update_connected_display_tense_apm_info(T_TENSE_APM_INFO
                                                         current_tens_apm_info);
void app_cluster_refresh_connected_display_tense_apm_info(gui_img_t *tense_high_image_obj,
                                                          gui_img_t *tense_low_image_obj,
                                                          T_TENSE_APM_INFO current_tens_apm_info);

/* Update battery information */
void app_cluster_update_connected_display_battery_info(uint8_t battery_level);

/* Update speed information */
void app_cluster_update_connected_display_speed_info(uint8_t current_speed);

/* Update turn information */
void app_cluster_update_connected_display_turn_info(T_TURN_INFO current_turn_information);
void app_cluster_refresh_connected_display_turn_info(gui_img_t *left_turn_image_obj,
                                                     gui_img_t *right_turn_image_obj,
                                                     T_TURN_INFO current_turn_information);

/* Update phone information */
void app_cluster_update_connected_display_phone_infor(app_phone_data *app_call_information);

/* Update Navi information */
void app_cluster_update_connected_display_navi_infor(T_NAVI_INFO navi_information);
void app_cluster_update_connected_display_message_infor(app_message_data *app_message_information);

/* Update connected_display every frame */
void paint_connected_display_cb(gui_win_t *win);

#ifdef __cplusplus
}
#endif

#endif
