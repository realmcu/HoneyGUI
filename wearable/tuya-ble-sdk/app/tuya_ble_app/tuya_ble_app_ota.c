
#include <trace.h>
#include <string.h>
#include <gap_msg.h>
#include <board.h>
#include <app_msg.h>
#include "os_timer.h"
#include "tuya_ble_app_ota.h"
#include "rtl876x_wdg.h"
#include "rtl_uart.h"
#include "trace.h"
#include "dfu_service.h"
#include "gap_msg.h"
#include "app_msg.h"
#include "flash_device.h"
#include "dfu_flash.h"
#include "patch_header_check.h"
#include "board.h"
#include "flash_adv_cfg.h"
#include "os_sched.h"
#include "mem_config.h"
#include "otp.h"
#include "rtl876x_hw_aes.h"
#include "patch_header_check.h"
//#include "user_flash_driver.h"
#include "os_sched.h"
#include "app_section.h"
#include "os_sync.h"
#include "platform_utils.h"
#include "flash_adv_cfg.h"
#include "tuya_ble_type.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_app_demo.h"
#include "ftl.h"

#define  DFU_SETTINGS_ADDR 0x800

static volatile tuya_ota_status_t tuya_ota_status;

typedef struct
{
    uint32_t firmware_file_version;
    uint32_t firmware_file_length;
    uint32_t firmware_file_crc;
    uint8_t  firmware_file_md5[16];
} dfu_firmware_file_info_t;

typedef struct
{
    uint32_t firmware_image_offset_last;
    uint32_t firmware_image_crc_last;
} dfu_firmware_progress_t;


typedef struct
{
    uint32_t
    crc;                /**< CRC for the stored DFU settings, not including the CRC itself. If 0xFFFFFFF, the CRC has never been calculated. */
    uint32_t            settings_version;   /**< Version of the current DFU settings struct layout. */
    dfu_firmware_file_info_t file_info;
    uint32_t            write_offset;
    dfu_firmware_progress_t progress;
    uint32_t res[8];
} dfu_settings_t;


static dfu_settings_t dfu_settings;


#define DFU_MAX_CHECK_BUFFER_SIZE  512
#define DFU_TEMP_BUFFER_SIZE       2048

#define     APP_NEW_FW_MAX_SIZE          OTP->ota_bank0_size


static uint8_t TempBufferHead[DFU_TEMP_BUFFER_SIZE];

static uint8_t erase_error = 0;

static uint16_t ota_tmp_buf_used_size = 0;
static uint8_t prev_bp_lv = 0xfe;
bool flstatus = false;//flash lock status
uint32_t signal;//os lock signal

uint32_t dfu_flash_check_blank(uint16_t signature, uint32_t offset, uint16_t nSize);


uint32_t sil_dfu_update(uint16_t signature, uint32_t offset, uint32_t length,
                        uint32_t/*void*/ *p_void);

//extern uint32_t patch_version,fsbl_version;


#define MAX_DFU_DATA_LEN  DFU_MAX_CHECK_BUFFER_SIZE

#define crc32_compute tuya_ble_crc32_compute

//static uint8_t file_crc_check_in_flash(uint32_t length,uint32_t *crc);

static uint16_t current_package = 0;
static uint16_t last_package = 0;

static uint32_t settings_crc_get(dfu_settings_t const *p_settings)
{
    // The crc is calculated from the s_dfu_settings struct, except the crc itself
    return crc32_compute((uint8_t *)(p_settings) + 4, sizeof(dfu_settings_t) - 4, NULL);
}

static bool crc_ok(dfu_settings_t const *p_settings)
{
    if (p_settings->crc != 0xFFFFFFFF)
    {
        // CRC is set. Content must be valid
        uint32_t crc = settings_crc_get(p_settings);
        if (crc == p_settings->crc)
        {
            return true;
        }
    }
    return false;
}


static uint32_t dfu_settings_write(void)
{
    dfu_settings.crc = settings_crc_get(&dfu_settings);
    return ftl_save(&dfu_settings, DFU_SETTINGS_ADDR, sizeof(dfu_settings_t));
}

static uint32_t dfu_settings_read(void)
{
    return ftl_load(&dfu_settings, DFU_SETTINGS_ADDR, sizeof(dfu_settings_t));
}

