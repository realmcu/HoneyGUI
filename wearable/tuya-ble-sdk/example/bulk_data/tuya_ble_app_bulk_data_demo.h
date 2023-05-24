#ifndef TUYA_BLE_APP_BULK_DATA_DEMO_H_
#define TUYA_BLE_APP_BULK_DATA_DEMO_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_ble_type.h"



/**@brief   Function for handling the bulk data events.
 *
 * @details The bulk data reading steps are:
 *
 * 1. The mobile app first reads the bulk data information of the specified type, and triggers the Tuya BLE SDK
 *    to send the 'TUYA_BLE_BULK_DATA_EVT_READ_INFO' event to the device application. The event data contains the
 *    type value of the bulk data to be read by the mobile app. The device application calls the 'tuya_ble_bulk_data_response()'
 *    function to send the total length, total CRC32, block size and other information of the bulk data to the mobile app.
 *
 * 2. The mobile app starts the bulk data reading process according to the data information returned by the device application.
 *    First trigger the Tuya BLE SDK to send the 'TUYA_BLE_BULK_DATA_EVT_READ_BLOCK' event to request the data information of
 *    the first block, including block number (starting from 0), block size, and CRC16 of the block data. The device application
 *    calls the'tuya_ble_bulk_data_response()' function to reply to the block data information.
 * 3. After sending the block data information to the mobile app, the Tuya BLE SDK will automatically send the'TUYA_BLE_BULK_DATA_EVT_SEND_DATA'
 *    event to the device application. The event data contains the block number to be read. The device application calls the'tuya_ble_bulk_data_response()'
 *    function to send the block data .
 * 4. After sending the block data information to the mobile app, the Tuya BLE SDK will automatically send the'TUYA_BLE_BULK_DATA_EVT_SEND_DATA'
 *    event to the device application. The event data contains the block number to be read. The device application calls the'tuya_ble_bulk_data_response()'
 *    function to send the block data.
 * 5. After reading all the blocks, the mobile app will verify the data. When the verification is completed, all the dp point data in it will be parsed and
 *    uploaded to the cloud. When the upload is complete, the Tuya BLE SDK will be triggered to send the'TUYA_BLE_BULK_DATA_EVT_ERASE' event to the device
 *    application. The event data contains The type value of bulk data to be erased, after the device application erases the data,
 *    call the'tuya_ble_bulk_data_response()' function to send the erase result.
 * 6. If the device defines multiple types of bulk data, the mobile app will initiate the process of reading bulk data of other types.
 *
 * @note The application must call this function where it receives the @ref TUYA_BLE_CB_EVT_BULK_DATA event.
 *
 * @param[in] p_data     Event data received from the SDK.
 *
 */
void tuya_ble_app_bulk_data_handler(tuya_ble_bulk_data_request_t *evt);

/**@brief  Function for generate bulk data for testing.
 *
 * @details The device application calls this function to generate bulkdata test data.
 *
 */
void tuya_ble_test_bulk_data_generation(void);


#ifdef __cplusplus
}
#endif

#endif // 







