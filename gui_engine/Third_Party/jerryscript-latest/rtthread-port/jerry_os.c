#include <rtthread.h>

#include "jerry_util.h"

DECLARE_HANDLER(ps)
{
    /* list thread information */
#ifdef HOST_BUILD
    printf("list thread\n");
#else
    extern long list_thread(void);
    list_thread();
#endif

    return jerry_create_undefined();
}

jerry_value_t js_module_rtthread_init(void)
{
    jerry_value_t rtthread_obj = jerry_create_object();
    REGISTER_METHOD(rtthread_obj, ps);

    return rtthread_obj;
}
//JS_MODULE(os, js_module_rtthread_init);