uint32_t tuya_ota_init(void)
{
    tuya_ota_status = TUYA_OTA_STATUS_NONE;
    //uint32_t crc_check;
    current_package = 0;
    last_package = 0;
    memset(TempBufferHead, 0, sizeof(TempBufferHead));
    ota_tmp_buf_used_size = 0;
    //memset(&dfuPara,0,sizeof(dfuPara));

    if (dfu_settings_read())
    {
        TUYA_BLE_LOG_ERROR("dfu_settings_read failed !");
    }

    TUYA_BLE_LOG_DEBUG("dfu_settings.crc = 0x%08x ", dfu_settings.crc);
    TUYA_BLE_LOG_DEBUG("dfu_settings.progress.firmware_image_offset_last = %d ",
                       dfu_settings.progress.firmware_image_offset_last);
    TUYA_BLE_LOG_DEBUG("dfu_settings.progress.firmware_image_crc_last = 0x%08x ",
                       dfu_settings.progress.firmware_image_crc_last);

    if (!crc_ok(&dfu_settings))
    {
        TUYA_BLE_LOG_DEBUG("read dfu_settings...crc error!");
        memset(&dfu_settings, 0, sizeof(dfu_settings_t));
        if (dfu_settings_write())
        {
            TUYA_BLE_LOG_ERROR("dfu_settings_write failed !");
        }
    }

    return 0;
}



static void tuya_ota_start_req(uint8_t *recv_data, uint32_t recv_len)
{
    uint8_t p_buf[70];
    uint8_t payload_len = 0;
    uint8_t encry_mode = 0;
    uint32_t current_version = TY_APP_VER_NUM;
    tuya_ble_ota_response_t res_data;

    if (tuya_ota_status != TUYA_OTA_STATUS_NONE)
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_NONE  and is : %d !",
                           tuya_ota_status);
        return;
    }

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = TUYA_OTA_VERSION;
    p_buf[2] = 0;
    p_buf[3] = current_version >> 24;
    p_buf[4] = current_version >> 16;
    p_buf[5] = current_version >> 8;
    p_buf[6] = current_version;
    p_buf[7] = (uint8_t)(MAX_DFU_DATA_LEN >> 8);
    p_buf[8] = (uint8_t)(MAX_DFU_DATA_LEN);
    tuya_ota_status = TUYA_OTA_STATUS_START;
    payload_len = 9;

    res_data.type =  TUYA_BLE_OTA_REQ;
    res_data.data_len = payload_len;
    res_data.p_data = p_buf;

    if (tuya_ble_ota_response(&res_data) != TUYA_BLE_SUCCESS)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_start_response failed.");
    }

}

static uint8_t  flash_auto_read_buffer(uint32_t addr, uint32_t length, uint8_t *buf)
{
    uint32_t data_temp = 0;
    uint32_t cnt;
    if ((length == 0) || (length % 4))
    {
        TUYA_BLE_LOG_ERROR("flash_auto_read_buffer data length error.");
        return 1;
    }
    cnt = length / 4;
    //tuya_ble_device_enter_critical();
    for (uint32_t i = 0; i < cnt; i++)
    {
        data_temp = flash_auto_read((addr + i * 4) | FLASH_OFFSET_TO_NO_CACHE);
        memcpy(buf + i * 4, (uint8_t *)&data_temp, 4);
    }
    // tuya_ble_device_exit_critical();
    return 0;

}


static  uint8_t file_crc_check_in_flash(uint32_t length, uint32_t *crc)
{
    static uint8_t file_check_buf[256];
    uint8_t result = 0;

    if ((length == 0) || (length % 4) || (length < 256))
    {
        TUYA_BLE_LOG_ERROR("file_crc_check_in_flash data length error.");
        return 1;
    }

    uint32_t crc_temp = 0;
    uint32_t read_addr;
    uint32_t cnt = length / 256;
    uint32_t remainder = length % 256;

    read_addr = get_temp_ota_bank_addr_by_img_id((T_IMG_ID)AppPatch);
    TUYA_BLE_LOG_DEBUG("read_addr = 0x%08x", read_addr);
    //unlock_flash_all();

    if (flash_auto_read_buffer(read_addr, 256, file_check_buf))
    {
        // lock_flash();
        return 1;
    }
    TUYA_BLE_LOG_HEXDUMP_DEBUG("file_crc_check_in_flash:buf data before:", file_check_buf, 12);
    T_IMG_CTRL_HEADER_FORMAT *p_header = (T_IMG_CTRL_HEADER_FORMAT *) file_check_buf;
    p_header->ctrl_flag.flag_value.not_ready = 0;
    TUYA_BLE_LOG_HEXDUMP_DEBUG("file_crc_check_in_flash:buf data after:", file_check_buf, 12);
    read_addr += 256;
    crc_temp = crc32_compute(file_check_buf, 256, &crc_temp);

    for (uint32_t i = 1; i < cnt; i++)
    {
        if (flash_auto_read_buffer(read_addr, 256, file_check_buf))
        {
            // lock_flash();
            return 1;
        }
        crc_temp = crc32_compute(file_check_buf, 256, &crc_temp);
        read_addr += 256;
    }

    if (remainder > 0)
    {
        if (flash_auto_read_buffer(read_addr, remainder, file_check_buf))
        {
            // lock_flash();
            return 1;
        }
        crc_temp = crc32_compute(file_check_buf, remainder, &crc_temp);
        read_addr += remainder;
    }

    *crc = crc_temp;
    // lock_flash();
    return 0;
}




