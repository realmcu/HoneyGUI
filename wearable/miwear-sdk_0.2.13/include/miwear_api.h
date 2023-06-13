#ifndef __MIWEAR_API_H__
#define __MIWEAR_API_H__

#include "miwear_type.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//*************************************************************************
/**
  * sdk internal implement, and public to external     --   START
  */
//* ***********************************************************************

uint16_t miwear_mass_data_list_length_get(void);

/**
 * @brief initialize the sdk work
 *
 * @param write_handler:    callback function to receive once complete bt data
*/
miwear_status_t miwear_bluetooth_init(miwear_write_handler_t write_handler);

/***
 * @brief  whether has bond/registered
 *
 * @return see miwear_registration_status_t
 *
*/
miwear_registration_status_t miwear_get_registration_status(void);

/***
 * @brief whehter  has logged in/authenticated
 *
 * @return see miwear_authorization_status_t
*/
miwear_authorization_status_t miwear_get_authorization_status(void);

/***
 * @brief initialize the authenticate scheduler
 *
 * @param [in]handler: callback function to receive the bind/login event, such as timeout, success, or oob code confirm
*/
miwear_status_t miwear_scheduler_init(miwear_schd_event_handler_t handler);

/***
 * @brief user confirm, either directly or after comparing displayed oob number between device and phone
 *
 * @param [in]conn_handle: connection handle
*/
int miwear_schd_user_confirm(uint16_t conn_handle);

/***
 * @brief user cancel the confirmation
 *
 * @param [in]conn_handle: connection handle
*/
void miwear_schd_user_cancel(uint16_t conn_handle);

/**
 * @brief pass device did result to sdk. should be called after receive MIWEAR_SCHD_EVT_DID_REQ event
 *
 * @param conn_handle connection handle
 * @param did device did
 * @param did_len device did length (strlen(did))
 */
void miwear_schd_did_result(uint16_t conn_handle, char *did, int did_len);

/**
 * @brief pass psk hmac result to sdk. should be called after receive MIWEAR_SCHD_EVT_PSK_HMAC_REQ event
 *
 * @param conn_handle connection handle
 * @param hmac hmac result
 */
void miwear_schd_psk_hmac_result(uint16_t conn_handle, uint8_t hmac[32]);

/**
 *@brief    This function is MIWEAR GAP related event callback function.
 *@param    [in] evt : GAP EVENT
 *          [in] param : callback parameters corresponding to evt
 *@return   Void
 *@note     You should support this function in your own ble stack .
 *          Make sure when the corresponding event occurs, be able to call this
 *function
 *          and pass in the corresponding parameters.
 */
void miwear_gap_event_callback(miwear_gap_evt_t evt, miwear_gap_evt_param_t *param);

/**
 *@brief    This function is MIWEAR GATTS related event callback function.
 *@param    [in] evt : GATTS EVENT
 *          [in] param : callback parameters corresponding to evt
 *@return   Void
 *@note     You should support this function in your own ble stack .
 Make sure when the corresponding event occurs, be able to call this
 function and pass in the corresponding parameters.
 */
void miwear_gatts_event_callback(miwear_gatts_evt_t evt,
                                 miwear_gatts_evt_param_t *param);

/*
 *@brief    This function is miwear_arch api related event callback function.
 *@param    [in] evt: asynchronous function complete event
 *          [in] param: the return of asynchronous function
 *@note     You should support this function in corresponding asynchronous function.
 *          For now, miwear_gatts_service_int and miwear_record_write is asynchronous.
 * */
void miwear_arch_event_callback(miwear_arch_event_t evt,
                                miwear_arch_evt_param_t *param);

/***
 * @brief  read the charateristic value
 *
 * @param [in] char_handle:     harateristic handle
 *                  [out] value:    pointer to charateristic value buf
 *                  [in/out] p_value_len:   pointer to charateristic value len
 *
 * @note  [in]the initial  value (*p_value_len) is the value buf size
 *                  [out] the return value(*p_value_len ) is the value data size
*/
miwear_status_t miwear_gatt_chrc_value_get(int char_handle, uint8_t *value, uint16_t *p_value_len);

