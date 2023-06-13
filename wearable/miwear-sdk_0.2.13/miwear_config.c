#include "miwear_config.h"

#ifndef MIWEAR_PRODUCT_ID
#define MIWEAR_PRODUCT_ID 9050
#endif

/**
 * Log error   level    :1
 * Log warning level    :2
 * Log info    level    :3
 * Log debug   level    :4
 */
#ifndef MIWEAR_LOG_LEVEL
#define MIWEAR_LOG_LEVEL 4
#endif

/**
 * Verify Mode
 *
 * MIWEAR_VERIFY_MODE_PSK
 * MIWEAR_VERIFY_MODE_LOCAL
 *
 */
#ifndef MIWEAR_VERIFY_MODE
#define MIWEAR_VERIFY_MODE MIWEAR_VERIFY_MODE_LOCAL
#endif

/**
 * OOB Mode
 *
 * MIWEAR_OOB_MODE_NUMERIC_COMPARISON
 * MIWEAR_OOB_MODE_DYNAMIC_CODE
 * MIWEAR_OOB_MODE_BUTTON_CONFIRM
 *
 */
#ifndef MIWEAR_OOB_MODE
#define MIWEAR_OOB_MODE                                                        \
    (MIWEAR_OOB_MODE_DYNAMIC_CODE | MIWEAR_OOB_MODE_BUTTON_CONFIRM)
//#define MIWEAR_OOB_MODE (MIWEAR_OOB_MODE_BUTTON_CONFIRM)
#endif

miwear_config_t miwear_config =
{
    .miwear_log_level = MIWEAR_LOG_LEVEL,
    .pid = MIWEAR_PRODUCT_ID,
    .verify_mode = MIWEAR_VERIFY_MODE_LOCAL,//MIWEAR_VERIFY_MODE,
    .oob_mode = MIWEAR_OOB_MODE,
    .support_ble_bond_apply = 1,
    .support_silent_ota = 0,
    .oversea = 0,
};
