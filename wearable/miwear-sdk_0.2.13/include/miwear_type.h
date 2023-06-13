#ifndef __MIWEAR_TYPE_H__
#define __MIWEAR_TYPE_H__

#include "wear.pb.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIWEAR_LOG_LEVEL_NONE 0
#define MIWEAR_LOG_LEVEL_ERROR 1
#define MIWEAR_LOG_LEVEL_WARNING 2
#define MIWEAR_LOG_LEVEL_INFO 3
#define MIWEAR_LOG_LEVEL_DEBUG 4
#define MIWEAR_LOG_LEVEL_VERBOSE 5

typedef enum
{
    P256R1
} miwear_ecc_curve_t;

typedef uint8_t(miwear_ecc256_sk_t)[256 / 8];
typedef uint8_t(miwear_ecc256_pk_t)[256 / 8 * 2];
typedef uint8_t(miwear_ecc256_ss_t)[256 / 8];

typedef uint8_t(miwear_aes128_t)[128 / 8];

#define MIWEAR_VERIFY_MODE_PSK 0x00
#define MIWEAR_VERIFY_MODE_LOCAL 0x01

#define MIWEAR_OOB_MODE_NO_OOB 0x00
#define MIWEAR_OOB_MODE_NUMERIC_COMPARISON 0x01
#define MIWEAR_OOB_MODE_DYNAMIC_CODE 0x02
#define MIWEAR_OOB_MODE_BUTTON_CONFIRM 0x04

#define MIWEAR_GAP_EVT_BASE 0x00
#define MIWEAR_GATTS_EVT_BASE 0x40
#define MIWEAR_GATTC_EVT_BASE 0x80

#define MIWEAR_BLE_UUID_MI_SERVICE 0xFE95 /**The UUID of the Xiaomi Service. */
#define MIWEAR_BLE_UUID_UNLOCK_SERVICE 0xFDAB /**The UUID of the Unlock Service*/

#define MIWEAR_MI_SERVICE_BLE_UUID_VERSION 0x0050
#define MIWEAR_MI_SERVICE_BLE_UUID_PROTO_TX 0x0051 /**The UUID of the Proto TX Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_PROTO_RX 0x0052 /**The UUID of the Proto RX Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_FITNESS 0x0053 /**The UUID of the FITNESS Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_VOICE 0x0054 /**The UUID of the Voice Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_MASS 0x0055 /**The UUID of the Mass Data Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_OTA_TX 0x0056 /**The UUID of the OTA TX Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_OTA_RX 0x0057 /**The UUID of the OTA RX Characteristic. */
#define MIWEAR_MI_SERVICE_BLE_UUID_STAT 0x0058
#define MIWEAR_MI_SERVICE_BLE_UUID_LOG 0x0059
#define MIWEAR_MI_SERVICE_BLE_UUID_SENSOR_DATA 0x005A /**The UUID of the Sensor data Characteristic. */

#define MIWEAR_MI_SERVICE_MAX_CHAR_COUNT 11 /**The max charateristic count which Xiaomi Service can support in SDK*/

#define MIWEAR_UNLOCK_SERVICE_BLE_UUID_VERSION 0x0001
#define MIWEAR_UNLOCK_SERVICE_BLE_UUID_AUTHORIZE 0x0002
#define MIWEAR_UNLOCK_SERVICE_BLE_UUID_AUTHENTICATE 0x0003
#define MIWEAR_UNLOCK_SERVICE_CHAR_COUNT 3

typedef uint8_t miwear_addr_t[6];

typedef uint32_t miwear_cfm_t;

typedef enum
{
    MIWEAR_MI_SERVICE,
    MIWEAR_UNLOCK_SERVICE,
} miwear_ble_service_index_t;