static void tuya_ota_file_info_req(uint8_t *recv_data, uint32_t recv_len)
{
    uint8_t p_buf[30];
    uint8_t payload_len = 0;
    uint8_t encry_mode = 0;
    uint32_t file_version;
    uint32_t file_length;
    uint32_t file_crc;
    bool file_md5;
    // uint8_t file_md5[16];
    uint8_t state = 0;
    tuya_ble_ota_response_t res_data;
    uint32_t crc_temp  = 0;

    //file_crc_check_in_flash(256,&file_crc);
    if (tuya_ota_status != TUYA_OTA_STATUS_START)
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_START  and is : %d !",
                           tuya_ota_status);
        return;
    }

    if (recv_data[0] != TUYA_OTA_TYPE)
    {
        TUYA_BLE_LOG_ERROR("current ota fm type is not 0!");
        return;
    }

    file_version = recv_data[9] << 24;
    file_version += recv_data[10] << 16;
    file_version += recv_data[11] << 8;
    file_version += recv_data[12];

    if (memcmp(dfu_settings.file_info.firmware_file_md5,
               &recv_data[17 + TUYA_BLE_PRODUCT_ID_DEFAULT_LEN], 16) == 0)
    {
        file_md5 = true;
    }
    else
    {
        file_md5 = false;
    }

    file_length = recv_data[29] << 24;
    file_length += recv_data[30] << 16;
    file_length += recv_data[31] << 8;
    file_length += recv_data[32];

    file_crc = recv_data[33] << 24;
    file_crc += recv_data[34] << 16;
    file_crc += recv_data[35] << 8;
    file_crc += recv_data[36];

    TUYA_BLE_LOG_DEBUG("dfu_settings.file_info.firmware_file_version = 0x%04x ---- file_version = 0x%04x",
                       dfu_settings.file_info.firmware_file_version, file_version);
    TUYA_BLE_LOG_DEBUG("dfu_settings.file_info.firmware_file_length = %d ---- file_length = %d",
                       dfu_settings.file_info.firmware_file_length, file_length);
    TUYA_BLE_LOG_DEBUG("dfu_settings.file_info.firmware_file_crc = 0x%08x ---- file_crc = 0x%08x",
                       dfu_settings.file_info.firmware_file_crc, file_crc);
    TUYA_BLE_LOG_DEBUG("dfu_settings.progress.firmware_image_offset_last = %d ",
                       dfu_settings.progress.firmware_image_offset_last);
    TUYA_BLE_LOG_DEBUG("dfu_settings.progress.firmware_image_crc_last = 0x%08x ",
                       dfu_settings.progress.firmware_image_crc_last);
    TUYA_BLE_LOG_DEBUG("file_md5 = %d", file_md5);
    if ((file_version > TY_APP_VER_NUM) && (file_length <= APP_NEW_FW_MAX_SIZE))
    {

        if (file_md5 && (dfu_settings.file_info.firmware_file_version == file_version) &&
            (dfu_settings.file_info.firmware_file_length == file_length)
            && (dfu_settings.file_info.firmware_file_crc == file_crc))
        {
            state = 0;
        }
        else
        {
            TUYA_BLE_LOG_DEBUG("file info check failed.");
            memset(&dfu_settings, 0, sizeof(dfu_settings_t));
            dfu_settings.progress.firmware_image_crc_last = 0;
            dfu_settings.file_info.firmware_file_version = file_version;
            dfu_settings.file_info.firmware_file_length = file_length;
            dfu_settings.file_info.firmware_file_crc = file_crc;
            memcpy(dfu_settings.file_info.firmware_file_md5, &recv_data[17 + TUYA_BLE_PRODUCT_ID_DEFAULT_LEN],
                   16);
            //dfu_settings.write_offset = dfu_settings.progress.firmware_image_offset_last;
            state = 0;
            dfu_settings_write();
        }

    }
    else
    {
        if (file_version <= TY_APP_VER_NUM)
        {
            TUYA_BLE_LOG_ERROR("ota file version error !");
            state = 2;
        }
        else
        {
            TUYA_BLE_LOG_ERROR("ota file length is bigger than rev space !");
            state = 3;
        }
    }

    memset(p_buf, 0, sizeof(p_buf));
    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = state;
    if (state == 0)
    {

        if (dfu_settings.progress.firmware_image_offset_last > 0)
        {
            crc_temp  = 0;
            file_crc_check_in_flash(dfu_settings.progress.firmware_image_offset_last, &crc_temp);

            TUYA_BLE_LOG_DEBUG("file_crc_check_in_flash-firmware_image_crc_last = 0x%08x ---- crc_temp = 0x%08x",
                               dfu_settings.progress.firmware_image_crc_last, crc_temp);
            if (crc_temp != dfu_settings.progress.firmware_image_crc_last)
            {
                dfu_settings.progress.firmware_image_offset_last = 0;
                dfu_settings.progress.firmware_image_crc_last = 0;
                dfu_settings.write_offset = dfu_settings.progress.firmware_image_offset_last;
                dfu_settings_write();
            }

        }
        p_buf[2] = dfu_settings.progress.firmware_image_offset_last >> 24;
        p_buf[3] = dfu_settings.progress.firmware_image_offset_last >> 16;
        p_buf[4] = dfu_settings.progress.firmware_image_offset_last >> 8;
        p_buf[5] = (uint8_t)dfu_settings.progress.firmware_image_offset_last;
        p_buf[6] = dfu_settings.progress.firmware_image_crc_last >> 24;
        p_buf[7] = dfu_settings.progress.firmware_image_crc_last >> 16;
        p_buf[8] = dfu_settings.progress.firmware_image_crc_last >> 8;
        p_buf[9] = (uint8_t)dfu_settings.progress.firmware_image_crc_last;
        tuya_ota_status = TUYA_OTA_STATUS_FILE_INFO;
        current_package = 0;
        last_package = 0;
        memset(TempBufferHead, 0, sizeof(TempBufferHead));
        ota_tmp_buf_used_size = 0;

        TUYA_BLE_LOG_DEBUG("ota file length  : %d", dfu_settings.file_info.firmware_file_length);
        TUYA_BLE_LOG_DEBUG("ota file  crc    : 0x%04x", dfu_settings.file_info.firmware_file_crc);
        TUYA_BLE_LOG_DEBUG("ota file version : 0x%04x", dfu_settings.file_info.firmware_file_version);
        // NRF_LOG_DEBUG("ota file md5 : 0x%04x",s_dfu_settings.progress.firmware_file_length);
        TUYA_BLE_LOG_DEBUG("ota firmware_image_offset_last : 0x%04x",
                           dfu_settings.progress.firmware_image_offset_last);
        TUYA_BLE_LOG_DEBUG("ota firmware_image_crc_last    : 0x%04x",
                           dfu_settings.progress.firmware_image_crc_last);
        TUYA_BLE_LOG_DEBUG("ota firmware   write offset    : 0x%04x", dfu_settings.write_offset);

    }
    payload_len = 26;

    res_data.type =  TUYA_BLE_OTA_FILE_INFO;
    res_data.data_len = payload_len;
    res_data.p_data = p_buf;

    if (tuya_ble_ota_response(&res_data) != TUYA_BLE_SUCCESS)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_file_info_response failed.");
    }

}


