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
 * Declaring a key does not register it or guarantee platform support. Ports
 * register only the keys they implement. Unsupported keys are rejected as
 * unknown requests; supported keys without a current value respond with
 * GUI_SYSVAL_STATUS_ERROR and no value. Zero is a valid numeric value where
 * the key's range permits it, not a universal missing-data sentinel.
 *
 * Read-only describes application access. Simulator controls may inject
 * read-only observations through the provider without changing this access.
 * Independent requests do not provide an atomic multi-key snapshot.
 *
 * String limits count payload bytes, excluding the terminating NUL. UTF-8
 * providers must truncate on complete code-point boundaries. The existing
 * value buffer holds at most 31 payload bytes; previews do not promise a
 * complete long message or metadata field.
 *
 * Enum codes are transported as GUI_SYSVAL_TYPE_INT. No new value types or
 * automatic refresh behavior are introduced by this catalog. Fixed-point
 * values are integers in the documented unit. Platforms may accept a subset
 * of a writable range and report rejection using the existing request API.
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

/** Integer codes for charge state. */
#define GUI_SYSVAL_CHARGE_STATE_DISCONNECTED 0
#define GUI_SYSVAL_CHARGE_STATE_CHARGING 1
#define GUI_SYSVAL_CHARGE_STATE_FULL 2
#define GUI_SYSVAL_CHARGE_STATE_POWERED 3

/** Integer codes for measurement state. */
#define GUI_SYSVAL_MEASUREMENT_STATE_IDLE 0
#define GUI_SYSVAL_MEASUREMENT_STATE_MEASURING 1
#define GUI_SYSVAL_MEASUREMENT_STATE_COMPLETED 2
#define GUI_SYSVAL_MEASUREMENT_STATE_FAILED 3
#define GUI_SYSVAL_MEASUREMENT_STATE_TIMEOUT 4
#define GUI_SYSVAL_MEASUREMENT_STATE_NOT_WORN 5

/** Integer codes for workout state. */
#define GUI_SYSVAL_WORKOUT_STATE_IDLE 0
#define GUI_SYSVAL_WORKOUT_STATE_COUNTDOWN 1
#define GUI_SYSVAL_WORKOUT_STATE_ACTIVE 2
#define GUI_SYSVAL_WORKOUT_STATE_PAUSED 3
#define GUI_SYSVAL_WORKOUT_STATE_FINISHED 4

/** Integer codes for weather state. */
#define GUI_SYSVAL_WEATHER_STATE_UNAVAILABLE 0
#define GUI_SYSVAL_WEATHER_STATE_CURRENT 1
#define GUI_SYSVAL_WEATHER_STATE_EXPIRED 2
#define GUI_SYSVAL_WEATHER_STATE_DISABLED 3

/** Integer codes for music state. */
#define GUI_SYSVAL_MUSIC_STATE_UNAVAILABLE 0
#define GUI_SYSVAL_MUSIC_STATE_STOPPED 1
#define GUI_SYSVAL_MUSIC_STATE_PLAYING 2
#define GUI_SYSVAL_MUSIC_STATE_PAUSED 3

/** Integer codes for stopwatch state. */
#define GUI_SYSVAL_STOPWATCH_STATE_IDLE 0
#define GUI_SYSVAL_STOPWATCH_STATE_RUNNING 1
#define GUI_SYSVAL_STOPWATCH_STATE_PAUSED 2
#define GUI_SYSVAL_STOPWATCH_STATE_LIMIT_REACHED 3

/** Integer codes for countdown state. */
#define GUI_SYSVAL_COUNTDOWN_STATE_IDLE 0
#define GUI_SYSVAL_COUNTDOWN_STATE_RUNNING 1
#define GUI_SYSVAL_COUNTDOWN_STATE_PAUSED 2
#define GUI_SYSVAL_COUNTDOWN_STATE_COMPLETED 3


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


/*============================================================================*
 * System, device, power and display contracts
 *============================================================================*/

/**
 * Preferred clock display format.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 12 or 24
 * Unit: Hour format
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIME_FORMAT "system/time_format"

/**
 * Configured local UTC offset.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: -720..840
 * Unit: Minutes east of UTC
 * This is the effective offset, including any platform-applied daylight
 * saving. It does not change the encoding of system/time or system/date.
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIMEZONE_MINUTES "system/timezone_minutes"

/**
 * Whether the wall clock has an accepted synchronization source.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * False does not make the numeric time or date unreadable.
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIME_SYNCED "system/time_synced"

/**
 * Selected UI language tag.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: ASCII BCP 47 tag, for example en or zh-CN. Platforms accept
 * only supported languages and return the tag that took effect.
 */