/**
 * @brief  get the adv data - mi service data
 *
 * @param [out]srv_data:    pointer to service data buf
 *                    [in/out]p_srv_len:    pointer to service data len
 *
 * @return MI_ERR_DATA_SIZE:    srv_data real size < the  service data size
 *                    MI_SUCCESS:   data writen to srv_data successfully
 *
 * @note [in]the initial  value (*p_srv_len) is the service buf size
 *                [out] the return value(*p_srv_len ) is the service data size
*/
miwear_status_t mibeacon_mi_service_data_get(uint8_t *srv_data, uint16_t *p_srv_len);

/**
 * @brief  get the adv data - mi service data
 *
 * @param [out]srv_data:    pointer to service data buf
 *                      [in/out]p_srv_len:    pointer to service data len
 *                      [in]solicited: ios device should start bt bond if solicited is true
 *                      [in]bond_new: start bond new phone procedure if bond_new is true
 *
 * @return MI_ERR_DATA_SIZE:    srv_data real size < the  service data size
 *                    MI_SUCCESS:   data writen to srv_data successfully
 *
 * @note [in]the initial  value (*p_srv_len) is the service buf size
 *                [out] the return value(*p_srv_len ) is the service data size
*/
miwear_status_t mibeacon_mi_service_data_get_with_config(uint8_t *srv_data,
                                                         uint16_t *p_srv_len,
                                                         bool solicited,
                                                         bool bond_new);

/**
 * @brief get the adv data - unlock service data
 *
 * @param srv_data          pointer to service data buf
 * @param p_srv_len         pointer to service data len
 * @param type              unlock device type, see unlock_device_type_t
 * @param sub_type          unlock device sub type, see unlock_device_subtype_t
 * @return miwear_status_t  MI_ERR_DATA_SIZE: srv_data real size < the  service data size
 *                          MI_SUCCESS: data writen to srv_data successfully
 *                          MI_ERR_INVALID_ADDR: no srv address found
 */
miwear_status_t mibeacon_unlock_service_data_get(uint8_t *srv_data, uint16_t *p_srv_len,
                                                 miwear_unlock_device_type_t type, miwear_unlock_device_subtype_t sub_type);

/**
 * @brief confirm add new unlock device
 *
 * @return int
 */
int miwear_unlock_add_confirm(void);

/**
 * @brief cancel add new unlock device
 *
 * @return int
 */
int miwear_unlock_add_cancel(void);

/**
 * @brief initialize unlock related data structure and callback
 *
 * @param h callback function to process unlock events, see miwear_unlock_callback_t
 */
void miwear_unlock_init(miwear_unlock_callback_t h);

/**
 * @brief send encoded protobuf data to miwear app, just push data to queue, and sdk will transmit each data one by one automatically
 *
 * @param data data to send
 * @param data_len length of data to send
 * @param id used to distinguish different data set
 * @param send_cb when data send complete, whether success or not, this callback will be called, 0 means success, see data_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_proto_data_send(uint8_t *data, uint16_t data_len, uint32_t id,
                           data_send_cb_t send_cb);

/**
 * @brief send file or flash data to miwear app, just push file to queue, and sdk will transmit each file one by one automatically
 *
 * @param type see miwear_file_type_t
 * @param id file id, which is unique for each file
 * @param id_len file id length
 * @param header header data of file
 * @param header_len length of header data
 * @param size file size
 * @param send_cb when file send complete, whether success or not, this callback will be called, 0 means success, see file_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_file_enqueue(miwear_file_type_t type, uint8_t *id, size_t id_len,
                        uint8_t *header, size_t header_len, size_t size,
                        file_send_cb_t send_cb);

/**
 * @brief send voice start cmd to miwear app
 *
 * @param session_id session id used by miwear app
 * @param tts indicate whether device support tts or not
 * @param id used to distinguish different data set
 * @param send_cb when send complete, whether success or not, this callback will be called, 0 means success, see data_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_voice_start(uint8_t session_id, bool tts, uint32_t id,
                       data_send_cb_t send_cb);

/**
 * @brief send voice data to miwear app
 *
 * @param data voice data to send
 * @param data_len length of voice data
 * @param id used to distinguish different data set
 * @param send_cb when send complete, whether success or not, this callback will be called, 0 means success, see data_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_voice_data_send(uint8_t *data, uint16_t data_len, uint32_t id,
                           data_send_cb_t send_cb);

/**
 * @brief send voice data end cmd to miwear app
 *
 * @param id used to distinguish different data set
 * @param send_cb when send complete, whether success or not, this callback will be called, 0 means success, see data_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_voice_data_end(uint32_t id, data_send_cb_t send_cb);

/**
 * @brief send voice exit cmd to miwear app
 *
 * @param id used to distinguish different data set
 * @param send_cb when send complete, whether success or not, this callback will be called, 0 means success, see data_send_cb_t
 * @return int 0 - enqueue success
 *              < 0 - enqueue failed
 */
