
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
    target_compile_definitions(gui PRIVATE ENABLE_RTK_GUI_800_480_DEMO DRV_LCD_WIDTH=800 DRV_LCD_HIGHT=480 DRV_PIXEL_BITS=16)
else()
    message(WARNING "Target 'gui' not found. Definitions not applied.")
endif()
# Define macros for the specific target


file(GLOB gui_example_sources 
*.cpp
*.c )
if(NOT CONFIG_LVGL)
    list(REMOVE_ITEM gui_example_sources ${CMAKE_CURRENT_SOURCE_DIR}/app_lvgl.c)
endif()
list(REMOVE_ITEM gui_example_sources 
${CMAKE_CURRENT_SOURCE_DIR}/app_dashboard_480_272.c)
list(REMOVE_ITEM gui_example_sources 
${CMAKE_CURRENT_SOURCE_DIR}/app_dashboard_800_400.c)
set(gui_example_sources ${gui_example_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_example_headers ./)

set(gui_example_headers ${gui_example_headers} 
${CMAKE_CURRENT_SOURCE_DIR}/root_image_hongkong
${CMAKE_CURRENT_SOURCE_DIR}
CACHE INTERNAL  "headers")


if(gui_debug_enable)  
   message(--------------example-------------)
foreach(ITEM ${gui_example_sources})
   message(${ITEM})
endforeach()
endif()

