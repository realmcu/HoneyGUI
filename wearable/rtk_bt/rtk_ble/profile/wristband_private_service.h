#ifndef __WRISTBAND_PRIVATE_SERVICE_H__
#define __WRISTBAND_PRIVATE_SERVICE_H__

#include <stdint.h>
#include <profile_server.h>


#define BLE_UUID_BWPS_SERVICE               0xFF01
#define BLE_UUID_BWPS_TX_CHARACTERISTIC     0xFF02
#define BLE_UUID_BWPS_RX_CHARACTERISTIC     0xFF03
#define BLE_UUID_BWPS_DEVICE_NAME           0xFF04
#define BLE_UUID_BWPS_LED_CONTROL           0xFF05
#define BLE_UUID_BWPS_DEBUG_PATH            0xFF06


#define GATT_SRV_BWPS_TX_INDEX              0x02
#define GATT_SRV_BWPS_RX_INDEX              0x04
#define GATT_SRV_BWPS_CCCD_INDEX            0x05
#define GATT_SRV_BWPS_DEVNAME_INDEX         0x07

#define DEVICE_NAME_MAX_LENGTH              0x0C
#define ADVERTISING_PACKET_LENGTH           0x1f

/** @defgroup TSIMP_WRITE_MSG TSIMP_WRITE_MSG
  * @brief Simple BLE service written msg to application.
  * @{
  */
typedef struct
{
    uint8_t opcode; //!< ref: @ref SIMP_Control_Point_OpCodes, @ref SIMP_Service_Write_Info
    T_WRITE_TYPE write_type;
    uint16_t len;
    uint8_t *p_value;
} TBWPS_WRITE_MSG;
/** @} End of TSIMP_WRITE_MSG */


/** @defgroup TSIMP_UPSTREAM_MSG_DATA TSIMP_UPSTREAM_MSG_DATA
  * @brief Simple BLE service callback message content.
  * @{
  */
typedef union
{
    uint8_t notification_indification_index; //!< ref: @ref SIMP_Service_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref SIMP_Service_Read_Info
    TBWPS_WRITE_MSG write;
} TBWPS_UPSTREAM_MSG_DATA;
/** @} End of TSIMP_UPSTREAM_MSG_DATA */

/** @defgroup TSIMP_CALLBACK_DATA TSIMP_CALLBACK_DATA
  * @brief Simple BLE service data to inform application.
  * @{
  */
typedef struct
{
    uint8_t                 conn_id;
    T_SERVICE_CALLBACK_TYPE msg_type;
    TBWPS_UPSTREAM_MSG_DATA msg_data;
} TBWPS_CALLBACK_DATA;
/** @} End of TSIMP_CALLBACK_DATA */


T_SERVER_ID bwps_service_add_service(void *p_func);
void wristband_send(uint8_t conn_id, uint16_t length, uint8_t *value);



#endif // __WRISTBAND_PRIVATE_SERVICE_H__
