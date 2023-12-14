#include "js_user.h"
#ifdef RTL87x2G
#include "wdg.h"
#include "rtl_wdg.h"
#endif
DECLARE_HANDLER(dataRst)
{
    // TODO: Reset matter

    gui_log("reset from ui\n");
#ifdef RTL87x2G
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
