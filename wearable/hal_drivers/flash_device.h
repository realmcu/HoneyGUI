/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      flash_device.h
* @brief     This file contains structures the functions regarding flash device.
* @author    Brenda_li
* @date      2016-11-9
* @version   v1.0

  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/*****************************************************************************************
* Introduction
*****************************************************************************************
* - Flash function would be splitted into two modules: flash_driver and flash_device
* - flash_driver will handle flash_spic register to provide flash basic read/write function
* - flash_device would provide flash layout or other functions.
******************************************************************************************/
#ifndef _FLASH_DEVICE_H
#define _FLASH_DEVICE_H
/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>

/** @defgroup  FLASH_DEVICE    Flash Device
    * @brief Flash image layout and APIs
    * @{
    */

/*============================================================================*
  *                                   Macros
  *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Macros Flash Device Exported Macros
    * @brief
    * @{
    */
#define FMC_PAGE_SIZE                   0x1000
#define FMC_SEC_SECTION_LEN             (4 * 1024)
#define FMC_BLK_SECTION_LEN             (64 * 1024)
#define FMC_ERASE_PATTERN               (0xFFFFFFFF)

#define FLASH_OFFSET_TO_NO_CACHE        0x04000000 /*!< memory offset between no cache and cache flash address */


//leverage config signature from bee1
#define SIGNATURE_CONFIG                0x8721bee2

/******** bit definitions of hardfault configuration ********/
#define BIT_ENABLE_SAVE_HARDFAULT       0x00000001    /* BIT0 */
#define BIT_ENABLE_DUMP_HARDFAULT       0x00000004    /* BIT2 */
#define BIT_CLEAR_HISTROY_AFTER_DUMP    0x00000008    /* BIT3 */
#define BIT_CLEAR_HISTROY_BEFORE_SAVING 0x00000010    /* BIT4 */

