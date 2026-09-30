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
 * Declaring a key does not register it. An adapter registers only the keys
 * its platform supports; a request for any other key is rejected with
 * GUI_SYSVAL_REQUEST_INVALID.
 *
 * Missing data: a supported key that has no current value answers
 * GUI_SYSVAL_STATUS_ERROR with no value. There are no separate "valid" or
 * "available" keys, and zero is a real value, not a missing-data marker.
 *
 * Units are documented per key and are not repeated in the path
 * (power/battery/remaining_seconds predates this rule). Fixed-point
 * values are integers in the documented unit, for example 365 in tenths of
 * a degree is 36.5 degrees. A state is GUI_SYSVAL_TYPE_STR holding one of
 * the lower-case tokens listed for its key.
 *
 * Strings hold at most GUI_SYSVAL_STR_MAX - 1 bytes. Longer text is
 * truncated by the adapter, on a UTF-8 character boundary.
 *
 * Each request is answered on its own; reading several keys is not an atomic
 * snapshot. A platform may accept only part of a writable range and reject
 * the rest with GUI_SYSVAL_STATUS_INVALID_VALUE.
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

/**
 * Preferred clock display format.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 12 or 24
 * Unit: Hours
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIME_FORMAT "system/time_format"

/**
 * Local offset from UTC, including any daylight saving in effect.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: -720..840
 * Unit: Minutes east of UTC
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIMEZONE "system/timezone"

/**
 * Whether the clock has been set from a trusted source, such as the phone.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_SYSTEM_TIME_SYNCED "system/time_synced"


/*============================================================================*
 *                         System and device
 *============================================================================*/

/**
 * UI language.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: BCP 47 tag, for example "en" or "zh-CN". A set accepts only a
 * supported language and returns the tag that took effect.
 */
#define GUI_SYSVAL_KEY_SYSTEM_LANGUAGE "system/language"

/**
 * Do-not-disturb enable state. Which calls, alarms and notifications it
 * silences is platform-defined.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_SYSTEM_DO_NOT_DISTURB_ENABLED \
    "system/do_not_disturb/enabled"

/**
 * Device display name.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: UTF-8
 */
#define GUI_SYSVAL_KEY_DEVICE_NAME "device/name"

/**
 * Device model name.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: UTF-8
 */
#define GUI_SYSVAL_KEY_DEVICE_MODEL "device/model"

/**
 * Firmware version, in a platform-defined format.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: ASCII
 */
#define GUI_SYSVAL_KEY_DEVICE_FIRMWARE_VERSION "device/firmware_version"


/*============================================================================*
 *                         Power
 *============================================================================*/

/**
 * Remaining battery charge.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
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

/**
 * Battery voltage.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Millivolts
 */
#define GUI_SYSVAL_KEY_POWER_BATTERY_VOLTAGE "power/battery/voltage"

/**
 * Whether external power, such as a charger, is connected. Together with
 * power/battery/charging: connected and not charging means full or powered.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_POWER_EXTERNAL_CONNECTED "power/external/connected"

/**
 * Power-saving mode enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_POWER_SAVING_ENABLED "power/saving/enabled"


/*============================================================================*
 *                         Device settings
 *============================================================================*/

/**
 * Display brightness.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS "display/brightness"

/**
 * Idle time before the display turns off.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..86400
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_DISPLAY_TIMEOUT "display/timeout"

/**
 * Always-on display enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_DISPLAY_ALWAYS_ON_ENABLED "display/always_on/enabled"

/**
 * Lift-to-wake enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_DISPLAY_LIFT_TO_WAKE_ENABLED \
    "display/lift_to_wake/enabled"

/**
 * Audio volume.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
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
 * Whether a Bluetooth bond is stored. A bonded device may be disconnected.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_BONDED \
    "connectivity/bluetooth/bonded"

/**
 * Bluetooth audio link state, independent of the data link.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_AUDIO_CONNECTED \
    "connectivity/bluetooth/audio_connected"

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
 * Range: 1000..100000
 * Unit: Steps
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEP_TARGET \
    "health/pedometer/step_target"

/**
 * Daily active-energy target.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Kilocalories
 */