static void tuya_ota_offset_req(uint8_t *recv_data, uint32_t recv_len)
{
    uint8_t p_buf[5];
    uint8_t payload_len = 0;
    uint8_t encry_mode = 0;
    uint32_t offset = 0, offset_temp = 0;
    tuya_ble_ota_response_t res_data;

    if (tuya_ota_status != TUYA_OTA_STATUS_FILE_INFO)
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_FILE_INFO  and is : %d !",
                           tuya_ota_status);
        return;
    }

    offset  = recv_data[1] << 24;
    offset += recv_data[2] << 16;
    offset += recv_data[3] << 8;
    offset += recv_data[4];

    if ((offset == 0) && (dfu_settings.progress.firmware_image_offset_last != 0))
    {
        dfu_settings.progress.firmware_image_crc_last = 0;
        dfu_settings.progress.firmware_image_offset_last = 0;
        dfu_settings.write_offset = dfu_settings.progress.firmware_image_offset_last;
        dfu_settings_write();
    }

    if (dfu_settings.progress.firmware_image_offset_last > 0)
    {
        offset_temp = dfu_settings.progress.firmware_image_offset_last % FMC_SEC_SECTION_LEN;
        if (offset_temp)
        {
            dfu_settings.progress.firmware_image_offset_last -= offset_temp;
            if (dfu_settings.progress.firmware_image_offset_last > 0)
            {
                file_crc_check_in_flash(dfu_settings.progress.firmware_image_offset_last,
                                        &dfu_settings.progress.firmware_image_crc_last);
            }
            else
            {
                dfu_settings.progress.firmware_image_crc_last = 0;
            }
            TUYA_BLE_LOG_DEBUG("tuya_ota_offset_req new firmware_image_offset_last : 0x%04x",
                               dfu_settings.progress.firmware_image_offset_last);
            TUYA_BLE_LOG_DEBUG("tuya_ota_offset_req new firmware_image_crc_last    : 0x%04x",
                               dfu_settings.progress.firmware_image_crc_last);
            dfu_settings.write_offset = dfu_settings.progress.firmware_image_offset_last;
            dfu_settings_write();
        }
    }
    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = dfu_settings.progress.firmware_image_offset_last >> 24;
    p_buf[2] = dfu_settings.progress.firmware_image_offset_last >> 16;
    p_buf[3] = dfu_settings.progress.firmware_image_offset_last >> 8;
    p_buf[4] = (uint8_t)dfu_settings.progress.firmware_image_offset_last;

    tuya_ota_status = TUYA_OTA_STATUS_FILE_OFFSET;

    payload_len = 5;

    res_data.type =  TUYA_BLE_OTA_FILE_OFFSET_REQ;
    res_data.data_len = payload_len;
    res_data.p_data = p_buf;

    if (tuya_ble_ota_response(&res_data) != TUYA_BLE_SUCCESS)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_offset_response failed.");
    }

}



