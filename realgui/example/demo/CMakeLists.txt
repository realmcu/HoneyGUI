# Define the function
function(target_exists TARGET_NAME RESULT_VAR)
    if (TARGET ${TARGET_NAME})
        set(${RESULT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${RESULT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Check if MyExecutable exists
target_exists(gui TARGET_FOUND)

if (TARGET_FOUND)
    target_compile_definitions(gui PRIVATE LV_USE_CMAKE_KCONFIG)

if(CONFIG_LVGL_WIDGETS_DEMO)
message(STATUS "*****WIDGETS DEMO*****")
target_compile_definitions(gui PRIVATE LVGL_WIDGETS_DEMO_ENABLED=1)
elseif(CONFIG_LVGL_BENCHMARK)
message(STATUS "*****BENCHMARK*****")
target_compile_definitions(gui PRIVATE LVGL_BENCHMARK_ENABLED=1)
elseif(CONFIG_LVGL_ARC_2)
message(STATUS "*****ARC 2*****")
target_compile_definitions(gui PRIVATE LVGL_ARC_2_ENABLED=1)
endif()
else()
    message(WARNING "Target 'gui' not found. Definitions not applied.")
endif()


file(GLOB gui_example_sources 
app_ui_lvgl.c 
 )


set(gui_example_sources ${gui_example_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_example_headers ./)

set(gui_example_headers ${gui_example_headers} 

${CMAKE_CURRENT_SOURCE_DIR}
CACHE INTERNAL  "headers")


if(gui_debug_enable)  
   message(--------------example-------------)
foreach(ITEM ${gui_example_sources})
   message(${ITEM})
endforeach()
endif()