typedef enum
{
    MIWEAR_MI_SERVICE_CHAR_VERSION = 0,
    MIWEAR_MI_SERVICE_CHAR_PROTO_TX,
    MIWEAR_MI_SERVICE_CHAR_PROTO_RX,
    MIWEAR_MI_SERVICE_CHAR_FITNESS,
    MIWEAR_MI_SERVICE_CHAR_VOICE,
    MIWEAR_MI_SERVICE_CHAR_MASS,
    MIWEAR_MI_SERVICE_CHAR_OTA_TX,
    MIWEAR_MI_SERVICE_CHAR_OTA_RX,
    MIWEAR_MI_SERVICE_CHAR_STAT,
    MIWEAR_MI_SERVICE_CHAR_LOG,
    MIWEAR_MI_SERVICE_CHAR_SENSOR_DATA,
    MIWEAR_UNLOCK_SERVICE_CHAR_VERSION,
    MIWEAR_UNLOCK_SERVICE_CHAR_AUTHORIZE,
    MIWEAR_UNLOCK_SERVICE_CHAR_AUTHENTICATE,
    MIWEAR_BLE_MAX_CHAR_COUNT, /**The max charateristic count of all Service can support in SDK*/
} miwear_ble_char_index_t;

/*
 * 1. The spp channel number must be synchronized with miwear_ble_char_index_t
 *    in order to be processed together
 * 2. If there are new additions in the future, spp and ble need to be kept in
 *    sync
 * 3. SPP does not support unlock service
 */
typedef enum
{
    MIWEAR_SPP_CHANNEL_VERSION = 0,
    MIWEAR_SPP_CHANNEL_PROTO_TX,
    MIWEAR_SPP_CHANNEL_PROTO_RX,
    MIWEAR_SPP_CHANNEL_FITNESS,
    MIWEAR_SPP_CHANNEL_VOICE,
    MIWEAR_SPP_CHANNEL_MASS,
    MIWEAR_SPP_CHANNEL_OTA_TX,
    MIWEAR_SPP_CHANNEL_OTA_RX,
    MIWEAR_SPP_CHANNEL_STAT,
    MIWEAR_SPP_CHANNEL_LOG,
    MIWEAR_SPP_CHANNEL_SENSOR_DATA,
    MIWEAR_SPP_CHANNEL_MAX
} miwear_spp_channel_t;

typedef struct
{
    uint16_t begin_handle;
    uint16_t end_handle;
} miwear_handle_range_t;

typedef enum
{
    MIWEAR_ADDRESS_TYPE_PUBLIC, // public address
    MIWEAR_ADDRESS_TYPE_RANDOM, // random address
} miwear_addr_type_t;

/* GAP related */
typedef struct
{
    uint16_t scan_interval; // Range: 0x0004 to 0x4000 Time = N * 0.625 msec Time Range: 2.5 msec to 10.24 sec
    uint16_t scan_window; // Range: 0x0004 to 0x4000 Time = N * 0.625 msec Time Range: 2.5 msec to 10.24 seconds
    uint16_t timeout; // Scan timeout between 0x0001 and 0xFFFF in seconds, 0x0000 disables timeout.
} miwear_gap_scan_param_t;

typedef enum
{
    MIWEAR_ADV_DATA, // advertising data
    MIWEAR_SCAN_RSP_DATA, // response data from active scanning
} miwear_gap_adv_data_type_t;

typedef struct
{
    miwear_addr_t peer_addr;
    miwear_addr_type_t addr_type;
    miwear_gap_adv_data_type_t adv_type;
    int8_t rssi;
    uint8_t data[31];
    uint16_t data_len;
} miwear_gap_adv_report_t;

typedef enum
{
    MIWEAR_CONNECTION_TIMEOUT = 1,
    MIWEAR_REMOTE_USER_TERMINATED,
    MIWEAR_LOCAL_HOST_TERMINATED
} miwear_gap_disconnect_reason_t;

typedef struct
{
    uint16_t min_conn_interval; // Range: 0x0006 to 0x0C80, Time = N * 1.25 msec, Time Range: 7.5 msec to 4 seconds.
    uint16_t max_conn_interval; // Range: 0x0006 to 0x0C80, Time = N * 1.25 msec, Time Range: 7.5 msec to 4 seconds.
    uint16_t slave_latency; // Range: 0x0000 to 0x01F3
    uint16_t conn_sup_timeout; // Range: 0x000A to 0x0C80, Time = N * 10 msec, Time Range: 100 msec to 32 seconds
} miwear_gap_conn_param_t;

typedef enum
{
    MIWEAR_GAP_PERIPHERAL,
    MIWEAR_GAP_CENTRAL,
} miwear_gap_role_t;