#define GUI_SYSVAL_KEY_SYSTEM_LANGUAGE "system/language"

/**
 * Display name of the device.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: UTF-8. This is a bounded display label; it is not an
 * authentication identity.
 */
#define GUI_SYSVAL_KEY_DEVICE_NAME "device/name"

/**
 * Display model identifier.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: UTF-8.
 */
#define GUI_SYSVAL_KEY_DEVICE_MODEL "device/model"

/**
 * Display firmware version.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: ASCII. The format is platform-specific; clients must not assume
 * semantic versioning.
 */
#define GUI_SYSVAL_KEY_DEVICE_FIRMWARE_VERSION "device/firmware_version"

/**
 * Latest valid battery voltage.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Millivolts
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_VOLTAGE_MV "power/battery/voltage_mv"

/**
 * External-power and charging state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..3 (GUI_SYSVAL_CHARGE_STATE_*)
 * Unit: Charge state code
 * When both keys are supported, power/battery/charging is true exactly for
 * GUI_SYSVAL_CHARGE_STATE_CHARGING. Full and externally powered without
 * charging are separate states.
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_CHARGE_STATE "power/battery/charge_state"

/**
 * Power-saving mode enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 */
#define GUI_SYSVAL_KEY_POWER_SAVING_ENABLED "power/saving_enabled"

/**
 * Idle timeout before the active display is turned off.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..86400
 * Unit: Seconds
 * Always-on display is controlled separately. The platform may support only
 * a subset of these timeout values.
 */
#define GUI_SYSVAL_KEY_DISPLAY_TIMEOUT_SECONDS "display/timeout_seconds"

/**
 * Always-on display enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 */
#define GUI_SYSVAL_KEY_DISPLAY_ALWAYS_ON_ENABLED "display/always_on_enabled"

/**
 * Lift-to-wake enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 */
#define GUI_SYSVAL_KEY_DISPLAY_LIFT_TO_WAKE_ENABLED "display/lift_to_wake_enabled"

/**
 * Whether a Bluetooth bond is stored.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * Bonding and an active link are independent states; a disconnected device
 * can remain bonded.
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_BONDED "connectivity/bluetooth/bonded"

/**
 * Whether the platform's Bluetooth audio link is connected.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * This does not imply that the application's BLE data link is connected.
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_AUDIO_CONNECTED \
    "connectivity/bluetooth/audio_connected"

/**
 * Do-not-disturb mode enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * Specific policies for calls, alarms and notifications remain
 * platform-defined.
 */
#define GUI_SYSVAL_KEY_SYSTEM_DO_NOT_DISTURB_ENABLED "system/do_not_disturb_enabled"


/*============================================================================*
 * Health observations and lifecycle contracts
 *============================================================================*/

/**
 * Daily active-energy target.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Kilocalories
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_CALORIE_TARGET "health/pedometer/calorie_target"

/**
 * Whether the device currently detects that it is being worn.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * Register this key only when the platform supports wear detection.
 */
#define GUI_SYSVAL_KEY_HEALTH_WEARING "health/wearing"