#define GUI_SYSVAL_KEY_HEALTH_PEDOMETER_CALORIE_TARGET \
    "health/pedometer/calorie_target"

/**
 * Whether the device detects that it is being worn.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_WEARING "health/wearing"

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
 * Lowest valid heart-rate reading today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_DAILY_MIN \
    "health/heart_rate/daily_min"

/**
 * Highest valid heart-rate reading today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_DAILY_MAX \
    "health/heart_rate/daily_max"

/**
 * Time since health/heart_rate was measured.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_HEALTH_HEART_RATE_AGE "health/heart_rate/age"

/**
 * Latest valid blood oxygen measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
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
 * Lowest valid blood oxygen reading today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_DAILY_MIN "health/spo2/daily_min"

/**
 * Highest valid blood oxygen reading today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_DAILY_MAX "health/spo2/daily_max"

/**
 * Time since health/spo2 was measured.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_HEALTH_SPO2_AGE "health/spo2/age"

/**
 * Systolic pressure of the latest valid blood-pressure measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: mmHg
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_SYSTOLIC \
    "health/blood_pressure/systolic"

/**
 * Diastolic pressure of the latest valid blood-pressure measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: mmHg
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_DIASTOLIC \
    "health/blood_pressure/diastolic"

/**
 * Blood-pressure measurement state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_BLOOD_PRESSURE_MEASURING \
    "health/blood_pressure/measuring"

/**
 * Latest valid body temperature reported by the device. Not a clinical
 * core-body measurement.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Unit: 0.1 degree Celsius
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE "health/temperature"

/**
 * Body temperature measurement state. A platform that measures
 * continuously may omit this key.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_TEMPERATURE_MEASURING \
    "health/temperature/measuring"

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

/**
 * Daily sleep target.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_TARGET "health/sleep/target"

/**
 * Whether today's sleep figures are final. False while sleep may still be
 * in progress.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_HEALTH_SLEEP_SUMMARY_COMPLETE \
    "health/sleep/summary_complete"


/*============================================================================*
 *                         Workout
 *============================================================================*/

/*
 * The current or most recently finished session. Every key except
 * workout/state answers GUI_SYSVAL_STATUS_ERROR while the state is "idle".
 * Starting, pausing and finishing a session are not part of this catalog.
 */

/**
 * Workout session state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: "idle", "countdown", "active", "paused" or "finished"
 */
#define GUI_SYSVAL_KEY_WORKOUT_STATE "workout/state"

/**
 * Workout type.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: lower-case ASCII token such as "walking", "running", "cycling"
 * or "strength". Product-specific types use a product prefix.
 */
#define GUI_SYSVAL_KEY_WORKOUT_MODE "workout/mode"

/**
 * Active time, excluding countdown and paused time.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_WORKOUT_DURATION "workout/duration"

/**
 * Total paused time.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_WORKOUT_PAUSE_DURATION "workout/pause_duration"

/**
 * Steps in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Steps
 */
#define GUI_SYSVAL_KEY_WORKOUT_STEPS "workout/steps"

/**
 * Distance in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Metres
 */
#define GUI_SYSVAL_KEY_WORKOUT_DISTANCE "workout/distance"

/**
 * Active energy in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Kilocalories
 */
#define GUI_SYSVAL_KEY_WORKOUT_CALORIES "workout/calories"

/**
 * Average heart rate in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_AVERAGE \
    "workout/heart_rate/average"

/**
 * Lowest heart rate in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_MIN "workout/heart_rate/min"

/**
 * Highest heart rate in the session.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Beats per minute
 */
#define GUI_SYSVAL_KEY_WORKOUT_HEART_RATE_MAX "workout/heart_rate/max"


/*============================================================================*
 *                         Weather
 *============================================================================*/

/*
 * The latest weather report received, usually from the phone. Every key
 * answers GUI_SYSVAL_STATUS_ERROR when there is no report; weather/age tells
 * how old it is.
 */

/**
 * City name.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: UTF-8
 */
#define GUI_SYSVAL_KEY_WEATHER_CITY "weather/city"

/**
 * Weather condition.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: one of "unknown", "clear", "cloudy", "overcast", "rain", "snow",
 * "storm", "fog", "wind", "dust" or "other"
 */