/** End of FLASH_DEVICE_Exported_Macros
    * @}
    */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Types Flash Device Exported Types
  * @brief
  * @{
  */

/********************  flash erase type  *******************
FLASH_ERASE_CHIP: erase whole chip
FLASH_ERASE_SECTOR: erase specified sector(page, 4K always)
FLASH_ERASE_BLOCK: erase specified block(16 sector always)
****************************************************************/
typedef enum
{
    FLASH_ERASE_CHIP = 1,  /*!<erase whole chip*/
    FLASH_ERASE_SECTOR = 2,  /*!<erase specified sector(page, 4K always)*/
    FLASH_ERASE_BLOCK = 4,  /*!<erase specified block(16 sector always)*/
    FLASH_ERASE_PAGE = 8,
} T_ERASE_TYPE;

typedef enum
{
    FLASH_SUCCESS               =  1,
    FLASH_NOT_SUPPORT           =  0,
    FLASH_ERROR_NOT_DEF         = -1,
    FLASH_ERROR_INVALID_INPUT   = -2,
    FLASH_ERROR_OUT_OF_RANGE    = -3,
    FLASH_ERROR_TIMEOUT         = -4,
    FLASH_NOT_EXIST             = -5,
    FLASH_CONFIG_FAILED         = -6,
    FLASH_CAL_FAILED            = -7
} T_FLASH_RET;


typedef enum
{
    FLASH_EXIST_NONE        = 0,
    FLASH_EXIST_BASIC_CFG   = 0x01,
    FLASH_EXIST_ADV_CFG     = 0x02,
    FLASH_EXIST_QUERY_INFO  = 0x04,
    FLASH_EXIST_ALL         = 0x07
} T_FLASH_EXIST_LV;

/********************  Access mode of DR register in user mode  *******************/
typedef enum
{
    FLASH_MODE_1BIT         = 0,
    FLASH_MODE_2BIT         = 1,
    FLASH_MODE_4BIT         = 2
} T_FLASH_MODE;

typedef enum
{
    FLASH_DMA_AUTO_F2R = 0,
    FLASH_DMA_AUTO_R2F,
    FLASH_DMA_USER_F2R,
    FLASH_DMA_USER_R2F
} T_FLASH_DMA_TYPE;

typedef enum
{
    FLASH_SCCD,
    FLASH_OCCD,
    FLASH_OTA_BANK_0,
    FLASH_OTA_BANK_1,
    FLASH_OTA_TMP,
    FLASH_BKP_DATA1,
    FLASH_BKP_DATA2,
    FLASH_FTL,
    FLASH_HARDFAULT_RECORD,
    FLASH_TOTAL,
} T_FLASH_LAYOUT_NAME;

typedef enum
{
    flash_ioctl_get_rdid                        = 0x0006,
    flash_ioctl_get_addr_mode                   = 0x0008,
    flash_ioctl_set_seq_trans_enable            = 0x1008,
    flash_ioctl_set_sr                          = 0x100A,
    flash_ioctl_set_addr_mode                   = 0x100B,
    flash_ioctl_exec_flash_sw_reset             = 0x3005,
    flash_ioctrl_app_base                       = 0x5000,
    //add more here
    flash_ioctl_time_consuming_test_for_Byte_Program,
} T_IOCTL_CODE;

typedef void (*FlashCB)(void);

/** End of FLASH_DEVICE_Exported_Types
  * @}
  */

/*============================================================================*
  *                                   Variables
 *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Variables Flash Device Exported Variables
    * @brief
    * @{
    */

extern void *flash_sem;

/** End of FLASH_DEVICE_Exported_Variables
  * @}
  */

/*============================================================================*
  *                                   Functions
 *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Functions Flash Device Exported Functions
    * @brief
    * @{
    */

/**
 * @brief dump spic register for debug purpose
 * @param none
 * @return none
*/
extern void flash_dump_flash_info(void);

/**
 * @brief calculate crc based on given algorism
 * @param data_p  data buffer to calculate
 * @param length  length of data buffer
 * @return crc value.
*/
extern unsigned short crc16(const unsigned char *data_p, unsigned int length);


/**
 * @brief get ota bank start address by signature in flash
 * @param name name to specify which image
 * @return bank address in flash
*/
extern uint32_t flash_get_bank_addr(T_FLASH_LAYOUT_NAME name);


/**
 * @brief get specific image bank size
 * @param name name to specify which image
 * @return bank size of specified image
*/
extern uint32_t flash_get_bank_size(T_FLASH_LAYOUT_NAME name);


/**
 * @brief vendor command interface for flash related control
 * @param cmd  command id
 * @param param_1   parameter 1
 * @param param_2   parameter 2
 * @retval 0 if any error
 * @retval otherwise return value back via return value
 * @note add more ctrl code here if neccessary
*/
extern uint32_t flash_ioctl(uint32_t cmd, uint32_t param_1, uint32_t param_2);


/**
 * @brief get flash_exists status
 * @param none
 * @return flash_exists
*/
extern T_FLASH_EXIST_LV flash_get_flash_exist(void);


/**
 * @brief erase flash with semaphore locked, it could be used for concorrent usage with other flash task.
 * @param type  erase type
 * @param addr  address to erase when erase block or sector
 * @return access flash successfully or not
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note  not used in isr mode.
*/
extern bool flash_erase_locked(T_ERASE_TYPE type, uint32_t addr);

/**
 * @brief safe version of program bunch of data to flash via user mode
 * @param start_addr    start_addr address where is going to be flash in flash
 * @param data_len      data length to be program
 * @param data          data buffer to be program into
 * @return     access flash successfully or not
 * @retval true    succeed to access flash
 * @retval false   fail to access flash because other flash access is not finished
*/
extern bool flash_write_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);

/**
 * @brief safe version of program data to flash via auto mode with lock
 * @param start_addr    start_addr address where is going to be flash in flash
 * @param data          data to be program into
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note
 *      1. auto write is sync with auto read. so if your use auto read, this api is preferred
 *      2. this API will not be interrupted by other flash operation
 *      3. recommandate to use this other than flash_write
*/
extern bool flash_auto_write_locked(uint32_t start_addr, uint32_t data);

/**
 * @brief safe version of program bunch of data to flash via auto mode
 * @param start_addr    start_addr address where is going to be flash in flash
 * @param data          data to be program into
 * @param len           len of data buffer
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note  start address should be 4 byte align
*/
extern bool flash_auto_write_buffer_locked(uint32_t start_addr, uint32_t *data, uint32_t len);

/**
 * @brief safe version of flash_read
 * @param start_addr    start_addr address where is going to be read in flash
 * @param data_len      data length to be read
 * @param data          data buffer to be read into
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note SPIC only supports SPIC_FIFO_SIZE FIFO, so max SPIC_FIFO_SIZE-4 bytes allowed to be written.
 *       auto mode performance is better than this function, and auto mode read is suggested.
 *       recommand to use auto read other than this api
*/
extern bool flash_read_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);

/**
 * @brief safe version of flash_auto_read
 * @param addr address to read
 * @param data data at addr
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
*/
extern bool flash_auto_read_locked(uint32_t addr, uint32_t *data);

