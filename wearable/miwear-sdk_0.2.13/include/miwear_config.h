#ifndef __MIWEAR_CONFIG_H__
#define __MIWEAR_CONFIG_H__

#include "miwear_type.h"
#include <stdint.h>

typedef struct
{
    uint16_t pid;
    uint8_t verify_mode;
    uint8_t oob_mode;
    uint8_t miwear_log_level : 3;
    uint8_t support_ble_bond_apply : 1;
    uint8_t oversea : 1;
    uint8_t reserve : 3;
    uint32_t device_capability;
} miwear_config_t;

extern miwear_config_t miwear_config;

#endif /* __MIWEAR_CONFIG_H__ */