int miwear_voice_exit(uint32_t id, data_send_cb_t send_cb);

/**
 * @brief receive ancs notification source notification from remote iOS device
 *
 * @param data notification data
 * @param length length of notification data
 */
void miwear_ancs_notification_source_receive(uint8_t *data, uint16_t length);

/**
 * @brief receive ancs data source notification from remote iOS device
 *
 * @param data notification data
 * @param length length of notification data
 */
void miwear_ancs_data_source_receive(uint8_t *data, uint16_t length);

/**
 * @brief pick up incoming phone call, only used for remote iOS device
 *
 */
void miwear_phone_call_pick_up(void);

/**
 * @brief hang up incoming phone call, used for remote Android & iOS device
 *
 */
void miwear_phone_call_hang_up(void);

/**
 * @brief mute incoming phone call, used for remote Android device
 *
 */
void miwear_phone_call_mute(void);

/**
 * @brief remove notification by uid, only used for remote iOS device
 *
 * @param uid uid of notification
 */
void miwear_remove_notification(uint32_t uid);

/**
 * @brief Function for sending a player control for media
 *
 * @param command:    refer to PlayerControl_Command in wear_media.pb.h
 * @param volume:     only ADJUST_VOLUME is valid, 0 is fine if command is not ADJUST_VOLUME
 * @return void
 */
void miwear_media_player_control(uint8_t command, uint8_t volume);

/**
 * @brief Function for sending a player request for media
 *
 * @return void
 */
void miwear_media_player_request(void);

/**
 * @brief Function for handling the application's BLE stack events for Apple Media Service
 *
 * @param p_evt:     Pointer to the miwear_media_ams_evt_t structure
 * @return void
 */
void miwear_media_ams_evt_handler(miwear_media_ams_evt_t *p_evt);

/**
 * @brief update spp service connected status
 *
 * @param connect_status 0: disconnect, 1: connected
 */
void miwear_spp_service_update_connect_status(uint16_t conn_handle, uint8_t connect_status);

/**
 * @brief the interface of receive data via spp(rfcomm)
 *
 * @param data              data buffer
 * @param data_len          data buffet length
 * @return miwear_status_t
 */
miwear_status_t miwear_spp_service_receive_data(uint16_t conn_handle,
                                                uint8_t *data,
                                                uint16_t data_len);

/**
 * @brief ble bond reply - response for ble bond apply
 *
 * @param conn_handle
 * @param status see BondReply_Status, the ble bond status
 */
void miwear_ble_bond_reply(uint16_t conn_handle, BondReply_Status status);

//*************************************************************************
/**
  * sdk internal implement, and public to external     --   END
  */
//* ***********************************************************************


// ***********************************************************************
/***
 * @note Methods to implement by each port   --  START
*/
// ***********************************************************************

/**
 * @brief get the BLE ATT MTU
 *
 * @param   [in] conn_handle: connect handle
 *                      [out] mtu: pointer to mtu
 *                      [out] hash_data: pointer to message digest data
 *
 * @return MI_SUCCESS               The requested att mtu were written to mtu
 *                    MI_ERR_INVALID_ADDR   Invalid pointer supplied.
 *                    MIWEAR_ERR_INVALID_CONN_HANDLE
 *
 * */
