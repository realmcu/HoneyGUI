#include "js_user.h"

DECLARE_HANDLER(startSpeed)
{
    gui_log("enter startSpeed\n");
#ifdef ENABLE_WIFI_NIC
    extern void iperf_start(char *ip, char *port);
    iperf_start(NULL, NULL);
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(stopSpeed)
{
    gui_log("enter stopSpeed\n");
#ifdef ENABLE_WIFI_NIC
    extern void iperf_stop(void);
    iperf_stop();
#endif
    return jerry_create_undefined();
}
DECLARE_HANDLER(readSpeed)
{
//    gui_log("enter readSpeed\n");
    static uint32_t speed = 10;
#ifdef ENABLE_WIFI_NIC
    extern uint32_t iperf_get_current_speed(void);
    speed = iperf_get_current_speed();
#else
    speed++;
    speed = speed > 2000 ?  10 : speed;
#endif
    return jerry_create_number((double)speed);
}
void wifi_init()
{

    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t wifi = jerry_create_object();
    js_set_property(global_obj, "wifi", wifi);
    REGISTER_METHOD(wifi, startSpeed);
    REGISTER_METHOD(wifi, stopSpeed);
    REGISTER_METHOD(wifi, readSpeed);
    jerry_release_value(global_obj);
}