static void tuya_ota_data_req(uint8_t *recv_data, uint32_t recv_len)
{
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint8_t state = 0;
    uint16_t data_length;
    uint8_t *p_value;
    // uint8_t * p_balloc_buf;
    tuya_ble_ota_response_t res_data;
    T_DFU_CTRL_POINT dfu_control_point;

    if ((tuya_ota_status != TUYA_OTA_STATUS_FILE_OFFSET) &&
        (tuya_ota_status != TUYA_OTA_STATUS_FILE_DATA))
    {
        TUYA_BLE_LOG_ERROR("current ota status is not TUYA_OTA_STATUS_FILE_OFFSET  or TUYA_OTA_STATUS_FILE_DATA and is : %d !",
                           tuya_ota_status);
        return;
    }

    state = 0;

    current_package = (recv_data[1] << 8) | recv_data[2];
    data_length = (recv_data[3] << 8) | recv_data[4];

    if ((current_package != (last_package + 1)) && (current_package != 0))
    {
        TUYA_BLE_LOG_ERROR("ota received package number error.received package number : %d",
                           current_package);
        state = 1;
        goto ota_data_response;
    }
    else  if (data_length > MAX_DFU_DATA_LEN)
    {
        TUYA_BLE_LOG_ERROR("ota received package data length error : %d", data_length);
        state = 5;
        goto ota_data_response;
    }
    else
    {
        if ((current_package == 0) && (dfu_settings.write_offset == 0))
        {
            p_value  = &recv_data[7];
            dfu_control_point.p.start_dfu.ic_type = (*p_value);
            p_value += 1;
            dfu_control_point.p.start_dfu.secure_version = (*p_value);
            p_value += 1;
            LE_ARRAY_TO_UINT16(dfu_control_point.p.start_dfu.ctrl_flag.value, p_value);
            p_value += 2;
            LE_ARRAY_TO_UINT16(dfu_control_point.p.start_dfu.signature, p_value);
            p_value += 2;
            LE_ARRAY_TO_UINT16(dfu_control_point.p.start_dfu.crc16, p_value);
            p_value += 2;

            LE_ARRAY_TO_UINT32(dfu_control_point.p.start_dfu.image_length, p_value);

            TUYA_BLE_LOG_DEBUG("DFU_OPCODE_START_DFU: ic_type=0x%x, secure_version=0x%x, ctrl_flag.value=0x%x, signature=0x%x,crc16=0x%04x, image_length=0x%x",
                               dfu_control_point.p.start_dfu.ic_type,
                               dfu_control_point.p.start_dfu.secure_version,
                               dfu_control_point.p.start_dfu.ctrl_flag.value,
                               dfu_control_point.p.start_dfu.signature,
                               dfu_control_point.p.start_dfu.crc16,
                               dfu_control_point.p.start_dfu.image_length
                              );
            if (dfu_control_point.p.start_dfu.ic_type != 0x05 ||
                dfu_control_point.p.start_dfu.signature != AppPatch  \
                || (dfu_control_point.p.start_dfu.image_length + IMG_HEADER_SIZE) !=
                dfu_settings.file_info.firmware_file_length)
            {
                state = 4;
                TUYA_BLE_LOG_ERROR("tuya_uart_ota_start_req_process error,status = %d ", state);
                goto ota_data_response;
            }

        }

        if (dfu_settings.write_offset + ota_tmp_buf_used_size + data_length >
            dfu_settings.file_info.firmware_file_length)
        {
            TUYA_BLE_LOG_ERROR("dfu_service_handle_packet_req: p_dfu->cur_offset=%d, ota_temp_buf_used_size =%d, length= %d, image_total_length = %d ",
                               dfu_settings.write_offset,
                               ota_tmp_buf_used_size,
                               data_length,
                               dfu_settings.file_info.firmware_file_length
                              );
            state = 2;
            TUYA_BLE_LOG_ERROR("DFU_LENGTH_ERROR");
            goto ota_data_response;
        }
        else
        {
            memcpy(TempBufferHead + ota_tmp_buf_used_size, recv_data + 7, data_length);
            ota_tmp_buf_used_size += data_length;

            if (ota_tmp_buf_used_size == 2048 ||
                dfu_settings.write_offset + ota_tmp_buf_used_size == dfu_settings.file_info.firmware_file_length)
            {
                unlock_flash_all();
                if (sil_dfu_update(AppPatch, dfu_settings.write_offset, ota_tmp_buf_used_size,
                                   (uint32_t *)TempBufferHead) == 0)
                {
                    lock_flash();
                    if (dfu_settings.write_offset == 0)
                    {
                        T_IMG_CTRL_HEADER_FORMAT *p_header = (T_IMG_CTRL_HEADER_FORMAT *) TempBufferHead;
                        p_header->ctrl_flag.flag_value.not_ready = 0;
                    }
                    dfu_settings.write_offset += ota_tmp_buf_used_size;
                    dfu_settings.progress.firmware_image_crc_last = crc32_compute(TempBufferHead, ota_tmp_buf_used_size,
                                                                                  &dfu_settings.progress.firmware_image_crc_last);
                    dfu_settings.progress.firmware_image_offset_last += ota_tmp_buf_used_size;

                    if ((current_package + 1) % 32 == 0)
                    {
                        dfu_settings_write();
                    }

                    if (dfu_settings.write_offset == dfu_settings.file_info.firmware_file_length)
                    {
                        //
                    }
                    ota_tmp_buf_used_size = 0;
                }
                else
                {
                    lock_flash();
                    state = 2;
                    TUYA_BLE_LOG_ERROR("tuya_uart_ota_data_process error,status = %d ", state);
                    goto ota_data_response;
                }
            }

        }

    }

ota_data_response:

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = state;

    tuya_ota_status = TUYA_OTA_STATUS_FILE_DATA;

    payload_len = 2;

    res_data.type =  TUYA_BLE_OTA_DATA;
    res_data.data_len = payload_len;
    res_data.p_data = p_buf;

    if (tuya_ble_ota_response(&res_data) != TUYA_BLE_SUCCESS)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_data_response failed.");
    }

    if (state != 0)
    {
        TUYA_BLE_LOG_ERROR("ota error so free!");
        tuya_ota_status = TUYA_OTA_STATUS_NONE;
        tuya_ota_init_disconnect();
        //memset(&dfu_settings, 0, sizeof(dfu_settings_t));
        //dfu_settings_write();
    }
    else
    {
        last_package = current_package;
    }


}


