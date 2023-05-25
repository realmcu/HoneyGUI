/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BOND_H_
#define _APP_BOND_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BOND App Bond
  * @brief
  * @{
  */

/** bitmask of bond flag */
#define BOND_FLAG_HFP                   0x00000001
#define BOND_FLAG_A2DP                  0x00000002
#define BOND_FLAG_SPP                   0x00000004
#define BOND_FLAG_PBAP                  0x00000008
#define BOND_FLAG_IAP                   0x00000010
#define BOND_FLAG_HSP                   0x00000020
#define BOND_FLAG_AMA                   0x00000040
#define BOND_FLAG_HID                   0x00000080

/**
    * @brief  Set bud2phone link record to highest priority.
    * @param  bd_addr
    * @return void
    */
void app_bond_set_priority(uint8_t *bd_addr);



#define MAX_BOND_INFO_NUM        8
#define MAX_DEVICE_NAME_NUM      20
#define TEMP_MAX_SEARCH          2

typedef enum
{
    T_DEVICE_TYPE_PHONE           = 0x00,
    T_DEVICE_TYPE_EAR             = 0x01,
    T_DEVICE_TYPE_DEFAULT         = 0x02,
} T_DEVICE_TYPE;

//data length 52
typedef struct
{
    uint8_t bd_addr[6];
    bool used;
    T_DEVICE_TYPE device_type;
    uint16_t device_name[MAX_DEVICE_NAME_NUM];
    uint16_t priority;
    uint16_t flag;//8 flag_exit_address, 8 name_len
} T_APP_BOND_DEVICE;

/**
    * @brief  Load bond device info from ftl
    * @param  bond_info
    * @return uint32_t
    */
uint32_t app_bt_bond_load_device_info_from_ftl(T_APP_BOND_DEVICE *bond_info);

/**
    * @brief  Save bond device info to ftl
    * @param  bond_info
    * @return uint32_t
    */
uint32_t app_bt_bond_save_device_info_to_ftl(void *bond_info);

/**
    * @brief  Get active device num by type
    * @param  device_type
    * @return uint8_t
    */
uint8_t app_bt_bond_get_active_num_by_type(T_DEVICE_TYPE device_type);

/**
    * @brief  Add the device to the bond array
    * @param  bd_addr device_type
    * @return bool
    */
bool app_bt_bond_add_device(uint8_t *bd_addr, T_DEVICE_TYPE device_type);

/**
    * @brief  Get the device type by cod
    * @param  cod
    * @return T_DEVICE_TYPE
    */
T_DEVICE_TYPE app_bt_bond_get_device_type(uint32_t cod);

/**
    * @brief  Set the device inactive
    * @param  bd_addr
    * @return bool
    */
bool app_bt_bond_inactive_device(uint8_t *bd_addr);

uint8_t app_bt_bond_add_temp_search_device(uint8_t *bd_addr, uint32_t cod, uint16_t *name,
                                           uint8_t len);
bool app_bt_bond_add_temp_search_device_name_info(uint8_t *bd_addr, uint16_t *name, uint8_t len);

T_APP_BOND_DEVICE *app_bt_bond_get_temp_search_device(uint8_t *bd_addr);

uint8_t *app_bt_bond_check_exit_other_active_device(uint8_t *addr, T_DEVICE_TYPE device_type);

bool app_bt_bond_add_search_device(uint8_t *bd_addr);

bool app_bt_bond_check_exist_device_info(void);

T_APP_BOND_DEVICE *app_bt_bond_get_active_device_by_type(T_DEVICE_TYPE device_type);

void app_bt_bond_free_temp_search_device(void);

bool app_bt_bond_del_bond_device(uint8_t *bd_addr);

uint8_t *app_bt_bond_get_highest_priority(void);

uint8_t app_bt_bond_get_num_by_type(T_DEVICE_TYPE device_type);

void app_bt_bond_free_all_device(void);

T_DEVICE_TYPE app_bt_bond_get_cod_type(uint8_t *bd_addr);

bool app_bt_bond_check_active_device_info_by_addr(uint8_t *bd_addr);

uint8_t app_bt_bond_get_index_by_addr(uint8_t *bd_addr);

bool app_bt_bond_inactive_device_by_index(uint8_t index);

T_APP_BOND_DEVICE *app_bt_bond_get_device_by_addr(uint8_t *bd_addr);

uint8_t app_bt_bond_get_num_ear(void);

bool app_bt_bond_linkback(uint16_t retry_timeout);

void app_bt_bond_sort_priority(T_APP_BOND_DEVICE *temp, int len);

uint8_t app_bt_bond_add_temp_cache_device(uint8_t *bd_addr, uint32_t cod, uint16_t *name,
                                          uint8_t len);

T_APP_BOND_DEVICE *app_bt_bond_get_temp_cache_device_by_index(uint8_t index);

void app_bt_bond_free_temp_cache_device(void);

bool app_bt_bond_temp_cache_exsit_device(void);

void app_bt_bond_temp_cache_save_to_search(void);

bool get_connect_status_flag(void);

void set_connect_status_flag(bool flag);

void set_connect_status_flag_addr(uint8_t *addr);

uint8_t *get_connect_status_flag_addr(void);

/** End of APP_BOND
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BOND_H_ */
