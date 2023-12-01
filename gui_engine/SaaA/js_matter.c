#include "js_user.h"

DECLARE_HANDLER(dataRst)
{
    // TODO: Reset matter


    return jerry_create_undefined();
}

void js_matter_init()
{
    jerry_value_t global_obj = jerry_get_global_object();
    jerry_value_t matter = jerry_create_object();
    js_set_property(global_obj, "matter", matter);
    REGISTER_METHOD(matter, dataRst);
    jerry_release_value(global_obj);
}