static void on_data_write_request_sched(void)
{
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint8_t state;
    bool check_result;
    tuya_ble_ota_response_t res_data;
    uint8_t old_bp_lv = 0x02;

    if (dfu_settings.progress.firmware_image_offset_last == dfu_settings.file_info.firmware_file_length)
    {
        TUYA_BLE_LOG_DEBUG("Whole firmware image received. Postvalidating.");
        /*
                uint32_t crc_temp  = 0;
                if(file_crc_check_in_flash(dfu_settings.progress.firmware_image_offset_last,&crc_temp)==0)
                {
                    if(dfu_settings.progress.firmware_image_crc_last != crc_temp)
                    {
                        TUYA_BLE_LOG_WARNING("file crc check in flash diff from crc_last. crc_temp = 0x%04x,crc_last = 0x%04x",crc_temp,dfu_settings.progress.firmware_image_crc_last);
                        dfu_settings.progress.firmware_image_crc_last = crc_temp;
                    }

                }
        */

        flstatus = flash_lock(FLASH_LOCK_USER_MODE_READ);//signal = os_lock();
        check_result = dfu_check_checksum(AppPatch);
        flash_unlock(FLASH_LOCK_USER_MODE_READ);//os_unlock(signal);

        TUYA_BLE_LOG_DEBUG("dfu_act_notify_valid, check_result:%d (1: Success, 0: Fail)", check_result);

        if (check_result)
        {
            memset(&dfu_settings, 0, sizeof(dfu_settings_t));
            dfu_settings_write();
            state = 0;

        }
        else
        {
            state = 2;
        }

    }
    else
    {
        state = 1;
    }

    p_buf[0] = TUYA_OTA_TYPE;
    p_buf[1] = state;
    tuya_ota_status = TUYA_OTA_STATUS_NONE;
    payload_len = 2;
    res_data.type =  TUYA_BLE_OTA_END;
    res_data.data_len = payload_len;
    res_data.p_data = p_buf;

    if (tuya_ble_ota_response(&res_data) != TUYA_BLE_SUCCESS)
    {
        TUYA_BLE_LOG_ERROR("tuya_ota_end_response failed.");
    }

    if (state == 0)
    {
        //tuya_bsp_delay_ms(1000);
        TUYA_BLE_LOG_INFO("start reset~~~.");
        if (flash_sw_protect_unlock_by_addr_locked(0x800000, &old_bp_lv))
        {
            APP_PRINT_INFO1("Unlock success! old_bp_lv=%d", old_bp_lv);
        }
        os_delay(1000);
        //tuya_ble_gap_disconnect();
        WDG_SystemReset(RESET_ALL, DFU_SWITCH_TO_OTA);
    }
    else
    {
        TUYA_BLE_LOG_ERROR("ota crc error!");
        tuya_ota_status = TUYA_OTA_STATUS_NONE;
        //tuya_ota_init_disconnect();
        memset(&dfu_settings, 0, sizeof(dfu_settings_t));
        dfu_settings_write();
    }

}