#define GUI_SYSVAL_KEY_WEATHER_CONDITION "weather/condition"

/**
 * Current temperature.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Unit: 0.1 degree Celsius
 */
#define GUI_SYSVAL_KEY_WEATHER_TEMPERATURE "weather/temperature"

/**
 * Forecast high for today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Unit: 0.1 degree Celsius
 */
#define GUI_SYSVAL_KEY_WEATHER_TEMPERATURE_HIGH "weather/temperature_high"

/**
 * Forecast low for today.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Unit: 0.1 degree Celsius
 */
#define GUI_SYSVAL_KEY_WEATHER_TEMPERATURE_LOW "weather/temperature_low"

/**
 * Relative humidity.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_WEATHER_HUMIDITY "weather/humidity"

/**
 * Time since the report was received.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_WEATHER_AGE "weather/age"


/*============================================================================*
 *                         Media
 *============================================================================*/

/**
 * Playback state of the connected music source, such as the phone. Play,
 * pause and skip are not part of this catalog.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: "unavailable", "stopped", "playing" or "paused"
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_STATE "media/music/state"

/**
 * Volume of the connected music source, separate from audio/volume. A set
 * returns the level that took effect after the source's own rounding.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 0..100
 * Unit: Percent
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_VOLUME "media/music/volume"

/**
 * Title of the current track, truncated to fit. Empty when unknown.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: UTF-8
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_TITLE "media/music/title"

/**
 * Artist of the current track, truncated to fit. Empty when unknown.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: UTF-8
 */
#define GUI_SYSVAL_KEY_MEDIA_MUSIC_ARTIST "media/music/artist"


/*============================================================================*
 *                         Notifications and alarms
 *============================================================================*/

/**
 * Number of stored notifications, read or not. The notifications
 * themselves need a separate list interface.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 */
#define GUI_SYSVAL_KEY_NOTIFICATIONS_COUNT "notifications/count"

/**
 * Number of configured alarms, enabled or not. The alarms themselves need a
 * separate list interface.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 */
#define GUI_SYSVAL_KEY_ALARMS_COUNT "alarms/count"


/*============================================================================*
 *                         Stopwatch and countdown
 *============================================================================*/

/*
 * Starting, pausing and resetting a timer are not part of this catalog.
 */

/**
 * Stopwatch state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: "idle", "running" or "paused"
 */
#define GUI_SYSVAL_KEY_STOPWATCH_STATE "stopwatch/state"

/**
 * Stopwatch running time. Zero when "idle".
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Milliseconds
 */
#define GUI_SYSVAL_KEY_STOPWATCH_ELAPSED "stopwatch/elapsed"

/**
 * Countdown state.
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_STR
 * Encoding: "idle", "running", "paused" or "completed"
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_STATE "countdown/state"

/**
 * Countdown length. A set while "running" or "paused" answers
 * GUI_SYSVAL_STATUS_BUSY.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: > 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_DURATION "countdown/duration"

/**
 * Countdown time left. Equals countdown/duration when "idle", zero when
 * "completed".
 *
 * Access: Read-only
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: >= 0
 * Unit: Seconds
 */
#define GUI_SYSVAL_KEY_COUNTDOWN_REMAINING "countdown/remaining"


/*============================================================================*
 *                         Reminders
 *============================================================================*/

/**
 * Sedentary reminder enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_REMINDERS_SEDENTARY_ENABLED \
    "reminders/sedentary/enabled"

/**
 * Sedentary reminder interval. Kept while the reminder is disabled.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_REMINDERS_SEDENTARY_INTERVAL \
    "reminders/sedentary/interval"

/**
 * Drink-water reminder enable state.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_BOOL
 */
#define GUI_SYSVAL_KEY_REMINDERS_DRINK_WATER_ENABLED \
    "reminders/drink_water/enabled"

/**
 * Drink-water reminder interval. Kept while the reminder is disabled.
 *
 * Access: Read/write
 * Type: GUI_SYSVAL_TYPE_INT
 * Range: 1..1440
 * Unit: Minutes
 */
#define GUI_SYSVAL_KEY_REMINDERS_DRINK_WATER_INTERVAL \
    "reminders/drink_water/interval"


#ifdef __cplusplus
}
#endif

#endif