typedef struct
{
    miwear_addr_t peer_addr;
    miwear_addr_type_t type;
    miwear_gap_role_t role;
    miwear_gap_conn_param_t conn_param;
} miwear_gap_connect_t;

typedef struct
{
    miwear_addr_t peer_addr;
    miwear_gap_disconnect_reason_t reason;
} miwear_gap_disconnect_t;

typedef struct
{
    miwear_gap_conn_param_t conn_param;
} miwear_gap_connect_update_t;

typedef struct
{
    uint16_t conn_handle;
    union
    {
        miwear_gap_connect_t connect;
        miwear_gap_disconnect_t disconnect;
        miwear_gap_adv_report_t report;
        miwear_gap_connect_update_t update_conn;
    };
} miwear_gap_evt_param_t;

typedef enum
{
    MIWEAR_GAP_EVT_CONNECTED = MIWEAR_GAP_EVT_BASE, /**< Generated when a connection is established.*/
    MIWEAR_GAP_EVT_DISCONNECT, /**< Generated when a connection is terminated.*/
    MIWEAR_GAP_EVT_CONN_PARAM_UPDATED,
    MIWEAR_GAP_EVT_ADV_REPORT,
} miwear_gap_evt_t;

/*GATTS related*/

// GATTS database
typedef struct
{
    uint32_t type; // MIWEAR_UUID_16 = 0    MIWEAR_UUID_128 = 1
    union
    {
        uint16_t uuid16;
        uint8_t uuid128[16];
    };
} miwear_uuid_t;

typedef enum
{
    MIWEAR_GATTS_EVT_WRITE = MIWEAR_GATTS_EVT_BASE, // When this event is called, the characteristic has been modified.
    MIWEAR_GATTS_EVT_READ_PERMIT_REQ, // If charicteristic's rd_auth = TRUE, this event will be generated.
    MIWEAR_GATTS_EVT_WRITE_PERMIT_REQ, // If charicteristic's wr_auth = TRUE, this event will be generated, meanwhile the char value hasn't been modified. miwear_gatts_rw_auth_reply().
    MIWEAR_GATTS_EVT_IND_CONFIRM
} miwear_gatts_evt_t;

/*
 * MIWEAR_GATTS_EVT_WRITE and MIWEAR_GATTS_EVT_WRITE_PERMIT_REQ events callback
 * parameters
 * NOTE: Stack SHOULD decide whether to response to gatt client. And if need to reply, just reply success or failure according to [permit]
 * */
typedef struct
{
    uint16_t value_handle; // char value_handle
    uint16_t offset;
    uint8_t *data;
    uint16_t len;
} miwear_gatts_write_t;

/*
 * MIWEAR_GATTS_EVT_READ_PERMIT_REQ event callback parameters
 * NOTE: Stack SHOULD decide to reply the char value or refuse according to [permit]
 * */
typedef struct
{
    uint16_t value_handle; // char value handle
} miwear_gatts_read_t;

/*
 * GATTS event callback parameters union
 * */
typedef struct
{
    uint16_t conn_handle;
    miwear_addr_t peer_public_addr;
    union
    {
        miwear_gatts_write_t write;
        miwear_gatts_read_t read;
    };
} miwear_gatts_evt_param_t;

/* TIMER related */
typedef void (*miwear_timer_handler)(void *);

typedef enum
{
    MIWEAR_TIMER_SINGLE_SHOT,
    MIWEAR_TIMER_REPEATED,
} miwear_timer_mode;

typedef enum
{
    MIWEAR_SUCCESS = 0x00,
    MIWEAR_ERR_INTERNAL,
    MIWEAR_ERR_NOT_FOUND,
    MIWEAR_ERR_NO_EVENT,
    MIWEAR_ERR_NO_MEM,
    MIWEAR_ERR_INVALID_ADDR, // Invalid pointer supplied
    MIWEAR_ERR_INVALID_PARAM, // Invalid parameter(s) supplied.
    MIWEAR_ERR_INVALID_STATE, // Invalid state to perform operation.
    MIWEAR_ERR_INVALID_LENGTH,
    MIWEAR_ERR_DATA_SIZE,
    MIWEAR_ERR_TIMEOUT,
    MIWEAR_ERR_BUSY,
    MIWEAR_ERR_RESOURCES,
    MIWEAR_ERR_INVALID_CONN_HANDLE,
    MIWEAR_ERR_ATT_INVALID_ATT_HANDLE,
    MIWEAR_ERR_GAP_INVALID_BLE_ADDR,
    MIWEAR_ERR_GATT_INVALID_ATT_TYPE,
    MIWEAR_ERR_GAP_DISCONNECT,
    MIWEAR_ERR_UNKNOWN, // other ble stack errors
} miwear_status_t;