/**
 * Whether a valid retained heart-rate reading is available.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * False means that the associated reading and age are unavailable. A failed
 * new measurement may retain a previous valid reading.
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_VALID "health/heart_rate/valid"

/**
 * Current heart-rate measurement lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..5 (GUI_SYSVAL_MEASUREMENT_STATE_*)
 * Unit: Measurement state code
 * When both keys are supported, the existing measuring boolean is true
 * exactly for GUI_SYSVAL_MEASUREMENT_STATE_MEASURING. This state does not
 * indicate the freshness of a retained reading.
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_STATE "health/heart_rate/state"

/**
 * Lowest valid heart-rate reading for the current local calendar day.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Beats per minute
 * If no valid daily samples exist, this reading is unavailable; do not
 * publish an empty-data sentinel as a sample.
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_DAILY_MIN "health/heart_rate/daily_min"

/**
 * Highest valid heart-rate reading for the current local calendar day.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Beats per minute
 * If no valid daily samples exist, this reading is unavailable; do not
 * publish an empty-data sentinel as a sample.
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_DAILY_MAX "health/heart_rate/daily_max"

/**
 * Elapsed time since the retained heart-rate sample was measured.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * Use elapsed time rather than subtracting mutable wall-clock values. This
 * key is unavailable when the associated valid flag is false.
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_AGE_SECONDS "health/heart_rate/age_seconds"

/**
 * Whether a valid retained blood-oxygen reading is available.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * False means that the associated reading and age are unavailable. A failed
 * new measurement may retain a previous valid reading.
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_VALID "health/spo2/valid"

/**
 * Current blood-oxygen measurement lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..5 (GUI_SYSVAL_MEASUREMENT_STATE_*)
 * Unit: Measurement state code
 * When both keys are supported, the existing measuring boolean is true
 * exactly for GUI_SYSVAL_MEASUREMENT_STATE_MEASURING. This state does not
 * indicate the freshness of a retained reading.
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_STATE "health/spo2/state"

/**
 * Lowest valid blood-oxygen reading for the current local calendar day.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 * If no valid daily samples exist, this reading is unavailable; do not
 * publish an empty-data sentinel as a sample.
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_DAILY_MIN "health/spo2/daily_min"

/**
 * Highest valid blood-oxygen reading for the current local calendar day.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 * If no valid daily samples exist, this reading is unavailable; do not
 * publish an empty-data sentinel as a sample.
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_DAILY_MAX "health/spo2/daily_max"

/**
 * Elapsed time since the retained blood-oxygen sample was measured.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * Use elapsed time rather than subtracting mutable wall-clock values. This
 * key is unavailable when the associated valid flag is false.
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_AGE_SECONDS "health/spo2/age_seconds"

/**
 * Systolic component of the latest valid blood-pressure result.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Millimetres of mercury
 * The two pressure components describe one retained measurement.
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_SYSTOLIC "health/blood_pressure/systolic"

/**
 * Diastolic component of the latest valid blood-pressure result.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Millimetres of mercury
 * The two pressure components describe one retained measurement.
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_DIASTOLIC "health/blood_pressure/diastolic"

/**
 * Whether a valid retained blood-pressure result is available.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * False means that both pressure components are unavailable.
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_VALID "health/blood_pressure/valid"

/**
 * Blood-pressure measurement enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * True requests measurement; false requests stopping it. A successful set
 * reports the enable state that took effect.
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_MEASURING "health/blood_pressure/measuring"

/**
 * Current blood-pressure measurement lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..5 (GUI_SYSVAL_MEASUREMENT_STATE_*)
 * Unit: Measurement state code
 * When both keys are supported, measuring is true exactly for
 * GUI_SYSVAL_MEASUREMENT_STATE_MEASURING.
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_STATE "health/blood_pressure/state"

/**
 * Latest valid device-reported health temperature.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: INT32_MIN..INT32_MAX
 * Unit: One tenth of a degree Celsius
 * Encoding: fixed-point integer; 365 means 36.5 degrees Celsius.
 * Measurement site and compensation are platform-defined and must not be
 * inferred as a clinical core-body measurement.
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE_DECI_CELSIUS "health/temperature/deci_celsius"

/**
 * Whether a valid retained health temperature is available.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * False means that the associated temperature is unavailable.
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE_VALID "health/temperature/valid"

/**
 * Health-temperature measurement enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * A platform with continuous passive reporting may omit this writable
 * control.
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE_MEASURING "health/temperature/measuring"

/**
 * Current health-temperature measurement lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..5 (GUI_SYSVAL_MEASUREMENT_STATE_*)
 * Unit: Measurement state code
 * When both keys are supported, measuring is true exactly for
 * GUI_SYSVAL_MEASUREMENT_STATE_MEASURING.
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE_STATE "health/temperature/state"

/**
 * Deep plus light sleep accumulated for the same day as the existing sleep
 * keys.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Minutes
 * This total is exactly health/sleep/deep plus health/sleep/light; other
 * stages are outside this contract.
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_TOTAL "health/sleep/total"

/**
 * Daily sleep-duration target.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_TARGET "health/sleep/target"

/**
 * Whether the current daily sleep summary is available.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * An available zero-minute summary differs from a missing summary.
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_AVAILABLE "health/sleep/available"

/**
 * Whether the available daily sleep summary is finalized.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 * This is a summary lifecycle flag, not a sensor-measurement success flag.
 * False when health/sleep/available is false.
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_SUMMARY_COMPLETE "health/sleep/summary_complete"


/*============================================================================*
 * Workout and weather contracts
 *============================================================================*/

/**
 * Mode of the current or most recently completed workout.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: lower-case ASCII token; common values include walking, running,
 * cycling and strength. Private modes use a product-prefixed token.
 * Unavailable when workout/state is IDLE.
 */
