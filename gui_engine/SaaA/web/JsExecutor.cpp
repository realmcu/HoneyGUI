#include "JsExecutor.h"
#include <iostream>

namespace WebParser
{

JsExecutor::JsExecutor()
{
    /* JERRY_ENABLE_EXTERNAL_CONTEXT */
    jerry_port_set_default_context(jerry_create_context(PKG_JMEM_HEAP_SIZE * 1024, context_alloc,
                                                        NULL));

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);
    global_object_ = jerry_create_undefined();
}

JsExecutor::~JsExecutor()
{
    jerry_release_value(global_object_);
    jerry_cleanup();
}

void JsExecutor::executeScript(const std::string &script)
{
    jerry_value_t parsed_code = jerry_parse(NULL, 0,
                                            reinterpret_cast<const jerry_char_t *>(script.c_str()), script.length(), JERRY_PARSE_NO_OPTS);
    if (jerry_value_is_error(parsed_code))
    {
        std::cerr << "Error parsing script" << std::endl;
        jerry_release_value(parsed_code);
        return;
    }

    jerry_value_t result = jerry_run(parsed_code);
    jerry_release_value(parsed_code);
    if (jerry_value_is_error(result))
    {
        std::cerr << "Error executing script" << std::endl;
    }
    jerry_release_value(result);
}

} // namespace WebParser