miwear_status_t miwear_att_mtu_get(uint16_t conn_handle, uint16_t *mtu);

/**
 * @brief   Get BLE mac address.
 * @param   [out] mac: pointer to data
 * @return  MI_SUCCESS          The requested mac address were written to mac
 *          MI_ERR_INTERNAL     No mac address found.
 * @note:   You should copy gap mac to mac[6]
 * */
miwear_status_t miwear_gap_address_get(miwear_addr_t mac);

/**
 * @brief   Update the connection parameters.
 * @param   [in] conn_handle: the connection handle.
 *          [in] conn_params: the connection parameters.
 * @return  MI_SUCCESS             The Connection Update procedure has been
 *started successfully.
 *          MI_ERR_INVALID_STATE   Initiated this procedure in disconnected
 *state.
 *          MI_ERR_INVALID_PARAM   Invalid parameter(s) supplied.
 *          MI_ERR_BUSY            The stack is busy, process pending events and
 *retry.
 *          MIWEAR_ERR_INVALID_CONN_HANDLE
 * @note    This function can be used by both central role and peripheral
 *role.
 * */
miwear_status_t miwear_gap_update_conn_params(uint16_t conn_handle,
                                              miwear_gap_conn_param_t conn_params);

/**
 * @brief   Set characteristic value and notify it to client.
 * @param   [in] conn_handle: conn handle
 *          [in] srv_handle: service handle
 *          [in] char_value_handle: characteristic  value handle
 *          [in] offset: the offset from which the attribute value has to
 * be updated
 *          [in] p_value: pointer to data
 *          [in] len: data length
 *
 * @return  MI_SUCCESS             Successfully queued a notification or
 * indication for transmission,
 *          MI_ERR_INVALID_ADDR    Invalid pointer supplied.
 *          MI_ERR_INVALID_PARAM   Invalid parameter (offset) supplied.
 *          MI_ERR_INVALID_STATE   Invalid Connection State or notifications
 * and/or indications not enabled in the CCCD.
 *          MI_ERR_INVALID_LENGTH   Invalid length supplied.
 *          MI_ERR_BUSY            Procedure already in progress.
 *          MIWEAR_ERR_ATT_INVALID_HANDLE     Attribute not found.
 *          MIWEAR_ERR_GATT_INVALID_ATT_TYPE   //Attributes are not modifiable by
 * the application.
 * @note    This function checks for the relevant Client Characteristic
 * Configuration descriptor value to verify that the relevant operation
 * has been enabled by the client.
 * */
miwear_status_t miwear_gatts_notify(uint16_t conn_handle,
                                    uint16_t srv_handle, uint16_t char_value_handle, uint16_t offset,
                                    uint8_t *p_value, uint16_t len);

/**
 *        SOFT TIMER APIs
 */

/**
 * @brief   Create a timer.
 * @param   [out] p_timer_id: a pointer to timer id address which can uniquely identify the timer.
 *          [in] timeout_handler: a pointer to a function which can be
 * called when the timer expires.
 *          [in] mode: repeated or single shot.
 * @return  MI_SUCCESS             If the timer was successfully created.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied.
 *          MI_ERR_INVALID_STATE   timer module has not been initialized or the
 * timer is running.
 *          MI_ERR_NO_MEM          timer pool is full.
 *
 * */
miwear_status_t miwear_timer_create(void **p_timer_id,
                                    miwear_timer_handler timeout_handler, miwear_timer_mode mode);

/**
 * @brief   Delete a timer.
 * @param   [in] timer_id: timer id
 * @return  MI_SUCCESS             If the timer was successfully deleted.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied..
 * */
miwear_status_t miwear_timer_delete(void *timer_id);

/**
 * @brief   Start a timer.
 * @param   [in] timer_id: timer id
 *          [in] timeout_value: Number of milliseconds to time-out event
 * (minimum 10 ms).
 *          [in] p_context: parameters that can be passed to
 * timeout_handler
 *
 * @return  MI_SUCCESS             If the timer was successfully started.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied.
 *          MI_ERR_INVALID_STATE   If the application timer module has not been
 * initialized or the timer has not been created.
 *          MI_ERR_NO_MEM          If the timer operations queue was full.
 * @note    If the timer has already started, it will start counting again.
 * */
