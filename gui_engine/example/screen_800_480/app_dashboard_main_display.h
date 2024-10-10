/*
 * File      : app_launcher.h
 */
#ifndef __APP_DASHBOARD_MAIN_DISPLAY_H__
#define __APP_DASHBOARD_MAIN_DISPLAY_H__



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
#include "root_image_800_480/ui_resource.h"
#include <gui_app.h>
#include <stdlib.h>
#include "app_dashboard_data.h"

static gui_img_t *dashboard_background = NULL;
static gui_img_t *speed_high_digital = NULL;
static gui_img_t *speed_low_digital = NULL;
static gui_img_t *bluetooth_status = NULL;
static gui_img_t *left_turn_light_status = NULL;
static gui_img_t *right_turn_light_status = NULL;
static gui_img_t *hour_high_digital = NULL;
static gui_img_t *hour_low_digital = NULL;
static gui_img_t *min_high_digital = NULL;
static gui_img_t *min_low_digital = NULL;
static gui_img_t *tense_high_digital = NULL;
static gui_img_t *tense_low_digital = NULL;
static gui_img_t *bat_high_digital = NULL;
static gui_img_t *bat_low_digital = NULL;
static gui_img_t *speed_meter = NULL;
static gui_img_t *tel_box = NULL;
static gui_img_t *tel_accept = NULL;//
static gui_img_t *tel_reject_end = NULL;//
static gui_img_t *ans_button = NULL;
static gui_img_t *refuse_button = NULL;
static gui_img_t *tel_box_left_button = NULL;
static gui_img_t *tel_box_right_button = NULL;
static gui_img_t *short_message = NULL;
static gui_img_t *dashboard_pointer = NULL;

static gui_text_t *short_tel_number = NULL;
static gui_text_t *short_tel_accept = NULL;
static gui_text_t *short_message_data = NULL;

static void *dashboard_pointer_num[25] =
{
    DASHBOARD_2_BIN, DASHBOARD_4_BIN, DASHBOARD_6_BIN, DASHBOARD_8_BIN, DASHBOARD_10_BIN,
    DASHBOARD_12_BIN, DASHBOARD_14_BIN, DASHBOARD_16_BIN, DASHBOARD_18_BIN, DASHBOARD_20_BIN,
    DASHBOARD_22_BIN, DASHBOARD_24_BIN, DASHBOARD_26_BIN, DASHBOARD_28_BIN, DASHBOARD_30_BIN,
    DASHBOARD_32_BIN, DASHBOARD_34_BIN, DASHBOARD_36_BIN, DASHBOARD_38_BIN, DASHBOARD_40_BIN,
    DASHBOARD_42_BIN, DASHBOARD_44_BIN, DASHBOARD_46_BIN, DASHBOARD_48_BIN, DASHBOARD_49_BIN
};

static void *main_display_time_resource_def[] =
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

static void *main_display_battery_resource_def[] =
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

static void *main_display_speed_resource_def[] =
{
    SPED0_BIN,
    SPED1_BIN,
    SPED2_BIN,
    SPED3_BIN,
    SPED4_BIN,
    SPED5_BIN,
    SPED6_BIN,
    SPED7_BIN,
    SPED8_BIN,
    SPED9_BIN,
};

/* Create function */
void app_dashboard_create_main_display(gui_win_t *target_main_display);

/* Update timer information */
void app_dashboard_update_main_display_time_info(void);
void app_dashboard_update_main_display_tense_apm_info(T_TENSE_APM_INFO current_tens_apm_info);
void app_dashboard_refresh_main_display_tense_apm_info(gui_img_t *tense_high_image_obj,
                                                       gui_img_t *tense_low_image_obj,
                                                       T_TENSE_APM_INFO current_tens_apm_info);

/* Update battery information */
void app_dashboard_update_main_display_battery_info(uint8_t battery_level);

/* Update speed information */
void app_dashboard_update_main_display_speed_info(uint8_t current_speed);

/* Update turn information */
void app_dashboard_update_main_display_turn_info(T_TURN_INFO current_turn_information);
void app_dashboard_refresh_main_display_turn_info(gui_img_t *left_turn_image_obj,
                                                  gui_img_t *right_turn_image_obj,
                                                  T_TURN_INFO current_turn_information);

/* Update Bluetooth status */
void app_dashboard_update_main_display_bluetooth_info(T_BLUETOOTH_INFO current_bluetooth_info);
void app_dashboard_refresh_main_display_bluetooth_info(gui_img_t *target_image_obj,
                                                       T_BLUETOOTH_INFO current_bluetooth_info);

/* Update phone information */
void app_dashboard_update_main_display_phone_infor(app_phone_data *app_call_information);
void app_dashboard_update_main_display_message_infor(app_message_data *app_message_information);

/* Update main_display every frame */
void paint_main_display_cb(void *param1, void *param2, struct gui_animate *anim) ;

#ifdef __cplusplus
}
#endif

#endif