typedef void (*miwear_handler_t)(void *arg);

typedef enum
{
    MIWEAR_ARCH_EVT_GATTS_SRV_INIT_CMP,
    MIWEAR_ARCH_EVT_RECORD_WRITE,
    MIWEAR_ARCH_EVT_RECORD_DELETE,
} miwear_arch_event_t;

typedef struct
{
    miwear_ble_char_index_t char_index;
    uint16_t char_value_handle;
} miwear_gatts_char_handle_t;

typedef struct
{
    miwear_ble_service_index_t srv_index;
    uint16_t srv_handle;
    uint8_t char_num;
    miwear_gatts_char_handle_t *p_char_handle;
} miwear_gatts_srv_handle_t;

typedef struct
{
    miwear_status_t status;
    uint8_t srv_num;
    miwear_gatts_srv_handle_t *p_gatts_srv_handle;
} miwear_arch_gatts_srv_init_cmp_t;

typedef struct
{
    uint16_t id;
    miwear_status_t status;
} miwear_arch_record_t;

typedef struct
{
    union
    {
        miwear_arch_gatts_srv_init_cmp_t srv_init_cmp;
        miwear_arch_record_t record;
    };
} miwear_arch_evt_param_t;

typedef enum
{
    MIWEAR_NOT_REGISTERED = 0,
    MIWEAR_REGISTERED,
} miwear_registration_status_t;

typedef enum
{
    MIWEAR_UNAUTHORIZATION = 0,
    MIWEAR_ADMIN_AUTHORIZATION,
} miwear_authorization_status_t;

typedef enum
{
    MIWEAR_SCHD_EVT_INVALID = 0,
    MIWEAR_SCHD_EVT_REG_START,
    MIWEAR_SCHD_EVT_REG_RESTART,
    MIWEAR_SCHD_EVT_REG_SUCCESS,
    MIWEAR_SCHD_EVT_REG_FAILED,
    MIWEAR_SCHD_EVT_ADMIN_LOGIN_START,
    MIWEAR_SCHD_EVT_ADMIN_LOGIN_RESTART,
    MIWEAR_SCHD_EVT_ADMIN_LOGIN_SUCCESS,
    MIWEAR_SCHD_EVT_ADMIN_LOGIN_FAILED,
    MIWEAR_SCHD_EVT_USER_CONFIRM,
    MIWEAR_SCHD_EVT_ADMIN_LOGIN_DEVICE,
    MIWEAR_SCHD_EVT_DID_REQ,
    MIWEAR_SCHD_EVT_PSK_HMAC_REQ,
    MIWEAR_SCHD_EVT_BLE_BOND_APPLY,
    MIWEAR_SCHD_EVT_MAX_NUM
} miwear_schd_evt_id_t;

typedef enum
{
    MIWEAR_COMPANION_DEVICE_UNKNOWN = -1,
    MIWEAR_COMPANION_DEVICE_ANDROID = 0,
    MIWEAR_COMPANION_DEVICE_IOS = 1
} miwear_companion_device_type_t;

typedef struct
{
    miwear_companion_device_type_t device_type;
    float system_version;
    char *device_name;
    char *device_id;
    uint32_t app_capability;
} miwear_companion_device_info_t;

typedef enum
{
    FAIL_REASON_CONFIRM_TIMEOUT,
    FAIL_REASON_USER_CANCEL,
    FAIL_REASON_APP_CANCEL,
    FAIL_REASON_FSM_TIMEOUT,
    FAIL_REASON_DISCONNECT,
    FAIL_REASON_USER_ID_MISMATCH,
    FAIL_REASON_UNKNOWN
} miwear_schd_fail_reason_t;