miwear_status_t miwear_timer_start(void *timer_id, uint32_t timeout_value,
                                   void *p_context);

/**
 * @brief   Stop a timer.
 * @param   [in] timer_id: timer id
 * @return  MI_SUCCESS             If the timer was successfully stopped.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied.
 *
 * */
miwear_status_t miwear_timer_stop(void *timer_id);

/**
 *        NVM APIs
 */
/**
 * @brief delete a record in flash
 *
 * @param key record key to delete
 * @return miwear_status_t
 */
miwear_status_t miwear_record_delete(const char *key);

/**
 * @brief read record from flash
 *
 * @param key record key to read
 * @param value pointer to value buffer
 * @param[inout] p_len pointer to len, which is len of value buffer for input and len of actual value data for output
 * @return miwear_status_t
 */
miwear_status_t miwear_record_read(const char *key, uint8_t *value, uint16_t *p_len);

/**
 * @brief write record to flash
 *
 * @param key record key to write
 * @param value pointer to value data
 * @param len len of value data
 * @return miwear_status_t
 */
miwear_status_t miwear_record_write(const char *key, const uint8_t *value, uint16_t len);

/**
 *        MISC APIs
 */

/**
 * @brief get the device model value
 *
 * @param   [out] model_buf: pointer to model
 *                      [out] p_model_len: pointer to model data len
 * @return MI_SUCCESS               The requested model were written to model_buf
 *                    MI_ERR_INVALID_ADDR   no model address found
 *                    MI_ERR_DATA_SIZE: model_buf real size < the  model data size
 *
 * @note [in]the initial  value (*p_model_len) is the model_buf size
 *                [out] the return value(*p_model_len ) is the model data size
 *
 * */
miwear_status_t miwear_device_model_get(char *model_buf, uint8_t *p_model_len);

/**
 * @brief get the device serial number
 *
 * @param   [out] sn_buf:   pointer to sn
 *                      [out] p_sn_len:   pointer to sn data len
 * @return MI_SUCCESS               The requested sn were written to sn_buf
 *                    MI_ERR_INVALID_ADDR   no sn address found
 *                    MI_ERR_DATA_SIZE: sn_buf real size < the  sn data size
 *
 * @note [in]the initial  value (*p_sn_len) is the sn_buf size
 *                [out] the return value(*p_sn_len ) is the sn data size
 *
 * */
miwear_status_t miwear_device_serial_number_get(char *sn_buf, uint8_t *p_sn_len);

/**
 * @brief get the device OOB
 *
 * @param oob_buf     pointer to oob buffer
 * @param p_oob_len   pointer to oob data len
 * @return miwear_status_t
 * @note [in]the initial value (*p_oob_len) is the oob buffer size
 *                [out] the return value(*p_oob_len ) is the oob data size
 */
miwear_status_t miwear_device_oob_get(char *oob_buf, uint8_t *p_oob_len);

/**
 * @brief   Get ture random bytes .
 * @param   [out] p_buf: pointer to data
 *          [in] len: Number of bytes to take from pool and place in
 * p_buff
 * @return  MI_SUCCESS          The requested bytes were written to
 * p_buff
 *          MI_ERR_NO_MEM       No bytes were written to the buffer, because
 * there were not enough random bytes available.
 * @note    SHOULD use TRUE random num generator
 * */
miwear_status_t miwear_rand_num_generator(uint8_t *p_buf, uint16_t len);

/**
 * @brief Initialize the miwear crypto module.
 *
 * @return miwear_status_t MI_SUCCESS crypto module initialization success
 */
miwear_status_t miwear_crypto_init(void);

/**
 * @brief   Encrypts a block according to the specified parameters. 128-bit
 * AES encryption. (zero padding)
 * @param   [in] key: encryption key
 *          [in] plaintext: pointer to plain text
 *          [in] plen: plain text length
 *          [out] ciphertext: pointer to cipher text
 * @return  MI_SUCCESS              The encryption operation completed.
 *          MI_ERR_INVALID_ADDR     Invalid pointer supplied.
 *          MI_ERR_INVALID_STATE    Encryption module is not initialized.
 *          MI_ERR_INVALID_LENGTH   Length bigger than 16.
 *          MI_ERR_BUSY             Encryption module already in progress.
 * @note    SHOULD use synchronous mode to implement this function
 * */
