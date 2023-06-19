#include <bas.h>
#include "app_ble_hid.h"
#include "hids_ms.h"
#include "trace.h"
#include "ble_gap_msg.h"
#ifdef OS_RTTHREAD
#include "rtthread.h"
#endif



static T_SERVER_ID hids_srv_id;


static T_APP_RESULT app_hid_callback(T_SERVER_ID service_id, void *p_data)
{
    if (service_id == hids_srv_id)
    {

    }
    return APP_RESULT_SUCCESS;
}

static void app_hid_msg(T_IO_MSG *p_gap_msg)
{
    APP_PRINT_INFO1("line = %d \n", __LINE__);
    APP_PRINT_INFO1("app_handle_gap_msg: subtype %d \n", p_gap_msg->subtype);
}

void app_ble_hid_init(void)
{
    hids_srv_id  = hids_add_service(app_hid_callback);
    le_msg_handler_cback_register(app_hid_msg);
}

#ifdef OS_RTTHREAD
static void hids_send(uint8_t argc, char **argv)
{
    uint8_t len = argc - 1;
    uint8_t *report = rt_malloc(argc);
    for (uint8_t i = 0; i < len; i++)
    {
        report[i] = atoi(argv[i + 1]);
    }

    hids_send_report(0, hids_srv_id, 4, report, len);
    rt_memset(report, 0x00, len);
    hids_send_report(0, hids_srv_id, 4, report, len);

}
MSH_CMD_EXPORT(hids_send, hid send test);
#endif


