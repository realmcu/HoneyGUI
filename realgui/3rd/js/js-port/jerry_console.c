#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "jerry_util.h"

DECLARE_HANDLER(dir)
{
    int index;
    for (index = 0; index < args_cnt; index ++)
    {
        js_value_dump(args[index]);
        printf("\n");
    }

    return jerry_create_undefined();
}

int js_console_init(void)
{
    jerry_value_t console = jerry_create_object();
    jerry_value_t global_obj = jerry_get_global_object();

    REGISTER_METHOD_ALIAS(console, log, jerryx_handler_print);
    REGISTER_METHOD_ALIAS(console, dir, dir_handler);

    js_set_property(global_obj, "console", console);

    jerry_release_value(global_obj);
    jerry_release_value(console);

    return 0;
}