miwear_status_t miwear_aes128_encrypt(const uint8_t *key, const uint8_t *plaintext, uint16_t plen,
                                      uint8_t *ciphertext);

/**
 * @brief Create a public/private key pair.
 *
 * @param curve curve type, see ecc_curve_t
 * @param sk    private key, pointer must be aligned to a 4-byte boundary
 * @param pk    public key, pointer must be aligned to a 4-byte boundary
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_ecdh_keypair_gen(miwear_ecc_curve_t curve, miwear_ecc256_sk_t sk,
                                               miwear_ecc256_pk_t pk);

/**
 * @brief Create a shared secret from a provided public/private key pair.
 *
 * @param curve curve type, see ecc_curve_t
 * @param sk    our private key, pointer must be aligned to a 4-byte boundary
 * @param pk    public key from another party, pointer must be aligned to a 4-byte boundary
 * @param ss    shared secret, pointer must be aligned to a 4-byte boundary
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_ecdh_shared_secret_compute(miwear_ecc_curve_t curve,
                                                         const miwear_ecc256_sk_t sk, const miwear_ecc256_pk_t pk, miwear_ecc256_ss_t ss);

/**
 * @brief This is the HMAC-based Extract-and-Expand Key Derivation Function (HKDF), using the sha-256 message digest
 *
 * @param key       The input keying material
 * @param key_len   The length of key
 * @param salt      An optional salt value (a non-secret random value)
 * @param salt_len  The length in bytes of the optional salt
 * @param info      An optional context and application specific information string
 * @param info_len  The length of info in bytes
 * @param out       The output keying material of out_len bytes
 * @param out_len   The length of the output keying material in bytes
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_hkdf_sha256(const uint8_t *key, uint16_t key_len, const uint8_t *salt,
                                          uint16_t salt_len,
                                          const uint8_t *info, uint16_t info_len, uint8_t *out, uint16_t out_len);

/**
 * @brief This function calculates the full generic HMAC on the input buffer with the provided key,
 *        using the sha-256 message digest
 *
 * @param k         The HMAC secret key
 * @param key_len   The length of the HMAC secret key in bytes
 * @param p_in      The buffer holding the input data
 * @param in_len    The length of the input data
 * @param p_out     The generic HMAC result
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_hmac_sha256(const uint8_t *k, uint16_t key_len, const uint8_t *p_in,
                                          uint16_t in_len, uint8_t *p_out);

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */

/**
 * @brief Output = SHA-256( input buffer )
 *
 * @param input   buffer holding the data
 * @param ilen    length of the input data
 * @param output  SHA-256 checksum result
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_sha256(const uint8_t *input, uint16_t ilen, uint8_t *output);

/**
 * @brief This function encrypts a buffer using aes ccm.
 *
 * @param key       key used for aes ccm encryption
 * @param iv        The initialization vector (nonce)
 * @param iv_len    The length of the nonce in bytes
 * @param add       The additional data field
 * @param add_len   The length of additional data in bytes
 * @param in        The buffer holding the input data
 * @param in_len    The length of the input data in bytes
 * @param out       The buffer holding the output data
 * @param tag       The buffer holding the authentication field
 * @param tag_len   The length of the authentication field
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_ccm_encrypt(const uint8_t *key,
                                          const uint8_t *iv, size_t iv_len,
                                          const uint8_t *add, size_t add_len,
                                          const uint8_t *in, size_t in_len,
                                          uint8_t *out,
                                          uint8_t *tag, size_t tag_len);

/**
 * @brief This function performs aes ccm authenticated decryption of a buffer.
 *
 * @param key       key used for aes ccm decryption
 * @param iv        The initialization vector (nonce)
 * @param iv_len    The length of the nonce in bytes
 * @param add       The additional data field
 * @param add_len   The length of additional data in bytes
 * @param in        The buffer holding the input data
 * @param in_len    The length of the input data in bytes
 * @param out       The buffer holding the output data
 * @param tag       The buffer holding the authentication field
 * @param tag_len   The length of the authentication field
 * @return miwear_status_t
 */
