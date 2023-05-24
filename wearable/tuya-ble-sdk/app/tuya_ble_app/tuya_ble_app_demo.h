#ifndef TUYA_BLE_APP_DEMO_H_
#define TUYA_BLE_APP_DEMO_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "profile_server.h"


#define APP_PRODUCT_ID      "h6zf5gif"//"xxxxxxxx"
//#define APP_PRODUCT_ID      "m6aekcua"//"xxxxxxxx"

#define APP_BUILD_FIRMNAME  "tuya_ble_sdk_2.0_demo_rtl8762c"

// firmware version
#define TY_APP_VER_NUM       0x0100
#define TY_APP_VER_STR       "1.0"

// hardware version
#define TY_HARD_VER_NUM      0x0100
#define TY_HARD_VER_STR      "1.0"


// ble adv interval, unit: ms
#define TUYA_BLE_APP_DEFAULT_ADV_INTERVAL   ( 200 )

// ble conn interval, unit:ms. range: 7.5~4000ms
#define TUYA_BLE_APP_CONN_INTERVAL_MIN      ( 80 )
#define TUYA_BLE_APP_CONN_INTERVAL_MAX      ( 100 )


int tuya_ble_app_init(void);
void app_ble_tuya_init(void);
T_SERVER_ID get_tuya_srv_id(void);


#ifdef __cplusplus
}
#endif

#endif // 

