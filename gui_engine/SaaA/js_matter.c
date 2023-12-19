#include "js_user.h"

DECLARE_HANDLER(dataRst)
{
#ifdef RTL87x2G
    // Factory reset matter
#ifdef BUILD_USING_MATTER_SWITCH
    gui_log("Erase matter flash \n");
    Bee4_KvsClean();
    otPlatFlashErase(NULL, 0);
#endif
    gui_log("Reset from ui\n");
    WDG_SystemReset(RESET_ALL, RESET_REASON_FACTORY_RESET);
#endif
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