miwear_status_t miwear_crypto_ccm_decrypt(const uint8_t *key,
                                          const uint8_t *iv, size_t iv_len,
                                          const uint8_t *add, size_t add_len,
                                          const uint8_t *in, size_t in_len,
                                          uint8_t *out,
                                          const uint8_t *tag, size_t tag_len);

/**
 * @brief  format and print data
*/
void miwear_log_printf(const char *format, ...);

/**
 * @brief  hex format and print
 *
 * @param p_name: real name of p_data
 * @param p_data: pointer to hex print
 * @param len: data len
*/
void miwear_hexdump_printf(const char *p_name, const uint8_t *p_data, uint16_t len);

/**
 * @brief os environment mutex creation, if there is no os just return NULL
 *
 * @return void* pointer to mutex instance
 */
void *miwear_mutex_create(void);

/**
 * @brief os environment mutex lock
 *
 * @param mutex pointer of mutex instance created by miwear_mutex_create
 */
void miwear_mutex_lock(void *mutex);

/**
 * @brief os environment mutex unlock
 *
 * @param mutex pointer of mutex instance created by miwear_mutex_create
 */
void miwear_mutex_unlock(void *mutex);

/**
 * @brief Get the trace time, used for sdk execution time profiling
 *
 * @return int time in microseconds
 */
int miwear_get_trace_time_us(void);

/**
 * @brief allocates size bytes, and return a pointer to the allocated memory. The memory is not initialized.
 * If size is 0, then return either NULL, or a unique pointer value that can later be successfully passed to miwear_free()
 *
 * @param size          bytes size to allocate memory
 *
 * @return the pointer to the allocated memory
*/
void *miwear_malloc(size_t size);

/**
 * @brief Allocates a block of memory for an array of num elements, each of them size bytes long, and initializes
 *  all its bits to zero.
 *
 * @param num
 * @param size
 * @return void*
 */
void *miwear_calloc(size_t num, size_t size);

/**
 * @brief changes the size of memory block pointed to by ptr to size bytes.
 *
 * @param ptr
 * @param size
 *
 * @return a pointer to the newly allocated memory
*/
void *miwear_realloc(void *ptr, size_t size);

/**
 * @brief frees the memory space, which must have been returned by a previous call to miwear_malloc, miwear_realloc
 *
 * @param ptr       memory space pointer
*/
void miwear_free(void *ptr);

/**
 * @brief open file or flash data area uniquely identified by id and header, and return open handle in open_cb
 *
 * @param type see miwear_file_type_t
 * @param id uniquely identify file or flash data
 * @param id_len length of id
 * @param index index is used to distinguish different files or flash data after opened
 * @param open_cb see file_open_cb_t
 * @return int
 */
int miwear_file_open(miwear_file_type_t type, uint8_t *id, size_t id_len, int index,
                     file_open_cb_t open_cb);

/**
 * @brief read file or flash data area by handle
 *
 * @param handle handle which is returned in miwear_file_open
 * @param buf buffer to hold read data
 * @param len length of read buffer
 * @param read_cb see file_read_cb_t
 * @return int
 */
int miwear_file_read(int handle, uint8_t *buf, size_t len, file_read_cb_t read_cb);

/**
 * @brief close file or flash data area by handle
 *
 * @param handle handle which is returned in miwear_file_open
 * @param close_cb see file_close_cb_t
 * @return int
 */
int miwear_file_close(int handle, file_close_cb_t close_cb);

/**
 * @brief the interface of send data via spp(rfcomm)
 *
 *        This is a specific implementation method provided by external
 *        functions(miwear_api_port.c), such as Socket Send, Write and other
 *        APIs, it will be called by the interface inside the sdk
 *
 * @param conn_handle       handle of the connection
 * @param data              data buffer
 * @param data_len          data buffet length
 * @param miwear_spp_notify_complete_cb_t spp notify complete callback
 * @param void*             user data
 * @return miwear_status_t
 */
