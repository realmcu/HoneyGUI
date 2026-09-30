/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval_keys.h
 * @brief Standard HoneyGUI system value keys and value contracts.
 *
 * Standard keys use lower-case ASCII path segments separated by '/'. A key
 * describes observable state rather than an action. Each key has one fixed
 * gui_sysval_type_t, listed as "Type" below; a set with any other type is
 * rejected. A successful set returns the value that took effect.
 *
 * Product-specific keys should use a private top-level namespace and must not
 * redefine a standard key.
 */

#ifndef __GUI_SYSVAL_KEYS_H__
#define __GUI_SYSVAL_KEYS_H__

#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                         Common values
 *============================================================================*/

/** Inclusive range used by normalized percentage values. */
#define GUI_SYSVAL_PERCENT_MIN 0
#define GUI_SYSVAL_PERCENT_MAX 100

/** Inclusive range of the standard daily step target. */
#define GUI_SYSVAL_STEP_TARGET_MIN 1000
#define GUI_SYSVAL_STEP_TARGET_MAX 100000


/*============================================================================*
 *                         System clock
 *============================================================================*/

/**
 * Local wall-clock time.
 *
 * Access: Read/write where the platform allows it
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..86399
 * Unit: Seconds since local midnight
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIME "system/time"

/**
 * Local calendar date.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Encoding: YYYYMMDD, for example 20260929
 */
#define GUI_SYSVAL_KEY_SYSTEM_DATE "system/date"


/*============================================================================*
 *                         Power
 *============================================================================*/

/**
 * Remaining battery charge.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: GUI_SYSVAL_PERCENT_MIN..GUI_SYSVAL_PERCENT_MAX
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL "power/battery/level"

/**
 * Battery charging state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING "power/battery/charging"

/**
 * Estimated battery runtime while discharging.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_REMAINING_SECONDS \
    "power/battery/remaining_seconds"


/*============================================================================*
 *                         Device settings
 *============================================================================*/

/**
 * Display brightness.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: GUI_SYSVAL_PERCENT_MIN..GUI_SYSVAL_PERCENT_MAX
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS "display/brightness"

/**
 * Audio volume.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: GUI_SYSVAL_PERCENT_MIN..GUI_SYSVAL_PERCENT_MAX
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_AUDIO_VOLUME "audio/volume"

/**
 * Audio mute state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_AUDIO_MUTED "audio/muted"

/**
 * Haptic feedback enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HAPTICS_ENABLED "haptics/enabled"


/*============================================================================*
 *                         Connectivity
 *============================================================================*/

/**
 * Bluetooth controller enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_ENABLED \
    "connectivity/bluetooth/enabled"

/**
 * Bluetooth link state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_CONNECTED \
    "connectivity/bluetooth/connected"

/**
 * Wi-Fi controller enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED \
    "connectivity/wifi/enabled"

/**
 * Wi-Fi link state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_CONNECTED \
    "connectivity/wifi/connected"


/*============================================================================*
 *                         Health
 *============================================================================*/

/**
 * Steps accumulated today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Steps
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS \
    "health/pedometer/steps"

/**
 * Distance accumulated today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Metres
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_DISTANCE \
    "health/pedometer/distance"

/**
 * Active energy accumulated today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Kilocalories
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_CALORIES \
    "health/pedometer/calories"

/**
 * Daily step target.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: GUI_SYSVAL_STEP_TARGET_MIN..GUI_SYSVAL_STEP_TARGET_MAX
 * Unit: Steps
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEP_TARGET \
    "health/pedometer/step_target"

/**
 * Latest valid heart-rate measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE "health/heart_rate"

/**
 * Heart-rate measurement state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_MEASURING \
    "health/heart_rate/measuring"

/**
 * Latest valid blood oxygen measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: GUI_SYSVAL_PERCENT_MIN..GUI_SYSVAL_PERCENT_MAX
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2 "health/spo2"

/**
 * Blood oxygen measurement state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_MEASURING "health/spo2/measuring"

/**
 * Deep sleep accumulated today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_DEEP "health/sleep/deep"

/**
 * Light sleep accumulated today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_LIGHT "health/sleep/light"


#ifdef __cplusplus
}
#endif

#endif