#define GUI_SYSVAL_KEY_WORKOUT_MODE "workout/mode"

/**
 * Current workout session state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..4 (GUI_SYSVAL_WORKOUT_STATE_*)
 * Unit: Workout state code
 * Starting, pausing, resuming and finishing a session are actions outside
 * this key catalog.
 */
#define GUI_SYSVAL_KEY_WORKOUT_STATE "workout/state"

/**
 * Active duration of the session, excluding countdown and paused time.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * These session values are unavailable in IDLE and remain readable in
 * FINISHED until the next session replaces them.
 */
#define GUI_SYSVAL_KEY_WORKOUT_ELAPSED_SECONDS "workout/elapsed_seconds"

/**
 * Accumulated paused duration of the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * These session values are unavailable in IDLE and remain readable in
 * FINISHED until the next session replaces them.
 */
#define GUI_SYSVAL_KEY_WORKOUT_PAUSED_SECONDS "workout/paused_seconds"

/**
 * Steps accumulated during the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Steps
 * These session values are unavailable in IDLE and remain readable in
 * FINISHED until the next session replaces them.
 */
#define GUI_SYSVAL_KEY_WORKOUT_STEPS "workout/steps"

/**
 * Distance accumulated during the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Metres
 * These session values are unavailable in IDLE and remain readable in
 * FINISHED until the next session replaces them.
 */
#define GUI_SYSVAL_KEY_WORKOUT_DISTANCE "workout/distance"

/**
 * Active energy accumulated during the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Kilocalories
 * These session values are unavailable in IDLE and remain readable in
 * FINISHED until the next session replaces them.
 */
#define GUI_SYSVAL_KEY_WORKOUT_CALORIES "workout/calories"

/**
 * Mean of valid session heart-rate samples.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Beats per minute
 * Unavailable in IDLE or if the session has no valid heart-rate samples.
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_AVERAGE "workout/heart_rate_average"

/**
 * Lowest valid session heart-rate sample.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Beats per minute
 * Unavailable in IDLE or if the session has no valid heart-rate samples.
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_MIN "workout/heart_rate_min"

/**
 * Highest valid session heart-rate sample.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Beats per minute
 * Unavailable in IDLE or if the session has no valid heart-rate samples.
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_MAX "workout/heart_rate_max"

/**
 * Availability and freshness of the current weather record.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..3 (GUI_SYSVAL_WEATHER_STATE_*)
 * Unit: Weather state code
 * CURRENT means usable under the platform's freshness policy. EXPIRED
 * preserves a stale record; UNAVAILABLE and DISABLED make record fields
 * unavailable.
 */
#define GUI_SYSVAL_KEY_WEATHER_STATE "weather/state"

/**
 * City display label of the retained weather record.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: UTF-8. Readable in CURRENT and EXPIRED.
 */
#define GUI_SYSVAL_KEY_WEATHER_CITY "weather/city"

/**
 * Normalized condition of the retained weather record.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: ASCII token from unknown, clear, cloudy, overcast, rain, snow,
 * storm, fog, wind, dust or other. Platform-specific wire codes must be
 * mapped explicitly. Readable in CURRENT and EXPIRED.
 */
#define GUI_SYSVAL_KEY_WEATHER_CONDITION "weather/condition"

/**
 * Current weather temperature.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: INT32_MIN..INT32_MAX
 * Unit: One tenth of a degree Celsius
 * Negative values are valid. Readable in CURRENT and EXPIRED; high must be
 * at least low.
 */
#define GUI_SYSVAL_KEY_WEATHER_TEMPERATURE_DECI_CELSIUS "weather/temperature_deci_celsius"

/**
 * Forecast high for the retained record's local date.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: INT32_MIN..INT32_MAX
 * Unit: One tenth of a degree Celsius
 * Negative values are valid. Readable in CURRENT and EXPIRED; high must be
 * at least low.
 */
#define GUI_SYSVAL_KEY_WEATHER_HIGH_DECI_CELSIUS "weather/high_deci_celsius"

/**
 * Forecast low for the retained record's local date.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: INT32_MIN..INT32_MAX
 * Unit: One tenth of a degree Celsius
 * Negative values are valid. Readable in CURRENT and EXPIRED; high must be
 * at least low.
 */
#define GUI_SYSVAL_KEY_WEATHER_LOW_DECI_CELSIUS "weather/low_deci_celsius"

/**
 * Relative humidity of the retained weather record.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 * Unavailable when the record omits humidity. Zero is not a missing-data
 * sentinel.
 */