static void tuya_ota_end_req(uint8_t *recv_data, uint32_t recv_len)
{
    uint8_t p_buf[2];
    uint8_t payload_len = 0;
    uint8_t encry_mode = 0;


    if (tuya_ota_status == TUYA_OTA_STATUS_NONE)
    {
        TUYA_BLE_LOG_ERROR("current ota status is TUYA_OTA_STATUS_NONE!");
        return;
    }

    on_data_write_request_sched();

}


void tuya_ota_proc(uint16_t cmd, uint8_t *recv_data, uint32_t recv_len)
{
    TUYA_BLE_LOG_DEBUG("ota cmd : 0x%04x , recv_len : %d", cmd, recv_len);
    switch (cmd)
    {
    case TUYA_BLE_OTA_REQ:
        tuya_ota_start_req(recv_data, recv_len);
        break;
    case TUYA_BLE_OTA_FILE_INFO:
        tuya_ota_file_info_req(recv_data, recv_len);
        break;
    case TUYA_BLE_OTA_FILE_OFFSET_REQ:
        tuya_ota_offset_req(recv_data, recv_len);
        break;
    case TUYA_BLE_OTA_DATA:
        tuya_ota_data_req(recv_data, recv_len);
        break;
    case TUYA_BLE_OTA_END:
        tuya_ota_end_req(recv_data, recv_len);
        break;
    default:
        break;
    }

}

void tuya_ota_status_set(tuya_ota_status_t status)
{
    tuya_ota_status = status;
}


tuya_ota_status_t tuya_ota_status_get(void)
{
    return tuya_ota_status;
}

uint8_t tuya_ota_init_disconnect(void)
{
    if (tuya_ota_status != TUYA_OTA_STATUS_NONE)
    {
        dfu_settings_write();
        tuya_ota_status = TUYA_OTA_STATUS_NONE;
        current_package = 0;
        last_package = 0;
    }
    return 0;
}


bool unlock_flash_all(void)
{
    prev_bp_lv = 0;
    APP_PRINT_INFO0("**********[Flash Set] Flash unlock ***********");
    if (FLASH_SUCCESS == flash_sw_protect_unlock_by_addr_locked((0x00800000), &prev_bp_lv))
    {
        APP_PRINT_INFO1("[Flash Set] Flash unlock address = 0x800000, prev_bp_lv = %d", prev_bp_lv);
        return true;
    }
    return false;
}
void lock_flash(void)
{
    if (prev_bp_lv != 0xfe)
    {
        flash_set_block_protect_locked(prev_bp_lv);
    }
}
DATA_RAM_FUNCTION bool app_flash_wait_busy(void)
{
    uint8_t status = 0;
    uint32_t ctr = 0;
    bool ret;

    while (ctr++ <= 0x100000)
    {
        ret = flash_cmd_rx(0x05, 1, &status);

//        DFU_PRINT_INFO2("ret is %x,status is %x", ret, status);
        if (!ret)
        {
            goto wait_busy_fail;
        }

        if (!(status & BIT_STATUS_WIP))
        {
            return true;
        }
//        DFU_PRINT_INFO1("CNT is %x", ctr);
    }

wait_busy_fail:
    return false;
}

