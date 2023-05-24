
#ifndef  OTA_H_
#define  OTA_H_

#include <stdint.h>


#define TUYA_OTA_VERSION 3

#define TUYA_OTA_TYPE    0



typedef enum
{
    TUYA_OTA_STATUS_NONE,
    TUYA_OTA_STATUS_START,
    TUYA_OTA_STATUS_FILE_INFO,
    TUYA_OTA_STATUS_FILE_OFFSET,
    TUYA_OTA_STATUS_FILE_DATA,
    TUYA_OTA_STATUS_FILE_END,
    TUYA_OTA_STATUS_MAX,
} tuya_ota_status_t;

/*
typedef struct
{
   uint16_t cmd;
   uint16_t data_length;
   uint8_t  *data;
}tuya_ota_cmd_t;
*/

void tuya_ota_proc(uint16_t cmd, uint8_t *recv_data, uint32_t recv_len);

uint32_t tuya_ota_init(void);

uint8_t tuya_ota_init_disconnect(void);

void tuya_ota_status_set(tuya_ota_status_t status);

tuya_ota_status_t tuya_ota_status_get(void);


#endif /* OTA_H_ */