typedef struct
{
    miwear_schd_evt_id_t id;
    union
    {
        uint8_t oob[6];
        miwear_companion_device_info_t device;
        miwear_schd_fail_reason_t fail_reason;
        uint8_t hmac_in[80];
    } data;
} miwear_schd_evt_t;

typedef enum
{
    MIWEAR_UNLOCK_DEVICE_TYPE_WEARABLE = 0x01
} miwear_unlock_device_type_t;

typedef enum
{
    MIWEAR_UNLOCK_DEVICE_SUBTYPE_WATCH = 0x01,
    MIWEAR_UNLOCK_DEVICE_SUBTYPE_RECTANGLE_RTOS = 0x02,
    MIWEAR_UNLOCK_DEVICE_SUBTYPE_CIRCLE_RTOS = 0x03,
    MIWEAR_UNLOCK_DEVICE_SUBTYPE_MCU_BAND = 0x04
} miwear_unlock_device_subtype_t;

typedef enum
{
    MIWEAR_UNLOCK_TARGET_DEVICE_TYPE_PHONE = 0x02,
    MIWEAR_UNLOCK_TARGET_DEVICE_TYPE_PC = 0x03
} miwear_unlock_target_device_type_t;

typedef uint8_t miwear_unlock_target_device_subtype_t;

typedef enum
{
    MIWEAR_UNLOCK_EVT_USER_CONFIRM,
    MIWEAR_UNLOCK_EVT_CONFIRM_TIMEOUT,
    MIWEAR_UNLOCK_EVT_CONNECTED,
    MIWEAR_UNLOCK_EVT_DISCONNECTED,
    MIWEAR_UNLOCK_EVT_ENABLE,
    MIWEAR_UNLOCK_EVT_DISABLE
} miwear_unlock_evt_id_t;

typedef struct
{
    miwear_unlock_evt_id_t id;
    miwear_unlock_target_device_type_t type;
} miwear_unlock_evt_t;

typedef enum
{
    MIWEAR_WRITE_EVT_PROTO,
    MIWEAR_WRITE_EVT_MASS,
    MIWEAR_WRITE_EVT_OTA,
} miwear_write_evt_t;

typedef struct
{
    uint8_t *data;
    uint16_t len;
} miwear_write_data_t;

typedef struct
{
    uint16_t conn_handle;
    miwear_write_data_t write_data;
    WearPacket *proto_packet;
} miwear_write_evt_param_t;

typedef enum
{
    MIWEAR_NOTIFY_TYPE_NOT_SUPPORTED,
    MIWEAR_NOTIFY_TYPE_PROTO,
    MIWEAR_NOTIFY_TYPE_FITNESS,
    MIWEAR_NOTIFY_TYPE_VOICE,
    MIWEAR_NOTIFY_TYPE_OTA,
    MIWEAR_NOTIFY_TYPE_STAT,
    MIWEAR_NOTIFY_TYPE_LOG,
    MIWEAR_NOTIFY_TYPE_SENSOR_DATA
} miwear_notify_type_t;

typedef enum
{
    TX_DATA_TYPE_PROTO,
    TX_DATA_TYPE_FILE,
    TX_DATA_TYPE_VOICE,
    TX_DATA_TYPE_MAX_NUM
} miwear_tx_data_type_t;

typedef enum
{
    FILE_TYPE_SENSOR,
    FILE_TYPE_FITNESS,
    FILE_TYPE_LOG,
    FILE_TYPE_STATISTICS,
    FILE_TYPE_MAX_NUM
} miwear_file_type_t;

typedef void (*file_open_cb_t)(int index, int handle);
typedef void (*file_read_cb_t)(int handle, int result);
typedef void (*file_close_cb_t)(int result);

typedef void (*data_send_cb_t)(uint32_t id, int result);
typedef void (*file_send_cb_t)(miwear_file_type_t type, uint8_t *id, size_t id_len, int result);

typedef void (*miwear_ble_notify_callback_t)(uint16_t conn_handle,
                                             miwear_notify_type_t type,
                                             miwear_status_t status, void *cb_arg);
typedef void (*miwear_spp_notify_callback_t)(uint16_t conn_handle,
                                             miwear_notify_type_t type,
                                             miwear_status_t status, void *cb_arg);

