
#include <gatts_client.h>
#include "trace.h"
#include "app_gatts_client.h"
#include "ble_gap_msg.h"
#include <ancs_client.h>


static T_CLIENT_ID gatts_client_id;


/**
 * @brief  Callback will be called when data sent from profile client layer.
 * @param  client_id the ID distinguish which module sent the data.
 * @param  conn_id connection ID.
 * @param  p_data  pointer to data.
 * @retval   result @ref T_APP_RESULT
 */
T_APP_RESULT app_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    APP_PRINT_INFO2("app_client_callback: client_id %d, conn_id %d",
                    client_id, conn_id);
    if (client_id == gatts_client_id)
    {
        T_GATTS_CLIENT_CB_DATA *p_gatts_client_cb_data = (T_GATTS_CLIENT_CB_DATA *)p_data;
        uint16_t value_size;
        uint8_t *p_value;
        uint16_t start_handle = 0;
        uint16_t end_handle = 0;
        switch (p_gatts_client_cb_data->cb_type)
        {
        case GATTS_CLIENT_CB_TYPE_DISC_STATE:
            switch (p_gatts_client_cb_data->cb_content.disc_state)
            {
            case DISC_GATTS_DONE:
                APP_PRINT_INFO0("app_client_callback: discover gatt service procedure done.");
                gatts_client_set_service_changed_ind(conn_id, true);
                break;
            case DISC_GATTS_FAILED:
                /* Discovery Request failed. */
                APP_PRINT_INFO0("app_client_callback: discover gatt service simp request failed.");
                break;
            default:
                break;
            }
            break;

        case GATTS_CLIENT_CB_TYPE_WRITE_RESULT:
            switch (p_gatts_client_cb_data->cb_content.write_result.type)
            {
            case GATTS_WRITE_SERVICE_CHANGED_IND_ENABLE:
                APP_PRINT_INFO1("GATTS_WRITE_SERVICE_CHANGED_IND_ENABLE: write result 0x%x",
                                p_gatts_client_cb_data->cb_content.write_result.cause);
                ancs_start_discovery(conn_id);
                break;
            case GATTS_WRITE_SERVICE_CHANGED_IND_DISABLE:
                APP_PRINT_INFO1("GATTS_WRITE_SERVICE_CHANGED_IND_DISABLE: write result 0x%x",
                                p_gatts_client_cb_data->cb_content.write_result.cause);
                break;
            default:
                break;
            }
            break;

        case GATTS_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
            value_size = p_gatts_client_cb_data->cb_content.notif_ind_data.data.value_size;
            p_value = p_gatts_client_cb_data->cb_content.notif_ind_data.data.p_value;
            switch (p_gatts_client_cb_data->cb_content.notif_ind_data.type)
            {
            case GATTS_SERVICE_CHANGED_INDICATE:
                start_handle = (*p_value) + ((*(p_value + 1)) << 8);
                end_handle = (*(p_value + 2)) + ((*(p_value + 3)) << 8);
                APP_PRINT_INFO3("GATTS_SERVICE_CHANGED_INDICATE: value_size %d, start handle 0x%x, end handle 0x%x",
                                value_size, start_handle, end_handle);
                //start_ancs_rediscovery_timer();
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    return result;
}



void app_gatts_client_init(void)
{
    gatts_client_id  = gatts_add_client(app_client_callback, 1);
}
