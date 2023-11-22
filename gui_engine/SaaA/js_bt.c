#include "js_user.h"

static void sim_bt_found(const jerry_value_t value)
{
    return;
}

DECLARE_HANDLER(bluetooth_open)
{
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_scan)
{
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_stopscan)
{
    return jerry_create_undefined();
}
DECLARE_HANDLER(bluetooth_close)
{
    return jerry_create_undefined();
}

DECLARE_HANDLER(bluetooth_onFound)
{
    if (args_cnt >= 1 && jerry_value_is_function(args[0]))
    {
        sim_bt_found(args[0]);
    }
    return jerry_create_undefined();
}