/**
 * @brief safe version of flash_split_read
 * @param start_addr    start_addr address where is going to be read in flash
 * @param data_len      data length to be read
 * @param data          data buffer to be read into
 * @param counter       split counter to info that how many split times occur
 *                      return 0xFFFFFFFF if not align 4
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note In order to enhance throughput, only word align (4 bytes) is allowed to be written.
 *       auto mode performance is better than this function, and auto mode read is suggested.
 *       recommand to use auto read other than this api
*/
extern bool flash_split_read_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data,
                                    uint32_t *counter);

/**
 * @brief safe version of flash auto dma read
 * @param dma_type      identify user / auto mode and flash_2_ram / ram_2_flash
 * @param flash_cb      callback function after DMA transmit completed if need
 * @param src_addr      address where is going to be read in flash
 * @param dst_addr      address where is going to write data to
 * @param data_len      data length to be read
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note    @ref FLASH_GDMA_HANDLER() will be executed after flash transmit by DMA complete.
*/
extern bool flash_auto_dma_read_locked(T_FLASH_DMA_TYPE dma_type, FlashCB flash_cb,
                                       uint32_t src_addr, uint32_t dst_addr, uint32_t data_len);

/**
 * @brief safe version of flash auto seqential transition dma read
 * @param dma_type      identify user / auto mode and flash_2_ram / ram_2_flash
 * @param flash_cb      callback function after DMA transmit completed if need
 * @param src_addr      address where is going to be read in flash
 * @param dst_addr      address where is going to write data to
 * @param data_len      data length to be read
 * @retval true succeed to access flash
 * @retval false fail to access flash because other flash access is not finished
 * @note    FLASH_GDMA_HANDLER() will be executed after flash transmit by DMA complete.
*/
extern bool flash_auto_seq_trans_dma_read_locked(T_FLASH_DMA_TYPE dma_type, FlashCB flash_cb,
                                                 uint32_t src_addr, uint32_t dst_addr, uint32_t data_len);

/**
 * @brief setup user split read by DMA + HW HSK
 * @param flash_cb      flash callback using dma
 * @param start_addr    start address to read
 * @param dst_addr      destination address of the read data
 * @param data_len      data length to be read
 * @return
*/
extern bool flash_split_read_by_dma_locked(FlashCB flash_cb, uint32_t start_addr, uint32_t dst_addr,
                                           uint32_t data_len);

/**
 * @brief set specific bit to specific address with flash lock mechanism
 * @param addr specific address, should not be flash address!
 * @param bit bit position to write
 * @return none
*/
extern void flash_set_bit(uint32_t *addr, uint32_t bit);

/**
 * @brief check if specific bit has been set with flash lock mechanism.
 * @param flag  value to be checked
 * @param bit bit position to check
 * @retval true if specified bit position of the flag has been set
 * @retval  false otherwise
*/
extern bool flash_get_bit(uint32_t flag, uint32_t bit);


/**
* @brief    get block protect level
* @param    *bp_lv  a set of BPx ~ BP0
* @return   success or not
*/
extern bool flash_get_block_protect_locked(uint8_t *bp_lv);

/**
* @brief    set block protect by map
* @param    bp_lv a set of BPx ~ BP0
* @return   success or not
*/
extern bool flash_set_block_protect_locked(uint8_t bp_lv);

/**
* @brief    only unlock prefer section by addres
* @param    unlock_addr address section to be unlocked
* @param    *old_bp_lv before unlock
* @return   success or not
*/
extern bool flash_sw_protect_unlock_by_addr_locked(uint32_t unlock_addr, uint8_t *old_bp_lv);

/**
 * @brief  flash try to switch to high speed bit mode
 * @note  switch back to 1 bit mode, if flash switch to high speed bit mode fail
 * @param  bit_mode config bit mode @ref T_FLASH_MODE
 * @retval FLASH_SUCCESS success
 * @retval others1 fail
  */
extern T_FLASH_RET flash_try_high_speed(T_FLASH_MODE bit_mode);

extern bool flash_ds_erase(T_ERASE_TYPE type, uint32_t addr);

extern bool flash_ds_auto_write(uint32_t addr, uint32_t data);

extern bool flash_ds_auto_read(uint32_t addr, uint32_t *data);

extern bool flash_ds_write(uint32_t addr, uint32_t data_len, uint8_t *data);

extern bool flash_ds_read(uint32_t addr, uint32_t data_len, uint8_t *data);

/** @} */ /* End of group FLASH_DEVICE_Exported_Functions */


/** @} */ /* End of group FLASH_DEVICE */


#endif //_FLASH_DEVICE_H