DATA_RAM_FUNCTION bool app_flash_cmd_tx(uint8_t cmd, uint8_t data_len, uint8_t *data_buf)
{
    bool retval = true;
    DFU_PRINT_INFO0("app_flash_cmd_tx");
    uint32_t ctrlr0 = SPIC->ctrlr0;
    uint32_t addr_len = SPIC->addr_length;

    spic_enable(DISABLE);
    spic_clr_multi_ch();
    spic_set_tx_mode();

    SPIC->addr_length = data_len;

    spic_set_dr(DATA_BYTE, cmd);

    while (data_len--)
    {
        spic_set_dr(DATA_BYTE, *data_buf++);
    }

    spic_enable(ENABLE);

    if (!spic_wait_busy())
    {
        retval = false;
    }

    spic_enable(DISABLE);
    DFU_PRINT_INFO1("app_flash_wait_busy ..%x", retval);
    if (retval == true && !app_flash_wait_busy())
    {
        retval = false;
    }


    //restore ctrl0 and addr_len register
    SPIC->ctrlr0 = ctrlr0;
    SPIC->addr_length = addr_len;

    return retval;
}
/**
 * @brief erase a sector of the flash.
 *
 * @param  addr          flash addr in sector to be erase.
 * @return  0 if erase successfully, error line number otherwise
*/
DATA_RAM_FUNCTION uint32_t flash_erase_sector(uint32_t addr)
{

    static uint8_t address[3];
    DFU_PRINT_INFO1("==> flash_erase_sector :%x \r\n", addr);
    address[0] = (addr >> 16) & 0xff;
    address[1] = (addr >> 8) & 0xff;
    address[2] = addr & 0xff;

    flstatus = flash_lock(FLASH_LOCK_USER_MODE_ERASE);//signal = os_lock();
    flash_write_enable();
    app_flash_cmd_tx(0x20, 3, address);
    flash_unlock(FLASH_LOCK_USER_MODE_ERASE);//os_unlock(signal);
    return 0;
}
/**
 * @brief  write specified image data with specified length to flash
 * @param  signature          signature to identify FW.
 * @param  offset             offset of the image.
 * @param  length             length of data.
 * @param  p_void             pointer to data.
 * @return 0 if write FW image successfully, error line number otherwise
*/
uint32_t sil_dfu_update(uint16_t signature, uint32_t offset, uint32_t length,
                        uint32_t/*void*/ *p_void)
{
    uint32_t result = 0;
    uint32_t dfu_base_addr;
    uint32_t start_addr;
    uint32_t s_val;

    DFU_PRINT_INFO1("==> dfu_update length:%d \r\n", length);

    if (length % 4)
    {
        result = __LINE__;
        goto L_Return;
    }

    if (p_void == 0)
    {
        result = __LINE__;
        goto L_Return;
    }
    /*get back up area address*/
    dfu_base_addr = get_temp_ota_bank_addr_by_img_id((T_IMG_ID)signature);
    DFU_PRINT_INFO1("==> dfu_update dfu_base_addr:0x%x \r\n", dfu_base_addr);
    if (dfu_base_addr == 0)
    {
        result = __LINE__;
        goto L_Return;
    }
//      dfu_base_addr&= 0xffffff;
    /*if it's start_packet*/
    if (offset == 0)
    {
        /*ASSERT(length>=sizeof(image_header_t));*/
        T_IMG_CTRL_HEADER_FORMAT *p_header = (T_IMG_CTRL_HEADER_FORMAT *) p_void;
        p_header->ctrl_flag.flag_value.not_ready = 0x1; /*make sure image is not ready, will use it later*/
        APP_PRINT_INFO3("dfu_update New Image Header:0x%08x, Signature:0x%08x, dfu_base_addr:0x%08x",
                        length, signature, dfu_base_addr);
    }

    if ((offset % FMC_SEC_SECTION_LEN) == 0)   //new page starts
    {
        erase_error = flash_erase_sector(dfu_base_addr + offset);
    }
    else  // cross page
    {
        if ((offset / FMC_SEC_SECTION_LEN) != ((offset + length) / FMC_SEC_SECTION_LEN))
        {
            erase_error = flash_erase_sector((dfu_base_addr + offset + length) & ~(FMC_SEC_SECTION_LEN - 1));
        }
    }
    start_addr = dfu_base_addr + offset;
    // APP_PRINT_INFO3("start_addr:0x%08x, *p_void:0x%08x, *p_void:0x%08x",start_addr, *p_void, *(p_void + 1));
    for (int i = 0; i < length; i = i + 4)
    {
        flash_auto_write(start_addr + i, *(uint32_t *)p_void);

        s_val = flash_auto_read(start_addr + i | FLASH_OFFSET_TO_NO_CACHE);
//        if(flash_auto_read_locked(start_addr + i | FLASH_OFFSET_TO_NO_CACHE,&s_val))
//        {
//            result = __LINE__;
//            goto L_Return;
//        }
        if (s_val != *(uint32_t *)p_void)
        {
            APP_PRINT_INFO3("s_val:0x%08x, *p_void:0x%08x, i:0x%08x",
                            s_val, *(uint32_t *)p_void, i);
            result = __LINE__;
            goto L_Return;
        }
        else
        {
            p_void++;
        }
    }

L_Return:

    DFU_PRINT_INFO1("<==dfu_update result:%d \r\n", result);
    return result;
}