#define GUI_SYSVAL_KEY_WEATHER_HUMIDITY "weather/humidity"

/**
 * Elapsed time since the retained weather record was received.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * Readable in CURRENT and EXPIRED. Age alone does not prescribe a global
 * expiration threshold.
 */
#define GUI_SYSVAL_KEY_WEATHER_AGE_SECONDS "weather/age_seconds"


/*============================================================================*
 * Media, counts, timers and reminder contracts
 *============================================================================*/

/**
 * Playback state of the connected music source.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..3 (GUI_SYSVAL_MUSIC_STATE_*)
 * Unit: Music state code
 * This is read-only state; play, pause, previous and next are actions
 * outside this key catalog.
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_STATE "media/music/state"

/**
 * Actual confirmed volume of the connected music source.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 * This is separate from device-local audio/volume. A ten-level source maps
 * level 0..10 to percent 0..100; successful writes return the applied
 * value, including platform quantization.
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_VOLUME "media/music/volume"

/**
 * Bounded preview of the current music title.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: UTF-8. Empty if no title is available. This key does not
 * promise the complete metadata text.
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_TITLE_PREVIEW "media/music/title_preview"

/**
 * Bounded preview of the current music artist.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Range: At most 31 payload bytes
 * Unit: None
 * Encoding: UTF-8. Empty if no artist is available. This key does not
 * promise the complete metadata text.
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_ARTIST_PREVIEW "media/music/artist_preview"

/**
 * Number of retained notification records.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Records
 * This is a stored-record count, not an unread count. Message contents and
 * ordering require a separate collection interface.
 */
#define GUI_SYSVAL_KEY_NOTIFICATIONS_COUNT "notifications/count"

/**
 * Number of configured alarm records, including disabled records.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Records
 * Alarm items and repeat masks require a separate collection interface.
 */
#define GUI_SYSVAL_KEY_ALARMS_COUNT "alarms/count"

/**
 * Stopwatch lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..3 (GUI_SYSVAL_STOPWATCH_STATE_*)
 * Unit: Stopwatch state code
 * Start, pause, resume and reset are actions outside this key catalog.
 */
#define GUI_SYSVAL_KEY_STOPWATCH_STATE "stopwatch/state"

/**
 * Accumulated running time of the stopwatch.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Milliseconds
 * Zero in IDLE. At INT32_MAX, a provider stops accumulation and reports
 * LIMIT_REACHED rather than wrapping.
 */
#define GUI_SYSVAL_KEY_STOPWATCH_ELAPSED_MS "stopwatch/elapsed_ms"

/**
 * Countdown lifecycle.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..3 (GUI_SYSVAL_COUNTDOWN_STATE_*)
 * Unit: Countdown state code
 * Start, pause, resume and cancel are actions outside this key catalog.
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_STATE "countdown/state"

/**
 * Configured duration of the countdown.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..INT32_MAX
 * Unit: Seconds
 * A provider can accept changes only in IDLE or COMPLETED; requests in
 * RUNNING or PAUSED report GUI_SYSVAL_STATUS_BUSY.
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_DURATION_SECONDS "countdown/duration_seconds"

/**
 * Remaining time in the countdown.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..INT32_MAX
 * Unit: Seconds
 * Zero in COMPLETED. In IDLE, equal to the configured duration.
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_REMAINING_SECONDS "countdown/remaining_seconds"

/**
 * Sedentary reminder enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 */
#define GUI_SYSVAL_KEY_REMINDERS_SEDENTARY_ENABLED "reminders/sedentary/enabled"

/**
 * Sedentary reminder interval.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 * The interval is retained while disabled. Active windows and weekday
 * schedules are outside this scalar contract.
 */
#define GUI_SYSVAL_KEY_REMINDERS_SEDENTARY_INTERVAL_MINUTES "reminders/sedentary/interval_minutes"

/**
 * Drink-water reminder enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 * Range: false or true
 * Unit: None
 */
#define GUI_SYSVAL_KEY_REMINDERS_DRINK_WATER_ENABLED "reminders/drink_water/enabled"

/**
 * Drink-water reminder interval.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 * The interval is retained while disabled. Active windows and weekday
 * schedules are outside this scalar contract.
 */
#define GUI_SYSVAL_KEY_REMINDERS_DRINK_WATER_INTERVAL_MINUTES \
    "reminders/drink_water/interval_minutes"


#ifdef __cplusplus
}
#endif

#endif
