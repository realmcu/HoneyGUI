#include "js_user.h"
#include "js_extern_io.h"

#if defined (TARGET_RTL8773E)
#ifdef ENABLE_RCU
#include "led_aip33a22.h"
#include "app_msg.h"
#include "app_scatternet_task.h"
extern T_IO_MSG key_msg_table[];
extern T_IO_MSG ble_msg_table[] ;

// static uint32_t color_tab[] = {0x00FDBF56, 0x00FFD794, 0x00D1D1E6, 0x003E4889, 0x00DDC4BD};

// kitchen, living, media, game, bedroom
static uint32_t color_tab[] = {0x00FF6000, 0x00FF8C00, 0x007044FF, 0x004E4AC4, 0x00FF8A7A};
#endif
#endif



/*============================================================================*
 *                           Wi-Fi Manage Functions
 *============================================================================*/


DECLARE_HANDLER(wait)
{
    gui_log("RCU wait\n");
#ifdef __arm__
    os_delay(10);
#else
    gui_log("rcu delay\n");
#endif
    return jerry_create_undefined();
}

DECLARE_HANDLER(key)
{
    uint32_t idx = 0;
    idx = jerry_get_number_value(args[0]);
    gui_log("RCU key %d\n", idx);
#ifdef __arm__
#ifdef ENABLE_RCU
    gui_send_msg_to_ble_app_task(&key_msg_table[idx]);
#endif
#else
    gui_log("send key msg\n");
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(config)
{
    uint32_t idx = 0;
    idx = jerry_get_number_value(args[0]);
    gui_log("RCU config %d\n", idx);
#ifdef __arm__
#ifdef ENABLE_RCU
    gui_send_msg_to_ble_app_task(&ble_msg_table[idx]);
    led_set_color(color_tab[idx]);
#endif
#else
    gui_log("send config msg\n");
#endif
    return jerry_create_undefined();
}



// extern module abstract
DECLARE_HANDLER(RCU)
{
    gui_log("enter RCU\n");
    REGISTER_METHOD(this_value, wait);
    REGISTER_METHOD(this_value, key);
    REGISTER_METHOD(this_value, config);
    return jerry_create_undefined();
}

void js_rcu_init(void)
{
    jerry_value_t global_obj = jerry_get_global_object();
    REGISTER_METHOD(global_obj, RCU);

}


