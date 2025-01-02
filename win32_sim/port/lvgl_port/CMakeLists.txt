


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
    target_compile_definitions(gui PRIVATE DRV_LCD_WIDTH=480 DRV_LCD_HIGHT=480 DRV_PIXEL_BITS=16)
endif()
set(lvgl_port_src
${CMAKE_CURRENT_SOURCE_DIR}/lv_port_indev.c
${CMAKE_CURRENT_SOURCE_DIR}/lv_port_disp.c
${CMAKE_CURRENT_SOURCE_DIR}/lv_port_fs.c
${CMAKE_CURRENT_SOURCE_DIR}/lv_port_indev.c
 CACHE INTERNAL  "gui sources ")

set(lvgl_port_inc
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32/include/SDL2/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32/include/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/
CACHE INTERNAL "gui headers ")
add_definitions(-DLV_CONF_INCLUDE_SIMPLE)



