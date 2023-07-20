
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <bt_task.h>
#include "board.h"
#include "trace.h"
#include "profile_client.h"
#include "ancs.h"
#include "app_gatts_client.h"
#include "app_ble_bas.h"
#ifdef RTK_TUYA_BLE_SDK
#include "tuya_ble_app_demo.h"
#endif


T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_APP_CB_DATA *p_param = (T_SERVER_APP_CB_DATA *)p_data;
        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
            APP_PRINT_INFO1("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                            p_param->event_data.service_reg_result);
            break;

        case PROFILE_EVT_SEND_DATA_COMPLETE:
            APP_PRINT_INFO5("PROFILE_EVT_SEND_DATA_COMPLETE: conn_id %d, cause 0x%x, service_id %d, attrib_idx 0x%x, credits %d",
                            p_param->event_data.send_data_result.conn_id,
                            p_param->event_data.send_data_result.cause,
                            p_param->event_data.send_data_result.service_id,
                            p_param->event_data.send_data_result.attrib_idx,
                            p_param->event_data.send_data_result.credits);
            if (p_param->event_data.send_data_result.cause == GAP_SUCCESS)
            {
                APP_PRINT_INFO0("PROFILE_EVT_SEND_DATA_COMPLETE success");
            }
            else
            {
                APP_PRINT_ERROR0("PROFILE_EVT_SEND_DATA_COMPLETE failed");
            }
            break;

        default:
            break;
        }
    }

    return app_result;
}



void app_le_profile_init(void)
{
    server_init(2);

    app_ble_bas_init();
    // app_ble_hid_init();
#ifdef RTK_TUYA_BLE_SDK
    app_ble_tuya_init();
#endif
#ifdef RTK_MI_WEAR
    extern void app_miwear_init(void);
    app_miwear_init();
#endif
#ifdef RTK_PRIVATE_SERVICE
    extern void app_ble_bwps_init(void);
    app_ble_bwps_init();
#endif

    server_register_app_cb(app_profile_callback);

#ifdef RTL8762G
#else
    client_init(2);
    ancs_init(1);
    app_gatts_client_init();
#endif
#if 0
    gcs_client_id = gcs_add_client(gcs_client_callback, APP_MAX_LINKS, 256);
    client_register_general_client_cb(gcs_client_callback);
#endif

}

/** @} */ /* End of group PERIPH_APP_TASK */