typedef void (*miwear_spp_notify_complete_cb_t)(miwear_status_t status,
                                                void *user_data);

typedef void (*miwear_write_handler_t)(miwear_write_evt_t evt, miwear_write_evt_param_t *param);
typedef void (*miwear_schd_event_handler_t)(uint16_t conn_handle, miwear_schd_evt_t *p_event);
typedef void (*miwear_unlock_callback_t)(miwear_unlock_evt_t *p_evt);

#define MIWEAR_MASS_ID_LEN 16

typedef enum
{
    MASS_PREPARE_TYPE_TRANSPORT_RESUME = 0,
    MASS_PREPARE_TYPE_TRANSPORT_RESTART,
} miwear_mass_prepare_type_t;

typedef enum
{
    MASS_PREPARE_ERRCODE_DEFAULT = 0x00,
    MASS_PREPARE_ERRCODE_BUSY = -1,
    MASS_PREPARE_ERRCODE_DUPLICATED = -2,
    MASS_PREPARE_ERRCODE_LOW_STORAGE = -3,
    MASS_PREPARE_ERRCODE_LOW_BATTERY = -4,
    MASS_PREPARE_ERRCODE_OTHERS = -255,
} miwear_mass_prepare_errcode_t;

typedef enum
{
    MASS_FINISH_RESULT_SUCCESS = 0x00,
    MASS_FINISH_RESULT_WRITE_FAIL,
    MASS_FINISH_RESULT_WRITE_ERR_CRC,
    MASS_FINISH_RESULT_WRITE_ERR_MD5,
    MASS_FINISH_RESULT_WRITE_ERR_LENGTH_INVALID,
    MASS_FINISH_RESULT_WRITE_ERR_TIMEOUT,
    MASS_FINISH_RESULT_WRITE_ERR_OTHERS,
    MASS_FINISH_RESULT_PAUSE,
    MASS_FINISH_RESULT_CANCEL
} miwear_mass_finish_result_t;

typedef void (*prepare_cb_t)(int index, int result, uint32_t received,
                             void *md5_context, uint8_t select_compress_mode,
                             uint32_t slice_length);

typedef void (*mass_write_cb_t)(int handle, int result);

typedef struct
{
    uint16_t conn_handle;
    uint16_t remote_command_char_handle;               //!< AMS Remote Command Characteristic. Allows interaction with the peer (0x81D8).
    uint16_t entity_update_char_handle;                //!< AMS Entity Update Characteristic. Contains the different information about the desired attributes (0xABCE).
    uint16_t entity_attribute_char_handle;             //!< AMS Entity Attribute Characteristic, where the extended value of an attribute can be received (0xF38C).
} miwear_media_ams_service_t;

typedef enum
{
    MIWEAR_MEDIA_AMS_EVT_DISCOVERY_COMPLETE,           //!< Event indicating that the Apple Media Service has been discovered at the peer. */
    MIWEAR_MEDIA_AMS_EVT_DISCOVERY_FAILED,             //!< Event indicating that the Apple Media Service has not been discovered at the peer. */
    MIWEAR_MEDIA_AMS_EVT_REMOTE_COMMAND_NOTIFICATION,  //!< Event indicating that a notification of the Remote Command characteristic has been received from the peer. */
    MIWEAR_MEDIA_AMS_EVT_ENTITY_UPDATE_NOTIFICATION,   //!< Event indicating that a notification of the Entity Update characteristic has been received from the peer. */
    MIWEAR_MEDIA_AMS_EVT_ENTITY_ATTRIBUTE_READ_RESP,   //!< Event indicating that a read response on Entity Attribute characteristic has been received from peer. */
} miwear_media_ams_evt_type_t;

typedef struct
{
    miwear_media_ams_evt_type_t type;                  //!< Type of event.
    uint16_t
    conn_handle;           //!< Connection handle on which the AMS service was discovered on the peer device. This field will be filled if the @p evt_type is @ref BLE_AMS_C_EVT_DISCOVERY_COMPLETE.
    uint16_t                    data_len;              //!< receive data length
    uint8_t                     data[];                //!< receive data
} miwear_media_ams_evt_t;

#ifdef __cplusplus
}
#endif

#endif // __MIWEAR_TYPE_H__