miwear_status_t miwear_spp_service_send_data(uint16_t conn_handle,
                                             uint8_t *data, uint16_t data_len,
                                             miwear_spp_notify_complete_cb_t cb,
                                             void *cb_data);

/**
 * @brief request to get a handle for write mass data
 *
 * @param prepare_type:          refer to miwear_mass_prepare_type_t
 * @param mass_type
 * @param id                     data id for mass data (md5)
 * @param id_len                 data id length for mass data
 * @param size                   the size of the file requested to be written (bytes)
 * @param index                  return the original value when trigger prepare_cb, refer to prepare_cb_t
 * @param md5_context            When resuming the file(prepare_type), mass data needs to be calculated and returned for subsequent calculation of the SDK
 * @param support_compress_mode  requested compression mode, it is 0 by default(no compression), will be supported in the future
 * @param prepare_cb             prepare after callback
 * @return int                   unused yet, just return 0 is fine
 */
int miwear_mass_prepare(miwear_mass_prepare_type_t prepare_type,
                        uint8_t mass_type, uint8_t *id,
                        size_t id_len, size_t size, int index,
                        void *md5_context, uint8_t support_compress_mode,
                        prepare_cb_t prepare_cb);

/**
 * @brief mass data write
 *
 * @param handle             uniquely identify handle for mass data
 * @param buf                write data buffer
 * @param buf_len            write data buffer length
 * @param packet_len_written length of the data that has been written
 * @param packet_len_total   total length of the data to be written
 * @param write_cb           write after callback
 * @return int               unused yet, just return 0 is fine
 */
int miwear_mass_write(int handle, uint8_t *buf, size_t buf_len,
                      size_t packet_len_written, size_t packet_len_total,
                      mass_write_cb_t write_cb);

/**
 * @brief the result of writing mass data
 *
 * @param handle: uniquely identify handle for mass data
 * @param result  write result, refer to miwear_mass_finish_result_t
 * @return int    unused yet, just return 0 is fine
 */
int miwear_mass_finish(int handle, int result);

/**
 * @brief   md5 init
 *
 * @param   [out] md5_context: a pointer to address which can uniquely identify the md5
 * @return  void
 * */
void miwear_md5_init(void **md5_context);

/**
 * @brief md5 update
 *
 * @param md5_context  md5 context
 * @param buf          data buffer
 * @param len          data buffet length
 * @return void
 */
void miwear_md5_update(void *md5_context, uint8_t *buf, uint32_t len);

/**
 * @brief md5 final
 *
 * @param md5_context  md5 context
 * @param result       md5 result(16 bytes)
 * @return void
 */
void miwear_md5_final(void *md5_context, uint8_t result[16]);

/**
 * @brief write ancs control point characteristics value of remote iOS device
 *
 * @param data data to write
 * @param len length of data
 */
void miwear_ancs_control_point_write(uint8_t *data, uint16_t len);

/**
 * @brief Function for reading the new value of the characteristic
 *
 * @param conn_handle:     connect handle
 * @param char_handle      character value handle
 * @param offset           offset value for the read request
 * @return void
 */
void miwear_media_ams_char_read(uint16_t conn_handle, uint16_t char_handle,
                                uint16_t offset);

/**
 * @brief Function for writing to the corresponding characteristic
 *
 * @param conn_handle      connect handle
 * @param char_handle      character value handle
 * @param data             data buffer
 * @param data_len         data buffer length
 * @param count            the count of data buffer
 * @return void
 */
void miwear_media_ams_char_write(uint16_t conn_handle, uint16_t char_handle,
                                 uint8_t *data, uint16_t data_len,
                                 uint8_t count);

/**
 * @brief used to accept/reject miwear bind procedure. for example, miwear bind is allowed only after ble bonded for iOS
 *
 * @return true
 * @return false
 */
bool miwear_bind_proc_allowed(void);

// ***********************************************************************
/***
 * @note Methods to implement by each port   --  END
*/
// ***********************************************************************

#ifdef __cplusplus
}
#endif

#endif // __MIWEAR_API_H__
