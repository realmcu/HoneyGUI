#include "js_user.h"

DECLARE_HANDLER(startSpeed)
{
    gui_log("enter startSpeed\n");
    extern void iperf_start(char *ip, char *port);
    //iperf_start(NULL, NULL);
    return jerry_create_undefined();
}
DECLARE_HANDLER(stopSpeed)
{
    gui_log("enter stopSpeed\n");
    extern void iperf_stop(void);
    //iperf_stop();
    return jerry_create_undefined();
}
DECLARE_HANDLER(readSpeed)
{
    gui_log("enter readSpeed\n");
    extern uint32_t iperf_get_current_speed(void);
    uint32_t speed = 1000;
    //speed = iperf_get_current_speed();
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