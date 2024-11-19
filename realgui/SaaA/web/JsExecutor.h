#pragma once
#include <string>
#include <jerryscript.h>
#include <jerry_util.h>
#include "../3rd/js/js-port/config.h"
namespace WebParser
{

class JsExecutor
{
public:
    JsExecutor();
    ~JsExecutor();

    void executeScript(const std::string &script);

private:
    jerry_value_t global_object_;

    static void *context_alloc(size_t size, void *cb_data_p)
    {
        void *js_buffer = NULL;
#ifdef RTL8763EP
        return malloc(size);
#elif defined RTL87x2G
        js_buffer = gui_lower_malloc((PKG_JMEM_HEAP_SIZE + 100) * 1024);
#elif defined RTL8762D
        js_buffer = (void *)(0x6900000);

#elif defined __WIN32
        static uint8_t buffer[(PKG_JMEM_HEAP_SIZE + 100) * 1024] = {0};
        js_buffer = buffer;
#else
        js_buffer = malloc(size);
#endif

        return (void *)js_buffer;
    }
};

} // namespace WebParser
