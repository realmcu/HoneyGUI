set(jerry_core_dir jerry-core)
set(jerry_ext_dir jerry-ext)
file(GLOB gui_3rd_js_sources 
${jerry_core_dir}/*.c
${jerry_core_dir}/api/*.c
${jerry_core_dir}/debugger/*.c
${jerry_core_dir}/ecma/base/*.c
${jerry_core_dir}/ecma/builtin-objects/*.c
${jerry_core_dir}/ecma/builtin-objects/typedarray/*.c
${jerry_core_dir}/ecma/operations/*.c
${jerry_core_dir}/jcontext/*.c
${jerry_core_dir}/jmem/*.c
${jerry_core_dir}/jrt/*.c
${jerry_core_dir}/lit/*.c
${jerry_core_dir}/parser/js/*.c
${jerry_core_dir}/parser/regexp/*.c
${jerry_core_dir}/vm/*.c
${jerry_ext_dir}/arg/*.c
${jerry_ext_dir}/handler/*.c
${jerry_ext_dir}/include/*.c
${jerry_ext_dir}/module/*.c
js-port/port.c
js-port//jerry_util.c
js-port/jerry_console.c
js-port/jerry_utf8.c
)

set(gui_3rd_js_sources ${gui_3rd_js_sources} CACHE INTERNAL  "sources")
set(jerry_core_header ${CMAKE_CURRENT_SOURCE_DIR}/jerry-core)
set(jerry_ext_header ${CMAKE_CURRENT_SOURCE_DIR}/jerry-ext)
set(gui_3rd_js_headers ${CMAKE_CURRENT_SOURCE_DIR}
${jerry_core_header}/api
${jerry_core_header}/debugger
${jerry_core_header}/ecma/base
${jerry_core_header}/ecma/builtin-objects
${jerry_core_header}/ecma/builtin-objects/typedarray
${jerry_core_header}/ecma/operations
${jerry_core_header}/include
${jerry_core_header}/jcontext
${jerry_core_header}/jmem
${jerry_core_header}/jrt
${jerry_core_header}/lit
${jerry_core_header}/parser/js
${jerry_core_header}/parser/regexp
${jerry_core_header}/vm
${jerry_ext_header}//arg
${jerry_ext_header}/handler
${jerry_ext_header}/include
${jerry_ext_header}/module
${CMAKE_CURRENT_SOURCE_DIR}/js-port
 CACHE INTERNAL  "headers")
target_compile_definitions(gui PRIVATE 
JERRY_JS_PARSER 
JERRY_ENABLE_EXTERNAL_CONTEXT
JERRY_ENABLE_ERROR_MESSAGES
JERRY_ENABLE_LOGGING
)

if(gui_debug_enable)  
   message(--------------gui 3rd jerryscript-------------)
foreach(ITEM ${gui_3rd_js_sources})
   message(${ITEM})
endforeach()
endif()

